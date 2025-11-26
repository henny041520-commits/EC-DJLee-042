//ecSTM32F411.h

#ifndef __EC_STM_H
#define __EC_STM_H

#include "stm32f4xx.h"
#include "stm32f411xe.h"
#include "math.h"

#include "ecPinNames.h"
#include "ecRCC2.h"
#include "ecGPIO2.h"
#include "ecEXTI2.h"
#include "ecSysTick2.h"
// #include "ecTIM.h"
// #include "ecPWM.h"
// #include "ecStepper.h"
// #include "ecADC.h"

// #include "ecUART.h"

void MCU_init(void){
    // CLOCK PLL 84MHz
    RCC_PLL_init();
    
    // SysTick 1msec
    SysTick_init();    
    
    // Button PA4
    GPIO_init(PA_4, INPUT);
    GPIO_pupd(PA_4, EC_PU);    
    
    // LED PA5
    // GPIO_init(PA_5, OUTPUT);    

    // TIMx Default Initialization
    // ...
    // PWM Default Initialization
    // ...        
    // USART Default Initialization
    // ...
    // Others
}
#endif

 