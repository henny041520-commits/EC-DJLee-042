
#include "ecSTM32F4v2.h"
void setup(void){
    RCC_HSI_init(); 
    RCC_PLL_init();              	
    SysTick_init(); 
    seven_seg_FND_init(PA_4);
};
int main(void) {
    setup();
    uint8_t selectFND=0;  
    uint8_t count = 0;
    while (1) {
        delay_ms(1000);
        count++;
        seven_seg_FND_display(count%10,selectFND);
        
    }
}

