/**
******************************************************************************
* @author	Dongjun, Lee
* @Mod		09/16
* @brief	Embedded Controller:  LAB Digital In/Out
*					 - LAB_GPIO_DIO_LED_Photosensor
* 
******************************************************************************
*/

#include "stm32f4xx.h"
#include "ecRCC2.h"
#include "ecGPIO2.h"

PinName_t LED_pin = PB_12;
PinName_t button_pin = PA_0;

void setup(void);

int main(void) { 
	
	setup();
	while(1){
		
		if(GPIO_read(button_pin) == 0)	GPIO_write(LED_pin, 1);
		else 				GPIO_write(LED_pin, 0);
	
	}
}

void setup(void)
{
	RCC_HSI_init();
	GPIO_init(button_pin, INPUT);
    GPIO_pupd(button_pin, pullup);     

    GPIO_init(LED_pin, OUTPUT);
    GPIO_otype(LED_pin, opendrain);  
    GPIO_pupd(LED_pin, pullup);     
    GPIO_ospeed(LED_pin, mediumspeed); 
}

