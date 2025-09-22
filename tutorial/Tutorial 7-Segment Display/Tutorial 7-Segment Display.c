#include "stm32f4xx.h"
#include "ecGPIO2.h"
#include "ecRCC2.h"


// Initialize 7 DOUT pins for 7 segment leds
void seven_seg_FND_init(void); 

// Select display: 0 to 3
// Display a number 0 - 9 only
void seven_seg_FND_display(uint8_t  num, uint8_t select);
PinName_t pinsFND[12]={PB_7, PB_6, PB_5, PB_4, PB_3, PB_2, PB_1, PB_0, PC_3, PC_4, PA_11, PA_10};
PinName_t pinsnum[8]={PB_7, PB_6, PB_5, PB_4, PB_3, PB_2, PB_1, PB_0};
PinName_t pinsSelect[4]={ PC_3, PC_4, PA_11, PA_10};


 uint8_t value[10] = {
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01001101,
    0b01111101,
    0b00100111,
    0b01111111,
    0b01101111,
    };
void setup(void){
    // Intialize System Clock
    RCC_HSI_init();
    
    // Intialize DOUT Pins      
    // and Others
    // [YOUR CODE GOES HERE]    
    seven_seg_FND_init(); 
};

int main(void) {
    setup();
    uint8_t numDisplay=8;
    uint8_t selectFND=0;
   
    
    while (1) {
        seven_seg_FND_display(numDisplay,selectFND);
    }
}


// Initialize DOUT pins for 7 segment leds
void seven_seg_FND_init(void){	
    for(int i=0;i<12;i++)
    {
    GPIO_init(pinsFND[i], OUTPUT); 
    GPIO_otype(pinsFND[i], pushpull);
    }
}
void seven_seg_FND_display(uint8_t  num, uint8_t select){
    GPIO_write(pinsSelect[select], 1); 
    for (int i=0; i<8;i++){
         GPIO_write(pinsnum[7-i], (value[num]>>i )& 0x01); 
    }   
}
