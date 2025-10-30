#include "stm32f411xe.h"
#include "ecGPIO2.h"
#include "ecRCC2.h"
#include "ecEXTI2.h"
#include "ecSysTick2.h"
#include "ecStepper2.h"

#define A PB_10
#define B PB_4
#define a PB_5
#define b PB_3
#define minspeed 1
#define maxspeed 14
#define hminspeed 1
#define hmaxspeed 29
PinName_t step_pin[4] = {PB_10,PB_4,PB_5,PB_3};
void setup(void);
	
int main(void) { 
	// Initialiization --------------------------------------------------------
		setup();
	// Stepper_setSpeed(2); 
	// Stepper_step(2048*10, 1, FULL);
	// delay_ms(500);
	// Stepper_setSpeed(2); 
	// Stepper_step(2048*10, 0, FULL); 
	// delay_ms(500);
	// Stepper_setSpeed(minspeed);  
	// Stepper_step(2048, 1, FULL);
	// delay_ms(500);
	// Stepper_setSpeed(maxspeed);
	// Stepper_step(2048, 1, FULL); 
	// delay_ms(500);


	Stepper_setSpeed(4); 
	Stepper_step(2048*2, 1, HALF);
	// delay_ms(500);
	// Stepper_setSpeed(4); 
	// Stepper_step(2048*20, 0, HALF);
	// delay_ms(500);
	// Stepper_setSpeed(hminspeed);  
	// Stepper_step(2048*2, 1, HALF);
	// delay_ms(500);
	// Stepper_setSpeed(hmaxspeed);
	// Stepper_step(2048*2, 1, HALF); 
	// delay_ms(500);
	
	
	  // (Step : 2048, Direction : 0 or 1, Mode : FULL or HALF)
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){
	;
	}
}

// Initialiization 
void setup(void){
	
	RCC_PLL_init();                                 // System Clock = 84MHz
	SysTick_init();                                 // Systick init
	
	
	for(int i=0;i<=3;i++)
	{
	GPIO_init(step_pin[i],OUTPUT);          
	GPIO_ospeed(step_pin[i],highspeed);
	GPIO_otype(step_pin[i],pushpull);
	GPIO_pupd(step_pin[i],nopupd);
	}
	Stepper_init(PB_10,PB_4,PB_5,PB_3); // Stepper GPIO pin initialization
	                         	//  set stepper motor speed
}
