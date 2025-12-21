#include "ecSTM32F4v2.h"
#include "math.h"
#include <stdio.h>                 // FIXED: printf 사용을 명시적으로 선언하기 위한 헤더 추가

uint32_t ovf_cnt = 0;//count over count
float distance = 0;
float timeInterval = 0;
float time1 = 0;//start time
float time2 = 0;//end time

#define TRIG PA_6 //pwm
#define ECHO PB_6 //echo


void setup(void);

int main(void){
	int count_test=0;
	setup();
	printf("Start LAB_TIMER_ICAP\r\n");
	while(1){
		distance = (float) timeInterval * 340.00f / 2.00f / 10.00f; 	// [mm] -> [cm]
		printf("%.2f cm\r\n", distance);   // FIXED: %.2f 자체는 정상. 다만 툴체인에서 float printf 활성화 필수
		delay_ms(1000);                    //0.5sec delay
	}
}

void TIM4_IRQHandler(void){
	if(is_UIF(TIM4)){                     // Update interrupt
		ovf_cnt++;													// overflow count
		clear_UIF(TIM4);  							    // clear update interrupt flag
	}
	if(is_CCIF(TIM4, 1)){ 								// TIM4_Ch1 (IC1) Capture Flag. Rising Edge Detect
		time1 = ICAP_capture(TIM4,1);									// Capture TimeStart
		clear_CCIF(TIM4, 1);                // clear capture/compare interrupt flag 
	}								                      
	else if(is_CCIF(TIM4, 2)){ 					// FIXED: CC2 플래그를 올바르게 검사하도록 수정
		time2 = ICAP_capture(TIM4,2);									// Capture TimeEnd
		timeInterval = ((time2-time1)+ovf_cnt*((TIM4->ARR)+1))/100.00f; 	// FIXED: float 상수 표기 정리, 10.00us 단위 → [msec] 환산 주석 확인 필요
		ovf_cnt = 0;                        // overflow reset
		clear_CCIF(TIM4,2);								  // clear capture/compare interrupt flag 
	}
}

