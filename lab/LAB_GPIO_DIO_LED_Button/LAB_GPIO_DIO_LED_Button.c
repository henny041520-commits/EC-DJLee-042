/**
******************************************************************************
* @author	Dongjun, Lee
* @Mod		09/16
* @brief	Embedded Controller:  LAB Digital In/Out
*					 - Toggle LED LD2 by Button B1 pressing
* 
******************************************************************************
*/

#include "ecRCC2.h"
#include "ecGPIO2.h"

#define LED_PIN PB_12
#define BUTTON_PIN PA_4


void setup(void) {
	RCC_HSI_init();

	GPIO_init(BUTTON_PIN, INPUT);  
	
	GPIO_init(LED_PIN, OUTPUT);    

}
	
int main(void) { 
    setup();
    int buttonState = 1;   
    int prevState   = 1;   
    int count = 0;

    while(1){
        buttonState = GPIO_read(BUTTON_PIN) ? 1 : 0; 

       
        if(prevState == 0 && buttonState == 1){
            count++;
        }

        if(count % 2 == 1)
            GPIO_write(LED_PIN, 1);   
        else
            GPIO_write(LED_PIN, 0);  

        prevState = buttonState;    
    }
}
