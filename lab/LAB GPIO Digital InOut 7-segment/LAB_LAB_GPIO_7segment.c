
#include "ecSTM32F4v2.h"
void setup(void){
    RCC_HSI_init();
    seven_seg_FND_init(PA_4); 
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

