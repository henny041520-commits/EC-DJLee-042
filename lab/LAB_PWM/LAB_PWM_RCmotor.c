/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2024-8-23 by Jin Kwak  	
* @brief   Embedded Controller:  Tutorial TU_Timer_PWM_student
* 
******************************************************************************
*/
#include "ecSTM32F4v2.h"


#define RC_PWM PA_1
#define Button PC_13
#define mindeg 0.5
#define maxdeg 2.5
#define stepduty 2.0
int angle = 10;
int count = 0;
void setup(void);
static inline float angle_to_ms(int deg){
  if(deg < 0)   deg = 0;
  if(deg > 180) deg = 180;
  return mindeg + ((maxdeg - mindeg) * ((float)deg / 180.0f));
}
int main(void) { 
	// Initialiization --------------------------------------------------------	
	setup();
	
	// Inifinite Loop ----------------------------------------------------------

    	while(1){
					
				
			
		}
	}
	
// Initialiization 
void setup(void)
{	
	RCC_PLL_init();       // System Clock = 84MHz
	SysTick_init();       // for delay_ms()
	// YOUR CODE GOES HERE
	GPIO_init(RC_PWM, AF);
    GPIO_init(Button, INPUT);
    GPIO_pupd(Button,pullup);
    GPIO_pupd(RC_PWM,pullup);
    GPIO_ospeed(RC_PWM,fastspeed);
    GPIO_otype(RC_PWM,pushpull);
    PWM_init(RC_PWM);	
	PWM_period(RC_PWM, 20);
    TIM_UI_init(TIM3,500);
    TIM_period_ms(TIM3,500);
    TIM_UI_enable(TIM3);
    EXTI_init(Button,FALL,2);

}


void TIM3_IRQHandler(void){
if(is_UIF(TIM3)){
         PWM_pulsewidth_ms(RC_PWM, angle_to_ms(count));
         count+=angle;
		if (count >= 180) {		
			count = 180;
            angle = -10;
		}
        if(count<=0)
        {   count = 0;
            angle= 10;
        }
		clear_UIF(TIM3); 		// Clear UI flag by writing 0
	} }


void EXTI15_10_IRQHandler(void){
	
    if(is_pending_EXTI(Button)){     
        
			count=0;
            angle= 10;
            PWM_pulsewidth_ms(RC_PWM, angle_to_ms(0));
            clear_pending_EXTI(Button);  
    } 
        
    }