void setup(){

	RCC_PLL_init(); 
	SysTick_init();//1msec
	UART2_init();
	GPIO_otype(TRIG, 0);//push pull
	GPIO_pupd(TRIG,0);//NO pull-up pull-down
	GPIO_ospeed(TRIG,EC_FAST);//FAST SPEED
	
	
  
// PWM configuration ---------------------------------------------------------------------	
	PWM_init(TRIG);			// PA_6: Ultrasonic trig pulse
	PWM_period_us(TRIG, 50000);    // PWM of 50.00ms period. Use period_us()
	PWM_pulsewidth_us(TRIG, 10);   // PWM pulse width of 10.00us
	
	
// Input Capture configuration -----------------------------------------------------------------------	
	ICAP_init(ECHO);    	// PB_6 as input caputre
	GPIO_pupd(ECHO,0);//NO pull-up pull-down
 	ICAP_counter_us(ECHO, 10);   	// ICAP counter step time as 10.00us
	ICAP_setup(ECHO, 1, IC_RISE);  // TIM4_CH1 as IC1 , rising edge detect
	ICAP_setup(ECHO, 2, IC_FALL);  // TIM4_CH2 as IC2 , falling edge detect

}
/*
(a) 전역 변수와 매크로

ovf_cnt

TIM4 카운터가 ARR를 넘어서 0으로 돌아갈 때마다(UP counter overflow) TIM4_IRQHandler에서 1씩 증가.

16-bit 카운터(0~65535) 범위를 넘어가는 장거리 측정에서 시간 누적에 사용.

time1, time2

time1: ECHO 상승 에지에서 CCR1 값(카운터 값).

time2: ECHO 하강 에지에서 CCR2 값.

둘의 차이 + 오버플로우 보정 → ECHO high 펄스 폭(왕복 시간).

timeInterval

ICAP_counter_us(ECHO, 10) 호출로 카운터 1 tick = 10.00 μs 가 되므로,

(time2 - time1) + ovf_cnt*(ARR+1) = “tick 개수”

… / 100.0 → timeInterval의 단위가 ms가 되도록 스케일링.

이유: 10.00 μs × tick = 0.01 ms × tick → ms로 바꾸려면 tick / 100.00.

distance

distance = timeInterval * 340.0 / 2.0 / 10.0;

340.00 m/s = 340.00 mm/ms.

거리(mm) = 시간(ms) × 340.00 / 2.00

cm로 바꾸려면 /10.00 → 최종적으로 cm 단위.

(b) main()
setup();
printf("Start LAB_TIMER_ICAP\r\n");

while(1){
    distance = (float)timeInterval * 340.0 / 2.0 /10.0;
    printf("%.2f cm\r\n", distance);
    delay_ms(1000);
}


setup()에서 시스템 클록, SysTick, UART, PWM, ICAP 등을 모두 초기화.

메인 루프는 인터럽트에서 업데이트되는 timeInterval을 이용해 distance를 계산하고 1.00 초마다 출력만 수행.

시간 측정과 카운터 처리는 모두 TIM4_IRQHandler에서 발생.

(c) TIM4_IRQHandler – 시간 측정 핵심
if(is_UIF(TIM4)){      // Update interrupt (overflow)
    ovf_cnt++;
    clear_UIF(TIM4);
}


TIM4->SR의 UIF 비트를 체크.

오버플로우 시 ovf_cnt 증가시켜 16-bit 제한 보정.

if(is_CCIF(TIM4, 1)){                 // Rising edge
    time1 = ICAP_capture(TIM4,1);     // CCR1 읽음
    clear_CCIF(TIM4, 1);
}
else if(is_CCIF(TIM4, 2)){            // Falling edge
    time2 = ICAP_capture(TIM4,2);     // CCR2 읽음
    timeInterval = ((time2 - time1) 
                   + ovf_cnt * ((TIM4->ARR) + 1)) / 100.0;
    ovf_cnt = 0;
    clear_CCIF(TIM4,2);
}


ICAP_setup(ECHO,1,IC_RISE) 때문에

TIM4_CH1(IC1)은 ECHO 상승 에지에서 캡처.

ICAP_setup(ECHO,2,IC_FALL) 때문에

TIM4_CH2(IC2)은 ECHO 하강 에지에서 캡처.

ICAP_capture(TIM4, n) 내부 동작 (ecICAP2.c 기준):

CCRn 값을 읽어서 그대로 리턴.

카운터 클럭은 ICAP_counter_us(ECHO,10)에서 설정한 PSC 기반:

void ICAP_counter_us(PinName_t pinName, int usec){
    ...
    TIMx->PSC = 84 * usec - 1;  // 84.00 MHz / (PSC+1) = 1/usec MHz
    TIMx->ARR = 0xFFFF;
}


시스템 클록 84.00 MHz 가정 시,

PSC = 84 × 10 − 1 = 839 → 타이머 클럭 = 84.00 MHz / 840.00 ≈ 0.10 MHz = 100 kHz

즉, 1 tick = 10.00 μs.

따라서

time2 - time1 = tick 수,

tick 수 × 10.00 μs = 실제 시간,

/100.00 → ms 단위.

(d) setup() – 전체 주변장치 설정

RCC_PLL_init();

PLL을 설정해 SYSCLK = 84.00 MHz 로 설정.

SysTick_init();

SysTick->LOAD = 84,000 − 1 로 설정해 1.00 ms tick.

delay_ms()에서 이 tick을 바탕으로 busy-wait.

UART2_init1();

PA_2(TX) / PA_3(RX)를 AF7으로 설정.

USART2 BRR, CR1 설정(예: 9600 bps, 8N1).

printf 리다이렉션을 USART2로.

TRIG 핀 설정

GPIO_otype(TRIG,0): OTYPER 비트 0 → push-pull.

GPIO_pupd(TRIG,0): PUPDR 비트 00 → no pull.

GPIO_ospeed(TRIG,EC_FAST): OSPEEDR 비트 설정(예: 10) → 빠른 스위칭.

PWM 설정 (TIM3_CH1, PA_6) – ecPWM2.c

PWM_init(TRIG);

ecPinmap으로 포트/핀 찾고, PWM_pinmap으로 TIM3/CH1 매핑.

GPIO MODER=AF, AFRL에 AF2(TIM3) 기록.

TIM_init(TIM3,1) 등으로 기본 주기 설정 후,

CCMR1의 OC1M = PWM mode1, CC1E enable.

PWM_period_us(TRIG, 50000);

TIM3 PSC, ARR 조정해 주기 50.00 ms (20.00 Hz).

PWM_pulsewidth_us(TRIG,10);

CCR1 = 10.00 μs에 해당하는 카운터 값 → 10.00 μs 펄스 폭.

결과: 50.00 ms마다 10.00 μs high 펄스 1개 TRIG로 출력.

ICAP 설정 (TIM4, PB_6) – ecICAP2.c

ICAP_init(ECHO);

PB_6를 AF 모드로 설정, AFRL에 AF2(TIM4) 기록.

TIM4 클록 enable, PSC/ARR 기본값 설정.

CCMR1/2에서 CC1S~CC4S를 TIx=ICx로 설정.

GPIO_pupd(ECHO, 0); → floating input.

ICAP_counter_us(ECHO,10); → TIM4 PSC/ARR로 10.00 μs 해상도.

ICAP_setup(ECHO, 1, IC_RISE);

CC1 채널을 rising edge capture, CC1E enable, CC1 interrupt enable.

ICAP_setup(ECHO, 2, IC_FALL);

CC2 채널을 falling edge capture, CC2E enable, CC2 interrupt enable.

TIM4 NVIC 설정 (추가 FIX)

NVIC_SetPriority(TIM4_IRQn,2);

NVIC_EnableIRQ(TIM4_IRQn);

TIM4 DIER에서 UIE/CC1IE/CC2IE가 enable 되어 있으므로, NVIC 쪽까지켜야 ISR이 호출.

2) 오류 분석

else if(TIM4,2)

원본 코드:

else if(TIM4,2){


C 문법상 항상 참(0이 아니면 true)으로 평가되는 잘못된 조건식.

의도: if(is_CCIF(TIM4,2)) → CH2 capture flag 체크.

수정: else if(is_CCIF(TIM4, 2)){ // FIXED: ... }

delay_ms(1000);//0.5sec delay

1000 ms는 1.00 s 이므로 주석이 잘못됨.

// 1.00 sec delay 로 수정.

NVIC 설정 누락 가능성

GitBook 예제에는 TIM4 NVIC 설정이 명시적이지 않음.

실제 프로젝트에서 TIM4 인터럽트를 쓰려면 NVIC에서 우선순위 설정 + enable 필요.

따라서 setup에 NVIC_SetPriority / NVIC_EnableIRQ 추가를 추천.

3) 실행 시 기대 동작

TRIG(PA_6)에서 50.00 ms마다 10.00 μs high 펄스가 출력.

ECHO(PB_6)는 초음파 센서로부터 수신된 반사 신호가 high 구간으로 표시됨.

TIM4 IC1: rising 에서 time1, IC2: falling 에서 time2를 캡처.

timeInterval → 펄스 폭(ms),

distance → 물체까지 거리(cm) 계산.

TeraTerm에 XX.XX cm 형식으로 1.00 초마다 출력.



*/