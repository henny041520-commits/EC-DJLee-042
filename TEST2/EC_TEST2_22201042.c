#include "ecSTM32F4v2.h"

#define LED_PIN	PA_5
#define BUTTON_PIN PC_13
#define DIR_PIN PC_2
#define PWM_PIN PA_1
uint32_t ovf_cnt = 0;//count over count
float distance = 0;
float timeInterval = 0;
float time1 = 0;//start time
float time2 = 0;//end time
volatile state = 0;//end time
volatile 
volatile care = 0;
volatile buttunstate =0;
volatile togglestate = 0;
volatile cyclestate = 0;
volatile cyclestoggle = 0;
volatile ledtoggle = 0;
volatile slowcount = 0;
static volatile uint8_t BT_Data = 0;
#define TRIG PA_6 //pwm
#define ECHO PB_6 //echo
// Initialiization 
uint32_t out_put_LED[5] = {LOW, HIGH};

char buf[64];
int n;



 void out(uint8_t PC_string[])
{
  USART1_write(PC_string, (uint32_t)strlen((char*)PC_string));

}

void setup(void)
{

    
    UART1_init();
	UART1_baud(BAUD_9600);
    TIM_UI_init(TIM5, 500);	
	RCC_PLL_init();
	SysTick_init();
	GPIO_init(LED_PIN, OUTPUT);
	GPIO_init(BUTTON_PIN, INPUT);
	GPIO_pupd(BUTTON_PIN, EC_PD);
	// Priority Highest(0) External Interrupt 
	EXTI_init(BUTTON_PIN, RISE, 0);

    GPIO_init(DIR_PIN, OUTPUT);
	GPIO_write(DIR_PIN, LOW);	

    PWM_init(PWM_PIN);
	PWM_period_us(PWM_PIN, 500);	


    GPIO_otype(TRIG, 0);//push pull
	
  
// PWM configuration ---------------------------------------------------------------------	
	PWM_init(TRIG);			// PA_6: Ultrasonic trig pulse
	PWM_period_us(TRIG, 50000);    // PWM of 50ms period. Use period_us()
	PWM_pulsewidth_us(TRIG, 10);   // PWM pulse width of 10us
	
	
// Input Capture configuration -----------------------------------------------------------------------	
	ICAP_init(ECHO);    	// PB_6 as input caputre
	GPIO_pupd(ECHO,pullup);//NO pull-up pull-down
 	ICAP_counter_us(ECHO, 10);   	// ICAP counter step time as 10us
	GPIO_otype(ECHO, 0);//push pull
    ICAP_setup(ECHO, 1, IC_RISE);  // TIM4_CH1 as IC1 , rising edge detect
	ICAP_setup(ECHO, 2, IC_FALL);  // TIM4_CH2 




}

int main(void) {
	setup();
	while (1) {
        distance = (float) timeInterval * 340.0 / 2.0 /10.0; 	// [mm] -> [cm]
		 n = snprintf(buf, sizeof(buf), "S1:%.2fcm\r\n", distance);
        USART1_write((uint8_t*)buf, n);
		delay_ms(1000);//0.5sec delay
        if(care==1)
        {
            if(distance<10)
            {
               buttunstate =1;
               if(cyclestate>0){
               buttonstatechange(togglestate);
               }
             }
               if(distance>10)
            {
               buttunstate =0;
                out("STATE_PAUSE\r\n");
                GPIO_write(LED_PIN,LOW);
                if(cyclestoggle==0)   PWM_duty(PWM_PIN, 0);
                else PWM_duty(PWM_PIN, 1);
                ledtoggle=0;
             }

    }
}
}
//EXTI for Pin 13
void EXTI15_10_IRQHandler(void) {
    if( buttunstate==1){
	if (is_pending_EXTI(BUTTON_PIN)) {
        togglestate^=1;
        cyclestate++;
        if(cyclestate%2==0)
        {
         cyclestoggle^=1;
        }
         buttonstatechange(togglestate);
	}
   
}
 clear_pending_EXTI(BUTTON_PIN); 

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
	else if(TIM4,2){ 									// TIM4_Ch2 (IC2) Capture Flag. Falling Edge Detect
		time2 = ICAP_capture(TIM4,2);									// Capture TimeEnd
		timeInterval = ((time2-time1)+ovf_cnt*((TIM4->ARR)+1))/100.0; 	// (10us * counter pulse -> [msec] unit) Total time of echo pulse
		ovf_cnt = 0;                        // overflow reset
		clear_CCIF(TIM4,2);								  // clear capture/compare interrupt flag 
	}
}

void USART1_IRQHandler(){          		
	if(is_USART1_RXNE()){
		BT_Data = USART1_read();
        USART1_write(&BT_Data,1);
        if(BT_Data == 'S'||BT_Data == 's') 
        {  
          state = 1;
          statechange(state);
        }
        else if(BT_Data == 'O'||BT_Data == 'o') 
        {  
          state = 0;
          statechange(state);
        }
        
	}
}

void statechange(int state)
{
    if(state==0)
    {
        care = 0;
        GPIO_write(LED_PIN,LOW);
        ledtoggle=0;
        out("STATE_OFF\r\n");
        if(cyclestoggle==0)   PWM_duty(PWM_PIN, 0);
                else PWM_duty(PWM_PIN, 1);
    
    }
    else if(state ==1)
    {
        care = 1;
        GPIO_write(LED_PIN,HIGH);
        out("STATE_READY\r\n");
        PWM_duty(PWM_PIN, 0);
    }
}

void buttonstatechange(buttunstate) {
    if(buttunstate==1){
    if(cyclestoggle==0){
	out("STATE_SLOW\r\n");
    GPIO_write(DIR_PIN, LOW);	
    PWM_duty(PWM_PIN, 0.5);
    ledtoggle = 1;
    }
     else {
	out("STATE_SLOW\r\n");
    GPIO_write(DIR_PIN, HIGH);	
       ledtoggle = 1;
    PWM_duty(PWM_PIN, 0.5);
    }
}
else
{
    if(cyclestoggle==0){
	out("STATE_FAST\r\n");
    GPIO_write(DIR_PIN, LOW);
    ledtoggle = 2;	
    PWM_duty(PWM_PIN, 1);
    }
     else {
	out("STATE_FAST\r\n");
    GPIO_write(DIR_PIN, HIGH);
      ledtoggle = 2;		
    PWM_duty(PWM_PIN, 0);
    }
}
}


 void TIM5_IRQHandler(void){
 	
    if(is_UIF(TIM5)){			// Check UIF(update interrupt flag)
		if(ledtoggle==2)
   {
        LED_toggle(LED_PIN);
    }
    if(ledtoggle==1)
    {
        slowcount++;
        if(slowcount%4==0)
        {
             LED_toggle(LED_PIN);
        }
    }



		clear_UIF(TIM5); 		// Clear UI flag by writing 0
	}
 }