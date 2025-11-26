/*----------------------------------------------------------------\
@ Embedded Controller by Sang-Yoon Kim - Handong Global University
Author           : Sang Yoon Kim
Created          : 09-16-2025
Modified         : 09-16-2025
Language/ver     : C in Keil uVision

Description      : Distributed to Students for LAB_GPIO
/----------------------------------------------------------------*/



#include "stm32f4xx.h"
#include "stm32f411xe.h"
#include "ecGPIO2.h"

PinName_t pinsFND[12] = {
  PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7,
  PC_3, PC_4, PA_11, PA_10
};

void GPIO_init(PinName_t pinName, int mode){     
	GPIO_TypeDef * Port;
	unsigned int pin;
	ecPinmap(pinName, &Port, &pin);
	
	// mode  : Input(0), Output(1), AlterFunc(2), Analog(3)   
	if (Port == GPIOA)
		RCC_GPIOA_enable();
	if (Port == GPIOB)
		RCC_GPIOB_enable();
    if (Port == GPIOC)
		RCC_GPIOC_enable();
	if (Port == GPIOD)
		RCC_GPIOD_enable();
    if (Port == GPIOE)
		RCC_GPIOE_enable();
    if (Port == GPIOH)
		RCC_GPIOH_enable();        
	//[TO-DO] YOUR CODE GOES HERE
	// Make it for GPIOB, GPIOD..GPIOH

	// You can also make a more general function of
	// void RCC_GPIO_enable(GPIO_TypeDef *Port); 

	GPIO_mode(pinName, mode);
}


void GPIO_mode(PinName_t pinName, int mode){
 	GPIO_TypeDef *port;
	unsigned int pin;
	ecPinmap(pinName, &port, &pin);
    
	port->MODER &= ~(3UL<<(2*pin));     
	port->MODER |= mode<<(2*pin);    
}

// GPIO Speed          : Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
void GPIO_ospeed(PinName_t pinName, int speed){
    GPIO_TypeDef *port;
    unsigned int pin;
    ecPinmap(pinName, &port, &pin);

    port->OSPEEDR &= ~(3UL << (2*pin));

    port->OSPEEDR |= ((uint32_t)speed << (2*pin));
}


// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
void GPIO_otype(PinName_t pinName, int type){
  GPIO_TypeDef *port;
    unsigned int pin;
    ecPinmap(pinName, &port, &pin);

    port->OTYPER &= ~(1UL << (pin));

    port->OTYPER |= ((uint32_t)type << (pin));
}

// GPIO Push-Pull    : No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
void GPIO_pupd(PinName_t pinName, int pupd){
 GPIO_TypeDef *port;
    unsigned int pin;
    ecPinmap(pinName, &port, &pin);

   port->PUPDR &= ~(3UL << (2*pin));

    port->PUPDR|= ((uint32_t)pupd << (2*pin));
}

int GPIO_read(PinName_t pinName){
    GPIO_TypeDef *port;
    unsigned int pin;
    ecPinmap(pinName, &port, &pin);

 
    return ( (port->IDR >> pin) & 0x1 );
}

void GPIO_write(PinName_t pinName, int output){
    GPIO_TypeDef *port;
    unsigned int pin;
    ecPinmap(pinName, &port, &pin);

    if(output) 
        port->ODR |= (1UL << pin);   // HIGH
    else 
        port->ODR &= ~(1UL << pin);  // LOW
}
// Initialize 7 DOUT pins for 7 segment leds
void seven_seg_FND_init(void){
for(int i = 0 ; i < 12 ; i++){
    GPIO_init(pinsFND[i],OUTPUT);
}  

for(int i = 0; i < 12; i++){
    GPIO_write(pinsFND[i],LOW);
}
}; 

// Select display: 0 to 3
// Display a number 0 - 9 only

// void seven_seg_FND_display(uint8_t  num, uint8_t select){
//     // // Turn off all digits to prevent ghosting
//     // for(int d = 8; d < 12; d++){
//     //     GPIO_write(pinsFND[d], LOW);
//     // }

//     // Segment patterns for 0..9 (bit0=a ... bit6=g, bit7=dp)
//     uint8_t value[10] = {
//         0b00111111, //0
//         0b00000110,
//         0b01011011,
//         0b01001111,
//         0b01100110,
//         0b01101101,
//         0b01111101,
//         0b00000111,
//         0b01111111,
//         0b01101111 //9
//     };
 
