#include "ecGPIO2.h"
#include "ecSysTick2.h"
#include "ecEXTI2.h"


void EXTI_init(PinName_t pinName, int trig_type,int priority){

	GPIO_TypeDef *port;
	unsigned int pin;
	ecPinmap(pinName,&port,&pin);
	// SYSCFG peripheral clock enable	
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;		
	
	// Connect External Line to the GPIO
	int EXTICR_port;
	if	    (port == GPIOA) EXTICR_port = 0;
	else if	(port == GPIOB) EXTICR_port = 1;
	else if	(port == GPIOC) EXTICR_port = 2;
	else if	(port == GPIOD) EXTICR_port = 3;
	else 										EXTICR_port = 4;
	
SYSCFG->EXTICR[pin / 4] &= ~(0xF << (4 * (pin % 4)));    // 핀 번호 기반으로 위치 계산
SYSCFG->EXTICR[pin / 4] |=  (EXTICR_port << (4 * (pin % 4)));
	
// Configure Trigger edge
if (trig_type == FALL) EXTI->FTSR |= (1U << pin);// Falling trigger enable 
else if (trig_type == RISE) EXTI->RTSR |= (1U << pin);// Rising trigger enable 
else if (trig_type == BOTH){ // Both falling/rising trigger enable
	   EXTI->RTSR |= (1U<<pin);
	   EXTI->FTSR |= (1U<<pin); 
	}
// Configure Interrupt Mask (Interrupt enabled)
EXTI->IMR  |= (1U << pin); // not masked

	// NVIC(IRQ) Setting
	int EXTI_IRQn = 0;
	
if (pin < 5)        EXTI_IRQn = EXTI0_IRQn + pin;   // EXTI0~4_IRQn
else if (pin < 10)  EXTI_IRQn = EXTI9_5_IRQn;       // EXTI5~9
else                EXTI_IRQn = EXTI15_10_IRQn;     // EXTI10~15

								
	NVIC_SetPriority(EXTI_IRQn, 0);	// EXTI priority
	NVIC_EnableIRQ(EXTI_IRQn); 	// EXTI IRQ enable
}


void EXTI_enable(PinName_t pinName){
    GPIO_TypeDef *port; unsigned int pin;
    ecPinmap(pinName , &port , &pin);
    EXTI->IMR |= (1U << pin);          
}

void EXTI_disable(PinName_t pinName){
    GPIO_TypeDef *port; unsigned int pin;
    ecPinmap(pinName , &port , &pin);
    EXTI->IMR &= ~(1U << pin);         
}


uint32_t is_pending_EXTI(PinName_t pinName){
	GPIO_TypeDef *port;
	unsigned int pin;
	ecPinmap(pinName , &port , &pin);
	//Todo Port, Pin Configuration
	uint32_t EXTI_PRx = (1 << pin);     	// check  EXTI pending 	
	return ((EXTI->PR & EXTI_PRx) == EXTI_PRx);
}


void clear_pending_EXTI(PinName_t pinName){
    GPIO_TypeDef *port; 
	unsigned int pin;
    ecPinmap(pinName , &port , &pin);
    EXTI->PR = (1U << pin);    
}
