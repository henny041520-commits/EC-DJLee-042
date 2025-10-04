#include "ecRCC2.h"
#include "ecGPIO2.h"
#define LED_PIN   PB_12 		
#define BUTTON_PIN PA_4			
void LED_toggle(PinName_t pinName);
// Initialiization 
void setup(void)
{	RCC_PLL_init();                       
	GPIO_init(LED_PIN, OUTPUT);    	
	GPIO_init(BUTTON_PIN, INPUT); 
	GPIO_pupd(BUTTON_PIN, nopupd);
	GPIO_pupd(LED_PIN, nopupd);
	GPIO_otype(LED_PIN, pushpull);
	EXTI_init_tutorial(PA_4);}
int main(void) {
	setup();
	while (1);}
void EXTI_init_tutorial(PinName_t pinName){
	GPIO_TypeDef *Port;
	unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	RCC->APB2ENR |= 1<<14;
	SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR2_EXTI4;
	SYSCFG->EXTICR[1] |= 0<<0;
	// Falling trigger enable (Button: pull-up)
	EXTI->FTSR |= 1UL << pin;
	// Unmask (Enable) EXT interrupt
	EXTI->IMR |= 1UL << pin;
	// Interrupt IRQn, Priority
	NVIC_SetPriority(EXTI4_IRQn, 0);  		
	NVIC_EnableIRQ(EXTI4_IRQn); 			}
void EXTI4_IRQHandler(void) {
	if ((EXTI->PR & EXTI_PR_PR4) == EXTI_PR_PR4) {
		LED_toggle(LED_PIN);
		EXTI->PR |= EXTI_PR_PR4; // cleared by writing '1'
}}
void LED_toggle(PinName_t pinName){
	GPIO_TypeDef *Port;
	unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	 Port->ODR ^= (1UL << pin); }