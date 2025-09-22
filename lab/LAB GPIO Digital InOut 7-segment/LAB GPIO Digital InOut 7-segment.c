#include "stm32f4xx.h"
#include "ecGPIO2.h"
#include "ecRCC2.h"
#include "stdio.h"


void seven_seg_FND_init(void); 
int readButtonRising(PinName_t pinName);
void seven_seg_FND_display(uint8_t  num, uint8_t select);
void setup(void){
    RCC_HSI_init();
    seven_seg_FND_init(); 
};
int main(void) {
    setup();
    
    uint8_t selectFND=0;  
    uint8_t count = 0;
    while (1) {
        if(readButtonRising(PA_4)){
        count++;
}
        if(count==10)
        {
            count=0;
        }
        seven_seg_FND_display(count,selectFND);
    }
}





