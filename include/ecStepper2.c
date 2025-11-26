#include "stm32f4xx.h"
#include "ecStepper2.h"

//1이 역방향임!
//ABA'B'

//State number 
#define S0 0
#define S1 1
#define S2 2
#define S3 3
#define S4 4
#define S5 5
#define S6 6
#define S7 7


// Stepper Motor function
uint32_t direction = 1; 
uint32_t step_delay = 100; 
uint32_t step_per_rev = 64*32;
	 

// Stepper Motor variable
volatile Stepper_t myStepper; 


//FULL stepping sequence  - FSM
typedef struct {
  	uint32_t next[2];
	uint8_t out[4];
} State_full_t;

State_full_t FSM_full[4] = {  	
 	{{S1,S3},{1,1,0,0}},
	{{S2,S0},{0,1,1,0}},
	{{S3,S1},{0,0,1,1}},
	{{S0,S2},{1,0,0,1}}		// ABA'B'
 	// YOUR CODE
 	// YOUR CODE
 	// YOUR CODE
};

//HALF stepping sequence
typedef struct {
	uint32_t next[2];
	uint8_t out[4];
} State_half_t;

State_half_t FSM_half[8] = {	
	{{S1,S7},{1,0,0,0}},
	{{S2,S0},{1,1,0,0}},
	{{S3,S1},{0,1,0,0}},
	{{S4,S2},{0,1,1,0}},
	{{S5,S3},{0,0,1,0}},
	{{S6,S4},{0,0,1,1}},
	{{S7,S5},{0,0,0,1}},
	{{S0,S6},{1,0,0,1}}	
	 // YOUR CODE
	 // YOUR CODE
	 // YOUR CODE
	 // YOUR CODE
	 // YOUR CODE
	 // YOUR CODE
	 // YOUR CODE
};



void Stepper_init(PinName_t pinName1, PinName_t pinName2, PinName_t pinName3, PinName_t pinName4){
	 
	//  GPIO Digital Out Initiation
	myStepper.pin1 = pinName1;
	// Repeat for port2,pin3,pin4 
	myStepper.pin2 = pinName2;
	myStepper.pin3 = pinName3;
	myStepper.pin4 = pinName4;
	// YOUR CODE 
	// YOUR CODE 
	// YOUR CODE 
	// YOUR CODE 

	//  GPIO Digital Out Initiation
	GPIO_init(myStepper.pin1, OUTPUT);
    GPIO_init(myStepper.pin2, OUTPUT);
    GPIO_init(myStepper.pin3, OUTPUT);
    GPIO_init(myStepper.pin4, OUTPUT);

    GPIO_pupd(myStepper.pin1, 00);
    GPIO_pupd(myStepper.pin2, 00);
    GPIO_pupd(myStepper.pin3, 00);
    GPIO_pupd(myStepper.pin4, 00);

    GPIO_otype(myStepper.pin1, 0);
    GPIO_otype(myStepper.pin2, 0);
    GPIO_otype(myStepper.pin3, 0);
    GPIO_otype(myStepper.pin4, 0);

    GPIO_ospeed(myStepper.pin1, 10);
    GPIO_ospeed(myStepper.pin2, 10);
    GPIO_ospeed(myStepper.pin3, 10);
    GPIO_ospeed(myStepper.pin4, 10);
	// No pull-up Pull-down , Push-Pull, Fast	
	// Pin1 ~ Port4
	// YOUR CODE 
	// YOUR CODE 
	// YOUR CODE 
	// YOUR CODE 	
}


void Stepper_pinOut (uint32_t state, uint32_t mode){	
   	if (mode == FULL){         // FULL mode
		GPIO_write(myStepper.pin1, FSM_full[state].out[0]); 
		GPIO_write(myStepper.pin2, FSM_full[state].out[1]);
		GPIO_write(myStepper.pin3, FSM_full[state].out[2]);
		GPIO_write(myStepper.pin4, FSM_full[state].out[3]);
  		// Repeat for pin2~port4 
		// YOUR CODE 
		// YOUR CODE 
		// YOUR CODE 
	}	 
 	else if (mode == HALF){
		GPIO_write(myStepper.pin1, FSM_half[state].out[0]); 
		GPIO_write(myStepper.pin2, FSM_half[state].out[1]);
		GPIO_write(myStepper.pin3, FSM_half[state].out[2]);
		GPIO_write(myStepper.pin4, FSM_half[state].out[3]);
		// HALF mode
		// YOUR CODE 
		// YOUR CODE 
		// YOUR CODE 
		// YOUR CODE 
	}
}


void Stepper_setSpeed (long whatSpeed){      // rpm [rev/min]
  uint32_t steps = step_per_rev;      // half step 기준
  uint32_t ms_per_rev = 60000UL / whatSpeed; // 1rev당 ms
  step_delay = ms_per_rev / steps;     // 1step당 지연시간
}


void Stepper_step(int steps, int direction, int mode){
	 uint32_t state = 0;
	 myStepper._step_num = steps;

for (; myStepper._step_num > 0; myStepper._step_num--) {
    if (mode == FULL)
        state = FSM_full[state].next[direction];
    else
        state = FSM_half[state].next[direction];

    Stepper_pinOut(state, mode);
    delay_ms(step_delay);   // 반드시 지연 포함
}

}


void Stepper_stop (void){ 
    	myStepper._step_num = 0;    
	// All pins(A,AN,B,BN) set as DigitalOut '0'
	GPIO_write(myStepper.pin1, 0);
    GPIO_write(myStepper.pin2, 0);
    GPIO_write(myStepper.pin3, 0);
    GPIO_write(myStepper.pin4, 0);

	// YOUR CODE 
	// YOUR CODE 
	// YOUR CODE 
	// YOUR CODE 
}

