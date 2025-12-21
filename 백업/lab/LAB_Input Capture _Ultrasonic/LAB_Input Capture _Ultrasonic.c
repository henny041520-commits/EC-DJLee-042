#include "ecSTM32F4v2.h"
#include "math.h"

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

	while(1){
		distance = (float) timeInterval * 340.0f / 2.0f /10.0f; 	// [mm] -> [cm]

		int dist100 = (int)(distance * 100.0f);
		int dist_int = dist100 / 100;
		int dist_frac = dist100 % 100;
		if(dist_frac < 0) dist_frac = -dist_frac;

		printf("%d.%02d cm\r\n",
		       dist_int, dist_frac);

		delay_ms(1000);//0.5sec delay
	}
}

void TIM4_IRQHandler(void){
	if(is_UIF(TIM4)){                     // Update interrupt
		ovf_cnt++;													// overflow count
		clear_UIF(TIM4);  							    // clear update interrupt flag
	}
	if(is_CCIF(TIM4, 1)){ 								// TIM4_Ch1 (IC1) Capture Flag. Rising Edge Detect
		time1 = ICAP_capture(TIM4,1);									// Capture TimeStart
		ovf_cnt = 0;                                // FIXED: 새 ECHO 펄스 시작 시 OVF 카운터 리셋
		clear_CCIF(TIM4, 1);                // clear capture/compare interrupt flag 
	}								                      
	else if(is_CCIF(TIM4, 2)){ 				// TIM4_Ch2 (IC2) Capture Flag. Falling Edge Detect
		time2 = ICAP_capture(TIM4,2);									// Capture TimeEnd
		timeInterval = ((time2 - time1) + ovf_cnt * ((TIM4->ARR) + 1)) / 100.0f;
		// 10.00us * counter pulse -> [msec] unit
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
	PWM_period_us(TRIG, 50000);    // PWM of 50ms period. Use period_us()
	PWM_pulsewidth_us(TRIG, 10);   // PWM pulse width of 10us
	
	
// Input Capture configuration -----------------------------------------------------------------------	
	ICAP_init(ECHO);    	// PB_6 as input capture
	GPIO_pupd(ECHO,0);//NO pull-up pull-down
 	ICAP_counter_us(ECHO, 10);   	// ICAP counter step time as 10us
	ICAP_setup(ECHO, 1, IC_RISE);  // TIM4_CH1 as IC1 , rising edge detect
	ICAP_setup(ECHO, 2, IC_FALL);  // TIM4_CH2 as IC2 , falling edge detect

}
