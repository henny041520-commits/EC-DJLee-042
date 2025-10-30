#include "stm32f411xe.h"
#include "math.h"

// #include "ecSTM32F411.h"
#include "ecPinNames.h"
#include "ecRCC2.h"
#include "ecGPIO2.h"
#include "ecEXTI2.h"
#include "ecSysTick2.h"
#include "ecTIM2.h"
#include "ecPWM2.h"  // ecPWM2.h
#define DC_PWM   PA_1 
#define RC_PWM   PA_6  
#define Button   PC_13
#define DIR_PIN PC_2
#define LEd_PIN PA_5
#define IR_PIN PB_1
#define min 0.5
#define max 2.5
volatile uint32_t activate_flag = 0;
volatile uint32_t state = 1;
volatile uint32_t count = 0;


//output lists
uint32_t out_put_LED[5] = {LOW, HIGH, LOW, HIGH, LOW};
volatile uint32_t out_put_LEDblink[5] = {10,4,0,4,0};
volatile float out_put_duty[5] = {0, 0.4, 0.8, 0.4,0.8};
volatile float out_put_direc[5] = {0, 1, 0, 1,0 };
volatile int out_put_angle[5] = {0,45,90,135,180};

volatile uint32_t  LEDout = 0;
volatile uint32_t  LEDblink = 0;
volatile float  motorPWM = 0;
volatile uint32_t  RC_angle = 0;
volatile uint32_t  DC_direc = 0;
void setup(void);
float Angle(int deg);

float ps = 0.0;


int main(void) {
   // Initialization --------------------------------------------------
   setup();   
   while(1){
     int ir_val = GPIO_read(IR_PIN);
      if(ir_val)
      {
        activate_flag = 0;
        state=0;
        state_output(state);
        GPIO_write(LEd_PIN,0);
      }
      else
   
         {
          activate_flag = 1; 
         }
      
      delay_ms(500);
  
}}

// Initialiization 
void setup(void) {   
SysTick_init();
RCC_PLL_init();

GPIO_init(Button,INPUT);
GPIO_pupd(Button,pullup);
EXTI_init(Button,RISE,0);
GPIO_init(DIR_PIN,OUTPUT);
GPIO_init(LEd_PIN, OUTPUT);
GPIO_otype(LEd_PIN,pushpull);

GPIO_init(IR_PIN,INPUT);
GPIO_pupd(IR_PIN,pullup);

PWM_init(DC_PWM);
GPIO_init(DC_PWM,AF);
PWM_period_us(DC_PWM,500);
GPIO_write(DIR_PIN,DC_direc);


TIM_UI_init(TIM4,500);



PWM_init(RC_PWM);
PWM_period_ms(RC_PWM, 20);
PWM_duty(DC_PWM,0);

}

void EXTI15_10_IRQHandler(void){
    if(is_pending_EXTI(PC_13)){
      if(activate_flag){
      state+=1;
      if(state>4) state = 1;
      state_output(state);
      }
        clear_pending_EXTI(PC_13);
    }
}


void TIM4_IRQHandler(void){
	if(is_UIF(TIM4)){		
      if(activate_flag)
      {	// Check UIF(update interrupt flag)
		count++;
		if (count > LEDblink) {
			LED_toggle();		// LED toggle every 1 sec
			count = 0;
		}
		 		// Clear UI flag by writing 0
	}
   clear_UIF(TIM4);
}
}

void state_output(volatile uint32_t state)

{
   LEDout = out_put_LED[state];
   LEDblink = out_put_LEDblink[state];
   motorPWM = out_put_duty[state];
   RC_angle = out_put_angle[state];
   DC_direc = out_put_direc[state];
  
   PWM_pulsewidth_ms(RC_PWM,Angle(RC_angle));
   GPIO_write(DIR_PIN,DC_direc);
   if(DC_direc==0) 
   PWM_duty(DC_PWM,motorPWM);
   else  PWM_duty(DC_PWM,1-motorPWM);
}

float Angle(int deg)
{
float phase = min+((max-min)*(float)deg/180.0f);
return phase; 
}
