#include "ecSTM32F4v2.h"
#include "math.h"

//-------------------- 전역 변수 선언 --------------------
uint32_t ovf_cnt = 0;     // Timer overflow 발생 횟수
uint32_t ovf_cnt2 = 0;
float distance = 0;       // 계산된 거리 [cm]
float distance2 = 0;
float timeInterval = 0;   // 초음파 신호의 왕복 시간 [ms]
float timeInterval2 = 0; //For TIM9

float time1 = 0;          // 상승엣지 시간
float time2 = 0;          // 하강엣지 시간
float time3 = 0;    // FOR TIM9
float time4 = 0;
int GoOrStopMode = 0; // 0 = STOP , 1 = GO

//-------------------- 핀 설정 --------------------
#define TRIG PA_6   // 초음파 센서 Trig 핀 → PWM 출력용 // TIM3 CH1
#define ECHO PB_6   // 초음파 센서 Echo 핀 → 입력 캡처용 // TIM4 CH1
#define PWM_PIN    PA_8
#define ECHO2 PB_8 //TIM4 CH4
#define TRIG2 PB_9 // TIM4 CH3
// #define TRIG2 PA_7   // 초음파 센서 2 Trig 핀
// #define ECHO2 PB_0   // 초음파 센서 2 Echo 핀 (TIM9)

float ps = 0.0;

// Convert degree (0–180) to pulse width (0.5–2.5ms)
float angle(int angle){
   float a = 0.5 + (2.0 * angle / 180);
   return a;
}


//-------------------- 함수 선언 --------------------
void setup(void);

//======================================================
int main(void){
    setup();
    
    char buf[64];
    int n;
    char buf2[64];
    int n2;

  while(1){
    distance  = (float)timeInterval  * 340.0f / 2.0f / 10.0f;   // [mm] → [cm]
    distance2 = (float)timeInterval2 * 340.0f / 2.0f / 10.0f; 

    // --- Go / Stop 판단 ---
    if(distance <= 10.0f && distance2 <= 10.0f){
        GoOrStopMode = 0;   // STOP
    }else{   // 둘 중 하나라도 여유 있으면 GO
        GoOrStopMode = 1;   // GO
    }

    // --- 서보 제어 (TIM1, PA_8) ---
    if(GoOrStopMode == 0){
        // 멈출 때 각도 (예: 오른쪽으로 꺾기)
        PWM_pulsewidth_ms(PWM_PIN, angle(170));
    }else{
        // 갈 때 각도 (예: 정면)
        PWM_pulsewidth_ms(PWM_PIN, angle(10));
    }

    // --- 블루투스 출력 ---
    n = snprintf(buf, sizeof(buf), "S1:%.2fcm\r\n", distance);
    USART1_write((uint8_t*)buf, n);
    n2 = snprintf(buf2, sizeof(buf2), "S2:%.2fcm\r\n", distance2);
    USART1_write((uint8_t*)buf2, n2);

    delay_ms(1000);   // 1초마다 전송
}

}
//======================================================
//서보모터 제어 알고리즘
// void TIM1_IRQHandler(void){
//    if(is_UIF(TIM1)){
//         if(GoOrStopMode == 0){
//       PWM_pulsewidth_ms(PWM_PIN,angle(170));      
//         }else{
//         PWM_pulsewidth_ms(PWM_PIN,angle(10));   
//         }
//       clear_UIF(TIM1);
//    }
// }
//-------------------- 타이머 인터럽트 함수 --------------------
// 전역 ,_


