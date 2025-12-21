#include "ecSTM32F4v2.h"
// #include "ecADC.h"

// IR parameter
uint32_t value;

void setup(void);

int main(void){ 
    setup();

    while(1){
        printf("value = %d \r\n", value);
        printf("\r\n");
        delay_ms(1000);
    }
}

// Initialization 
void setup(void){	
    RCC_PLL_init();      // System Clock = 84.00 MHz
    UART2_init();        // UART2 Init
    SysTick_init();      // SysTick Init
    ADC_init(PB_0);      // Default: HW triggered by TIM3 counter @ 1.00 ms
}

// ADC Interrupt - End-of-Conversion
void ADC_IRQHandler(void){
    if(is_ADC_EOC()) {
        // if(is_ADC_OVR()) clear_ADC_OVR();
        value = ADC_read();
        // Reading ADC DR clears EOC flag
    }		
}
/*
(a) ADC_init(PB_0) 내부 동작 (ecADC2.c)

핀 → 채널 매핑

ecPinmap(PB_0, &port, &pin);

ADC_pinmap(PB_0, &chN); → PB_0 = ADC1_CH8.

GPIO 아날로그 설정

GPIO_init(PB_0, ANALOG);

MODER = 11 (Analog mode)

GPIO_pupd(PB_0, EC_NONE);

PUPDR = 00 (no pull).

ADC 클록 및 분주

RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

ADC->CCR |= ADC_CCR_ADCPRE_0; 등으로 PCLK2 분주 설정(예: /4, /8).

해상도, 샘플링 시간

ADC1->CR1 &= ~ADC_CR1_RES; → 12-bit (0~4095).

ADC1->SMPRx 에서 CH8에 충분한 샘플링 시간 설정(예: 84.00 MHz 기준 안정 측정).

연속 변환 모드

ADC1->CR2 |= ADC_CR2_CONT; → Continuous conversion enable.

Regular Group 설정 (단일 채널)

ADC1->SQR1 &= ~ADC_SQR1_L; → L=0000, 채널 수 1개.

ADC1->SQR3 의 SQ1에 chN(=8) 기록 → 첫 채널.

ADC1->CR1 &= ~ADC_CR1_SCAN; → Scan mode off (single channel).

인터럽트, NVIC

ADC1->CR1 |= ADC_CR1_EOCIE; → EOC interrupt enable.

NVIC_SetPriority(ADC_IRQn,2);

NVIC_EnableIRQ(ADC_IRQn);

하드웨어 트리거 설정 (ADC_trigger)

ADC_trigger(TIM3, 1, RISE_ADC); 내부:

TIM_init(TIM3,1); → TIM3를 1.00 ms 주기 Up-counter로 설정.

TIM3를 “master mode: TRGO = OC1REF” 등으로 구성해, 1.00 ms마다 트리거 펄스 발생.

ADC1->CR2의 EXTSEL에 TIM3_TRGO를 매핑, EXTEN을 rising edge로 설정.

ADC 시작

ADC_start()에서

ADC1->CR2 |= ADC_CR2_ADON;

필요 시 SWSTART 설정 등으로 첫 변환 시작.

(b) ADC_IRQHandler

is_ADC_EOC() → ADC1->SR의 EOC 비트 확인.

value = ADC_read();

ADC1->DR를 읽어서 0~4095 값을 반환.

DR 읽으면 자동으로 EOC 플래그 클리어 (데이터시트 동작).

결과적으로 PB_0에 연결된 IR 센서 전압이 1.00 ms마다 변환되고,

value 변수에는 항상 최신 샘플이 저장.













*/