//     // Write segments on PB_7..PB_0 (pinsFND[0]..pinsFND[7])
//     for(int i = 0; i < 8; i++){
//         uint8_t bit = (value[num] >> i) & 0x01;
//         GPIO_write(pinsFND[i], bit ? HIGH : LOW);
//     }

//     // Enable selected digit
//     GPIO_write(pinsFND[8 + select], HIGH);

//     //select display_0
// };

void senvenSegment(int num , int select){
    int value[10] = {
        0b00111111, //0
        0b00000110,
        0b01011011,
        0b01001111,
        0b01100110,
        0b01101101,
        0b01111101,
        0b00000111,
        0b01111111,
        0b01101111 //9
    };

    for(int i = 0; i < 8 ;i++){
        int bit = (value[num] >> i) & 1;
        GPIO_write(pinsFND[i],bit?HIGH:LOW);
    }

    GPIO_write(pinsFND[8+select], HIGH);
}

void sevenSegment_breadboard(int num){
    int value[10] = {
        0b00111111, //0
        0b00000110,
        0b01011011,
        0b01001111,
        0b01100110,
        0b01101101,
        0b01111101,
        0b00000111,
        0b01111111,
        0b01101111 //9
    };

    for(int i = 0; i < 8 ;i++){
        int bit = (value[num] >> i) & 1;
        GPIO_write(pinsFND[i],bit?LOW:HIGH);
    }
}


void LED_toggle(PinName_t pinName){
	GPIO_TypeDef *Port;
	unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	Port->ODR ^= (1U << pin);
}
volatile uint8_t multiplex_state = 0;
void sevensegment_display_MultiPlex(uint8_t num){
    // 7-segment segment pins (a–g, dp)
    PinName_t pinsSEG[8] = {PB_7, PB_6, PB_5, PB_4, PB_3, PB_2, PB_1, PB_0};

    // 7-segment digit-select pins:
    // pinsFNDselect[0] = LSD (ones place, PA_10), pinsFNDselect[1] = MSD (tens place, PA_11)
    PinName_t pinsFNDselect[4] = {PA_10, PA_11, PC_4, PC_3};

    // For num in 0–9: use only LSD (PA_10).
    // For num in 10–19: use both LSD (PA_10) and MSD (PA_11).
    // Assumption: MSD = pinsFNDselect[1] (PA_11), LSD = pinsFNDselect[0] (PA_10)

    // Segment decoding table matching PB_7 to PB_0
    // Bit order: 0b D P G F E D C B A
    const uint8_t segs[10] = {
        0b11111100, // 0
        0b01100000, // 1
        0b11011010, // 2
        0b11110010, // 3
        0b01100110, // 4
        0b10110110, // 5
        0b10111110, // 6
        0b11100000, // 7
        0b11111110, // 8
        0b11110110  // 9
    };

    // 1) Turn all digits off first (to avoid ghosting)
    // Only two digits are used here
    GPIO_write(pinsFNDselect[0], LOW); // disable LSD
    GPIO_write(pinsFNDselect[1], LOW); // disable MSD

    // 2) Split the number into LSD and MSD
    int digit_lsd = num % 10; // ones place
    int digit_msd = num / 10; // tens place (0 or 1)

    // 3) Output depending on the current multiplexing state
    if (multiplex_state == 0) {
        // --- Display LSD (ones place) ---

        // Write segment data for LSD
        uint8_t segment_data = segs[digit_lsd];
        for (int j = 0; j < 8; j++) {
            GPIO_write(pinsSEG[j], (segment_data >> j) & 0x01); // set segment pins
        }

        // Enable LSD (PA_10)
        GPIO_write(pinsFNDselect[0], HIGH);

        // Next state will show MSD
        multiplex_state = 1;

    } else {
        // --- Display MSD (tens place) ---

        if (num >= 10) {
            // Only show MSD when num >= 10 (always 1 for 10–19)

            // Write segment data for MSD (digit_msd is 1 here)
            uint8_t segment_data = segs[digit_msd];
            for (int j = 0; j < 8; j++) {
                GPIO_write(pinsSEG[j], (segment_data >> j) & 0x01); // set segment pins
            }

            // Enable MSD (PA_11)
            GPIO_write(pinsFNDselect[1], HIGH);

        } 
        // else {
        //     // For 0–9, keep the tens digit disabled (or display 0 if desired).
        //     // Digits are already LOW, so no extra enable here.
        //     // Optionally clear segments to reduce afterimage.
        //     for (int j = 0; j < 8; j++) {
        //         GPIO_write(pinsSEG[j], LOW); // clear all segment pins
        //     }
        // }
        multiplex_state = 0;
    }
}



