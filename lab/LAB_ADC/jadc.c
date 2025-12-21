#include "ecSTM32F4v2.h"
// #include "ecADC.h"

// IR parameter
uint32_t value1, value2;
PinName_t seqCHn[2] = {PB_0, PB_1};

void setup(void);

int main(void){ 
    setup();

    while(1){
        printf("value1 = %d \r\n", value1);
        printf("value2 = %d \r\n", value2);
        printf("\r\n");
        delay_ms(1000);
    }
}

// Initialization 
void setup(void){	
    RCC_PLL_init();                 // 84.00 MHz
    UART2_init();                   // UART2
    SysTick_init();                 // SysTick

    // JADC Init – TIM5 HW trigger, continuous injected scan
    JADC_init(PB_0);
    JADC_init(PB_1);

    // Injected group channel sequence 설정 (2채널)
    JADC_sequence(seqCHn, 2);
}

void ADC_IRQHandler(void){
    // if(is_ADC_OVR()) clear_ADC_OVR();	
    if(is_ADC_JEOC()){              // Injected sequence 변환 완료
        value1 = JADC_read(1);      // JDR1
        value2 = JADC_read(2);      // JDR2
        clear_ADC_JEOC();
    }
}
/*

(a) JADC_init(PB_0 / PB_1) 내부 동작

ADC_init과 비슷하지만, Injected Group(JSQR, JDRx) 전용 설정을 사용.

핀 설정

GPIO_init(pinName, ANALOG);

GPIO_pupd(pinName, EC_NONE);

ADC 기반 설정은 ADC_init와 비슷

클록, 해상도, 샘플링 시간, alignment (right aligned).

Injected Group 관련 설정 (중요)

ADC1->CR1 |= ADC_CR1_SCAN;        // Multi-channel scan mode enable
ADC1->CR2 &= ~ADC_CR2_ALIGN;      // Right alignment

ADC1->JSQR &= ~ADC_JSQR_JL;       // JL=00 → Injected sequence length 기본값
ADC1->JSQR &= ~ADC_JSQR_JSQ4;     // JSQ4 clear
ADC1->JSQR |= (chN & ADC_JSQR_JSQ4);  // 첫 채널을 JSQ4에 기록


여기서는 JADC_init을 채널마다 호출하지만, 실제 순서 설정은 나중에 JADC_sequence()에서 다시 JSQR를 구성.

JEOC 인터럽트 enable

ADC1->CR1 |= ADC_CR1_JEOCIE;

NVIC_SetPriority(ADC_IRQn,1); & NVIC_EnableIRQ(ADC_IRQn);

JADC 하드웨어 트리거 (JADC_trigger, TIM5)

JADC_trigger(TIM5, 1, RISE_ADC);


TIM5를 1.00 ms 주기로 설정하고,

TIM5_TRGO를 JEXTSEL에 연결, JEXTEN을 rising edge로 설정.

따라서 1.00 ms마다 Injected Group 변환 시작.

(b) JADC_sequence(seqCHn, 2)
void JADC_sequence(PinName_t *seqCHn, int seqCHnums){
    ADC1->CR2 &= ~ADC_CR2_ADON;        // ADC disable

    int chN[seqCHnums];

    if (seqCHnums > 1)
        ADC1->CR1 |= ADC_CR1_SCAN;     // Multi-channel scan

    for(int k=0; k<seqCHnums; k++)
        ADC_pinmap(seqCHn[k], &(chN[k]));
    
    // JSQR 길이 설정 (예: 2채널)
    // JL 비트 등으로 Injected sequence 길이 구성

    // 각 채널을 JSQx에 순서대로 기록 
    // (코드 중간 생략된 부분에서 chN[0] → JSQ4, chN[1] → JSQ3 등으로 배치)

    ADC_start();                        // ADON=1, 변환 시작 준비
}


핵심:

여러 핀을 받아서 대응 ADC 채널 번호(chN[])로 변환.

Injected 그룹 순서(JSQx)에 채널 번호를 기록.

이후 TIM5 트리거마다 JSQ 순서대로 연속 변환 → 결과는 각각 JDR1, JDR2에 저장.

(c) ADC_IRQHandler – JADC 읽기
if(is_ADC_JEOC()){          // Injected EOC
    value1 = JADC_read(1);  // JDR1
    value2 = JADC_read(2);  // JDR2
    clear_ADC_JEOC();
}


JADC_read(n)는 내부적으로 ADC1->JDRn 레지스터를 읽는다.

Injected 그룹은 채널별로 독립된 데이터 레지스터(JDR1~4) 를 가지므로,

multi-channel에서도 flag 하나로 “Sequence 완료” 감지 후,

각 채널 값을 별도의 JDR로 구분해서 읽을 수 있다.

3) ADC 다중 채널(Regular Group) 예제와의 비교

같은 페이지에 있는 ADC_sequence 예제:
ykkim.gitbook.io

// Regular Group 다중 채널 예시
uint32_t value1, value2;
int flag = 0;
PinName_t seqCHn[2] = {PB_0, PB_1};

...

void setup(void){
    ...
    ADC_init(PB_0);
    ADC_init(PB_1);
    ADC_sequence(seqCHn, 2);   // Regular group sequence 설정
}

void ADC_IRQHandler(void){
    if(is_ADC_OVR())
        clear_ADC_OVR();

    if(is_ADC_EOC()){          // Regular group sequence 완료
        if (flag == 0)
            value1 = ADC_read();  
        else if (flag == 1)
            value2 = ADC_read();

        flag = !flag;          // flag 토글
    }
}


Regular Group(ADC)은 모든 채널이 DR(단일 데이터 레지스터)을 공유한다.

EOC 마다 DR 값을 하나씩 읽어야 하고,

현재 “몇 번째 채널인지”는 SW에서 flag 등으로 추적해야 한다.

반면 Injected Group(JADC)은 JDR1~4 개별 레지스터가 있어서,

sequence 완료 후 JEOC 한 번으로 두 채널 값(value1, value2)을 동시에 구분해서 가져올 수 있다.

4) 실행 시 기대 동작

단일 채널 ADC 예제

PB_0(예: IR 센서) 전압이 1.00 ms마다 변환.

value 변수에 항상 최신 값(0~4095)이 저장되고,

1.00 초마다 TeraTerm에 value = XXXX 출력.

다중 채널 JADC 예제

PB_0, PB_1 두 채널이 1.00 ms마다 Injected Group으로 연속 변환.

JADC sequence가 끝날 때마다 JEOC가 발생하고,

ISR에서 value1(PB_0), value2(PB_1)를 각자 읽어 저장.

1.00 초마다 두 센서 값이 함께 출력.

5. 추가 점검 포인트

Ultrasonic, ADC/JADC 코드 디버깅할 때 특히 확인해야 할 것:

클록·타이머

RCC_PLL_init()로 SYSCLK가 84.00 MHz인지,

타이머 PSC/ARR 계산이 이 클록 기준으로 맞는지.

NVIC 설정

TIM4_IRQn, ADC_IRQn 우선순위와 enable 여부.

인터럽트 핸들러 이름(TIM4_IRQHandler, ADC_IRQHandler)이 스타트업 파일과 일치하는지.

핀 매핑

TRIG(PC_6/PA_6 등)가 실제로 해당 TIMx_CHn에 연결되는지(보드 datasheet, ecPinNames).

ECHO가 TIM4_CH1/2에 맞게 AF 설정되었는지.

ADC 핀(PB_0, PB_1)이 ADC1_CH8, CH9에 대응하는지.

플래그 처리

UIF, CCIF, EOC, JEOC, OVR 플래그를 꼭 읽고 클리어하는지.

하나라도 안 지우면 인터럽트가 폭주하거나 더 이상 들어오지 않을 수 있음.

단위 정리

Ultrasonic: ICAP_counter_us()에서 설정한 step(10.00 μs) 기준으로

timeInterval이 실제 ms 단위인지 다시 계산해 보는 것.

ADC: 12-bit 기준 변환

전압[V] = (ADC_value / 4095.00) × 3.30 처럼 실제 물리량으로 변환해 보는 것.

*/