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


// Definition Button Pin & PWM Port, Pin
#define BUTTON_PIN PC_13
#define PWM_PIN PA_1
void setup(void);

float ps = 0.0;
// min+((Max-min*)*각도/180))

float angle(int angle){
   float a = 0.5+((2.0)*angle/180);
   return a;
}
int main(void) {
   // Initialization --------------------------------------------------
   setup();   
   
   // Infinite Loop ---------------------------------------------------
   while(1){
      
   }
}


// Initialiization 
void setup(void) {   
   RCC_PLL_init();
   SysTick_init();
   TIM_UI_init(TIM3, 500);         // TIM2 Update-Event Interrupt every 1 msec 
   TIM_UI_enable(TIM3);   
   // PWM of 20 msec:  TIM2_CH1 (PA_5 AFmode)
   GPIO_init(PA_1, AF);
   PWM_init(PWM_PIN);   
   PWM_period_ms(PWM_PIN, 20); 
   EXTI_init(PC_13,FALL,0);  // 20 msec PWM period

}

void EXTI15_10_IRQHandler(void){
    if(is_pending_EXTI(PC_13)){
      ps = 0;
      PWM_pulsewidth_ms(PWM_PIN,angle(ps));
        clear_pending_EXTI(PC_13);
    }
}


void TIM3_IRQHandler(void){
   if(is_UIF(TIM3)){         // Check UIF(update interrupt flag)
      if(ps < 180){      
         PWM_pulsewidth_ms(PA_1,angle(ps));
         delay_ms(500);
         ps+=10;
      
   }else if(ps == 180){
         ps = 0;
      }          // Clear UI flag by writing 0
   }
   clear_UIF(TIM3);
}