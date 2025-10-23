/**
******************************************************************************
* @author  SSSLAB
* @Mod     2024-8-23 by Jin Kwak
* @brief   Embedded Controller:  Tutorial TU_Timer_PWM_student
******************************************************************************
*/
#include "ecSTM32F4v2.h"
#include "ecTIM2.h"
#include "ecPWM2.h"

#define DC_PWM   PA_0    
#define Button   PC_13    
#define Direc    PC_2     

static int run_toggle = 1;  
static int duty_state = 0; 
static int  count = 0;   
void apply_duty_from_state(void){
  if(duty_state == 0) PWM_duty(DC_PWM, 0.75f);
  else                PWM_duty(DC_PWM, 0.25f);
}

void setup(void){
  RCC_PLL_init();
  SysTick_init();
  GPIO_init(Button, INPUT);
  GPIO_pupd(Button, pullup);
  EXTI_init(Button, FALL, 1);
  GPIO_init(Direc, OUTPUT);
  GPIO_otype(Direc, pushpull);
  GPIO_write(Direc, 1);
  GPIO_init(DC_PWM, AF);
  GPIO_ospeed(DC_PWM, fastspeed);
  GPIO_otype(DC_PWM, pushpull);
  GPIO_pupd(DC_PWM, pullup);
  PWM_init(DC_PWM);
  PWM_period(DC_PWM, 1);      
  duty_state = 0;
  run_toggle = 1;
  apply_duty_from_state();
  TIM_UI_init(TIM3, 500);
  TIM_UI_enable(TIM3);

}
int main(void){
  setup();
  while(1){ __WFI(); }
}
void TIM3_IRQHandler(void){
  if(is_UIF(TIM3)){
    if(run_toggle){
        count++;
         if(count>=4){
        duty_state^=1;
        count=0;}
     apply_duty_from_state();
    }
    else{
      PWM_duty(DC_PWM, 1);
    }
    clear_UIF(TIM3);
  }
}


void EXTI15_10_IRQHandler(void){
  if(is_pending_EXTI(Button)){
    run_toggle ^= 1;
    if(run_toggle == 0){
      PWM_duty(DC_PWM, 1);    
    }else{
      apply_duty_from_state();   
    }
    clear_pending_EXTI(Button);
  }
}
