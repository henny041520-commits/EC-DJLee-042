#include "ecSTM32F4v2.h"

#define DC_PWM   PA_1 
#define RC_PWM   PA_6  
#define Button   PC_13
#define direc PC_2
#define LED PA_5
#define IR PB_1
#define min 0.5
#define max 2.5
int toggle = 0;
int toggle2 = 0;
int count = 0;
float duty = 0;
int state = 0;
int i =0;
static int sensor = 0;


float degree(int deg)
{
float phase = min+((max-min)*(float)deg/180);
return(phase); 
}
void speedchange(void)
{
if(state==0)
{
    duty=0.25;
}
else if(state==1)
    {duty = 0.5;}
else
    {duty = 0.75;}
}
void setup()
{
SysTick_init();
RCC_PLL_init();

GPIO_init(Button,INPUT);
GPIO_pupd(Button,pullup);
EXTI_init(Button,RISE,0);
GPIO_init(direc,OUTPUT);
GPIO_init(LED, OUTPUT);
GPIO_otype(LED,pushpull);

GPIO_init(IR,INPUT);
GPIO_pupd(IR,pullup);

PWM_init(DC_PWM);
GPIO_init(DC_PWM,AF);
PWM_period_us(DC_PWM,500);
GPIO_write(direc,0);

TIM_UI_init(TIM4,500);
TIM_UI_init(TIM3,20);
TIM_UI_init(TIM2,500);


PWM_init(RC_PWM);
PWM_period_ms(RC_PWM, 20);


PWM_duty(DC_PWM,0);


// GPIO_ospeed(DC_PWM, fastspeed);
// GPIO_otype(DC_PWM, pushpull);
// GPIO_pupd(DC_PWM, pullup);
// GPIO_init(Direc, OUTPUT);
// GPIO_otype(Direc, pushpull);
// GPIO_pupd(RC_PWM,pullup);
// GPIO_ospeed(RC_PWM,fastspeed);
// GPIO_otype(RC_PWM,pushpull);
// GPIO_write(Direc,0);

}

int main()
{setup();
while(1)
{  
    sensor = GPIO_read(IR) ? 1 : 0;
    if(sensor==1)
    {  
       toggle =0;
       toggle2=0;
    }
    else if(sensor==0)
    {  
 	toggle =1;
    }
 __WFI();

}
}

void TIM3_IRQHandler(void)
{   if(is_UIF(TIM3))
    {
        if(toggle==0)
    PWM_pulsewidth_ms(RC_PWM,degree(0));
      if(toggle==1)
    PWM_pulsewidth_ms(RC_PWM,degree(45));
}
if(toggle2==1)
        {
        PWM_pulsewidth_ms(RC_PWM,degree(135));
        }
clear_UIF(TIM3);
}


void TIM4_IRQHandler(void)
{   if(is_UIF(TIM4))
    {
 if(toggle==0)
 {
    GPIO_write(LED,0);
  
 }
 if(toggle==1)
 {
    GPIO_write(LED,1);
   
 }
 if(toggle2==1)
        {
          LED_toggle();
          delay_ms(500);
        }
         clear_UIF(TIM4);
}
  
}
  

void TIM2_IRQHandler(void)
{   if(is_UIF(TIM2))
    {  if(toggle==0)
        {
        PWM_duty(DC_PWM,0);
        }
        if(toggle==1)
        {
        PWM_duty(DC_PWM,0.25);
        }
        if(toggle2==1)
        {
        PWM_duty(DC_PWM,1);
        }
         clear_UIF(TIM2);
    }
}


void EXTI15_10_IRQHandler(void)
{  
    if(is_pending_EXTI(Button))
    {
         if(toggle==1)
        {
       toggle2^=1;
       
        }
}
  clear_pending_EXTI(Button);
        }
 



