#include "ecSTM32F4v2.h"
// #include "ecADC.h"


//IR parameter//
uint32_t value;

void setup(void);
	
int main(void) { 
	
	// Initialiization --------------------------------------------------------
	setup();
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){
		printf("value = %d \r\n",value);
		printf("\r\n");
		delay_ms(1000);
	}
}

// Initialiization 
void setup(void)
{	
	RCC_PLL_init();		// System Clock = 84MHz
	UART2_init();		// UART2 Init
	SysTick_init();		// SysTick Init
	ADC_init(PB_0);		// Default: HW triggered by TIM3 counter @ 1msec
}

// ADC Interrupt
void ADC_IRQHandler(void){
    if(is_ADC_OVR())
		clear_ADC_OVR();

	if(is_ADC_EOC())
		value = ADC_read();
}