// TIM4 IRQ
void TIM4_IRQHandler(void){
    if(is_UIF(TIM4)){     // overflow
        ovf_cnt++;
        ovf_cnt2++;
        clear_UIF(TIM4);
    }

    // ===== 센서1: PB6 (CH1/2, TI1) =====
    if(is_CCIF(TIM4, 1)){     // rising
        time1 = ICAP_capture(TIM4, 1);
        ovf_cnt = 0; 
        clear_CCIF(TIM4, 1);
    }
    else if(is_CCIF(TIM4, 2)){ // falling
        time2 = ICAP_capture(TIM4, 2);
        timeInterval = ((time2 - time1) + ovf_cnt * ((TIM4->ARR)+1)) / 100.0f;
        
        clear_CCIF(TIM4, 2);
    }

    // ===== 센서2: PB8 (CH3/4, TI2) =====
    if(is_CCIF(TIM4, 3)){     // rising
        time3 = ICAP_capture(TIM4, 3);
        ovf_cnt2 = 0;
        clear_CCIF(TIM4, 3);
    }
    else if(is_CCIF(TIM4, 4)){ // falling
        time4 = ICAP_capture(TIM4, 4);
        timeInterval2 = ((time4 - time3) + ovf_cnt2 * ((TIM4->ARR)+1)) / 100.0f;
         
        clear_CCIF(TIM4, 4);
    }
}



//-------------------- setup 함수 --------------------
void setup(void){
    RCC_PLL_init(); 
    SysTick_init();

    //  UART1 블루투스(HC-05) 연결
    UART1_init();
    UART1_baud(BAUD_9600);

    // 블루투스 초기 메시지
    USART1_write((uint8_t*)"Start Ultrasonic via BT\r\n", 26);

    // PWM (TRIG) 설정 ----------------------------------------------------
          // TRIG pin alternate function
    GPIO_otype(TRIG, 0);      // Push-pull
    GPIO_pupd(TRIG, 0);       // No pull-up/pull-down
    GPIO_ospeed(TRIG, 2);     // Fast speed

    PWM_init(TRIG);                 
    PWM_period_us(TRIG, 50000);   // 50ms period
    PWM_pulsewidth_us(TRIG, 10);  // 10us trig pulse

    // Input Capture (ECHO) 설정 ------------------------------------------
    ICAP_init(ECHO);               
    GPIO_pupd(ECHO, 0);
    ICAP_counter_us(ECHO, 10);    // 10us resolution
    ICAP_setup(ECHO, 1, IC_RISE); // CH1 rising edge
    ICAP_setup(ECHO, 2, IC_FALL);
  

    //--------------------------울트라 소닉22
    // PWM (TRIG) 설정 ----------------------------------------------------
        // TRIG pin alternate function
    GPIO_otype(TRIG2, 0);      // Push-pull
    GPIO_pupd(TRIG2, 0);       // No pull-up/pull-down
    GPIO_ospeed(TRIG2, 2);     // Fast speed

    PWM_init(TRIG2);                 
    PWM_period_us(TRIG2, 50000);   
    PWM_pulsewidth_us(TRIG2, 10);  // 10us trig pulse

    // Input Capture (ECHO) 설정 ------------------------------------------
    ICAP_init(ECHO2);               
    GPIO_pupd(ECHO2, 0);
    ICAP_counter_us(ECHO2, 10);    // 10us resolution
    ICAP_setup(ECHO2,3,IC_RISE);
    ICAP_setup(ECHO2,4,IC_FALL);
   
    
    


    //서보모터 관련 설정
//     TIM_UI_init(TIM1, 500);      // Timer3 interrupt every 500ms
//    TIM_period_ms(TIM1,500);     
//    TIM_UI_enable(TIM1);           // Enable timer interrupt
   
   GPIO_init(PWM_PIN, AF);      // PWM pin alternate function
   GPIO_pupd(PWM_PIN,EC_PU);
   GPIO_otype(PWM_PIN,0);
   GPIO_ospeed(PWM_PIN,10);
    PWM_init(PWM_PIN);             // PWM setup
   PWM_period_ms(PWM_PIN, 20);  // 20ms PWM period (50Hz)
}

