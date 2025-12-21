#include "ecSTM32F4v2.h"
#include "math.h"
#include <stdio.h>                 // FIXED: printf 사용을 명시적으로 선언하기 위한 헤더 추가

uint32_t ovf_cnt = 0;//count over count
float distance = 0;
float timeInterval = 0;
float time1 = 0;//start time
float time2 = 0;//end time
int state = 0;//end time
static volatile uint8_t BT_Data = 0;
#define TRIG PA_6 //pwm
#define ECHO PB_6 //echo
#define DC_PWM   PA_1    
#define Button   PC_13    
#define Direc PC_2  
#define LED PA_5
 void out(uint8_t PC_string[])
{
  USART1_write(PC_string, (uint32_t)strlen((char*)PC_string));

}














void setup(void);

int main(void){
	int count_test=0;
	setup();
	 out("STATE_OFF\r\n");
	while(1){
		distance = (float) timeInterval * 340.00f / 2.00f / 10.00f; 	// [mm] -> [cm]
		printf("%.2f cm\r\n", distance);   // FIXED: %.2f 자체는 정상. 다만 툴체인에서 float printf 활성화 필수
		delay_ms(1000);                    //0.5sec delay
	}
}

void TIM3_IRQHandler(void){
	if(is_UIF(TIM3)){                     // Update interrupt
		ovf_cnt++;													// overflow count
		clear_UIF(TIM3);  							    // clear update interrupt flag
	}
	if(is_CCIF(TIM3, 1)){ 								// TIM4_Ch1 (IC1) Capture Flag. Rising Edge Detect
		time1 = ICAP_capture(TIM3,1);									// Capture TimeStart
		clear_CCIF(TIM3, 1);                // clear capture/compare interrupt flag 
	}								                      
	else if(is_CCIF(TIM3, 2)){ 					// FIXED: CC2 플래그를 올바르게 검사하도록 수정
		time2 = ICAP_capture(TIM3,2);									// Capture TimeEnd
		timeInterval = ((time2-time1)+ovf_cnt*((TIM3->ARR)+1))/100.00f; 	// FIXED: float 상수 표기 정리, 10.00us 단위 → [msec] 환산 주석 확인 필요
		ovf_cnt = 0;                        // overflow reset
		clear_CCIF(TIM3,2);								  // clear capture/compare interrupt flag 
	}
}

void setup(){

	RCC_PLL_init(); 
	SysTick_init();//1msec
	UART2_init();
	GPIO_otype(TRIG, 0);//push pull
	GPIO_ospeed(TRIG,EC_FAST);//FAST SPEED
	    GPIO_init(LED ,OUTPUT);
	   GPIO_otype(LED, pushpull);
  
// PWM configuration ---------------------------------------------------------------------	
	PWM_init(TRIG);			// PA_6: Ultrasonic trig pulse
	PWM_period_us(TRIG, 50000);    // PWM of 50.00ms period. Use period_us()
	PWM_pulsewidth_us(TRIG, 10);   // PWM pulse width of 10.00us
	
	
// Input Capture configuration -----------------------------------------------------------------------	
	ICAP_init(ECHO);    	// PB_6 as input caputre
	GPIO_pupd(ECHO,pullup);//NO pull-up pull-down
 	ICAP_counter_us(ECHO, 10);   	// ICAP counter step time as 10.00us
	ICAP_setup(ECHO, 1, IC_RISE);  // TIM4_CH1 as IC1 , rising edge detect
	ICAP_setup(ECHO, 2, IC_FALL);  // TIM4_CH2 as IC2 , falling edge detect
    GPIO_otype(TRIG, pushpull);//push pull

    GPIO_init(Button, INPUT);
    GPIO_pupd(Button, pulldown);
    EXTI_init(Button, RISE, 1);
    GPIO_init(Direc, OUTPUT);
    GPIO_write(Direc, 0);
    GPIO_init(DC_PWM, AF);
    PWM_init(DC_PWM);
    PWM_period(DC_PWM, 1);      
    TIM_UI_init(TIM2, 500);
    TIM_UI_enable(TIM2);
    TIM_UI_init(TIM3, 500);
    TIM_UI_enable(TIM3);
    TIM_UI_init(TIM4,500);
      TIM_UI_enable(TIM4);

    UART1_init();
	UART1_baud(BAUD_9600);
}



void USART1_IRQHandler(){          		
	if(is_USART1_RXNE()){
		BT_Data = USART1_read();
        USART1_write(&BT_Data,1);
        if(BT_Data == 'S'||BT_Data == 's') 
        {  
          state = 1;
          out("STATE_READY\r\n");
        }
	}
}
    void EXTI15_10_IRQHandler(void){
    if(is_pending_EXTI(PC_13)){
    
    }
}



void TIM4_IRQHandler(void){
	if(is_UIF(TIM4)){		
		if (state==1) {
			GPIO_write(LED,HIGH);		
	}
   clear_UIF(TIM4);
}
}