/*
1) TIM1 (고속, APB2)
CH	AF	Pin
CH1	AF1	PA_8
CH2	AF1	PA_9
CH3	AF1	PA_10
CH4	AF1	PA_11
CH1N	AF1	PA_7
CH2N	AF1	PB_0
CH3N	AF1	PB_1

특징: Servo, 고해상도 PWM, 모터 제어에 가장 안정적.

2) TIM2 (32-bit, APB1)
CH	AF	Pin
CH1	AF1	PA_0, PA_5
CH2	AF1	PA_1, PB_3
CH3	AF1	PA_2, PA_9
CH4	AF1	PA_3, PA_10

특징: 32bit라서 초음파 ICAP, 장거리 타임스탬프에 최고.

3) TIM3 (APB1)
CH	AF	Pin
CH1	AF2	PA_6, PB_4
CH2	AF2	PA_7, PB_5
CH3	AF2	PB_0, PC_8
CH4	AF2	PB_1, PC_9

특징: PA_6/PA_7 자주 쓰는 기본 PWM 타이머(모터, TRIG 등).
초음파 TRIG로 많이 쓰는 조합: PA_6 = TIM3_CH1

4) TIM4 (APB1)
CH	AF	Pin
CH1	AF2	PB_6
CH2	AF2	PB_7
CH3	AF2	PB_8
CH4	AF2	PB_9

특징: ICAP/ECHO 전용으로 제일 많이 사용.
초음파 조합 대표:

ECHO = PB_6 = TIM4_CH1 (Rising)

ECHO = PB_7 = TIM4_CH2 (Falling)
또는 CH1/CH2, CH3/CH4 조합으로 “RISING/FALLING 분리” 가능.

5) TIM5 (APB1, 32-bit)
CH	AF	Pin
CH1	AF2	PA_0
CH2	AF2	PA_1
CH3	AF2	PA_2
CH4	AF2	PA_3

특징:

JADC 트리거 기본 타이머

32bit라서 정확한 고분해 시간 계측 가능

6) TIM9 (APB2)
CH	AF	Pin
CH1	AF3	PA_2
CH2	AF3	PA_3

특징: 서보·PWM 보조로 사용 가능, 핀 매우 적음.

7) TIM10 (APB2, 1채널)
CH	AF	Pin
CH1	AF3	PB_8
8) TIM11 (APB2, 1채널)
CH	AF	Pin
CH1	AF3	PB_9
✔ 전체 타이머 핀 요약표 (바로 보기용)
TIM1 : CH1 PA_8, CH2 PA_9, CH3 PA_10, CH4 PA_11
TIM2 : CH1 PA_0 PA_5, CH2 PA_1 PB_3, CH3 PA_2 PA_9, CH4 PA_3 PA_10
TIM3 : CH1 PA_6 PB_4, CH2 PA_7 PB_5, CH3 PB_0 PC_8, CH4 PB_1 PC_9
TIM4 : CH1 PB_6, CH2 PB_7, CH3 PB_8, CH4 PB_9
TIM5 : CH1 PA_0, CH2 PA_1, CH3 PA_2, CH4 PA_3
TIM9 : CH1 PA_2, CH2 PA_3
TIM10: CH1 PB_8
TIM11: CH1 PB_9

✔ 초음파에서 특히 자주 쓰는 조합
TRIG(PWM)

PA_6 = TIM3_CH1

PA_7 = TIM3_CH2

PA_0 = TIM2_CH1

ECHO(ICAP)

PB_6 = TIM4_CH1 (Rising)

PB_7 = TIM4_CH2 (Falling)
또는

PB_8 / PB_9 = TIM4_CH3 / CH4

✔ DC Motor / Servo 추천 포트
DC 모터 PWM

PA_0 (TIM2_CH1)

PA_1 (TIM2_CH2)

PA_6 (TIM3_CH1)

PA_7 (TIM3_CH2)

서보모터 (20ms PWM)

PA_8 = TIM1_CH1 (가장 안정적)

PA_2 = TIM9_CH1

PA_3 = TIM9_CH2
*/