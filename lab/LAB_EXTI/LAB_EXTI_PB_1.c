#include "ecSTM32F4v2.h"
#define BUTTON_PIN   PA_4
volatile uint8_t selectFND=0;  
volatile uint8_t selectFND2=1;  
volatile uint8_t count = 0;
void setup(void){
	RCC_HSI_init();
    RCC_PLL_init();    
    SysTick_init(); 
    seven_seg_FND_init(BUTTON_PIN );           
    EXTI_init(BUTTON_PIN, FALL, 2);  
	seven_seg_FND_display(0,selectFND);
}

int main(void){
    setup();
    while(1){
    seven_seg_FND_display(count/10,selectFND);
	delay_ms(1);
	sevenseg_clear(selectFND);
	seven_seg_FND_display(count%10,selectFND2);
	delay_ms(1);
	sevenseg_clear(selectFND2);
    }
}

// --- EXTI 인터럽트 핸들러 (PA4 → EXTI4) ---
void EXTI4_IRQHandler(void){
	
    if(is_pending_EXTI(BUTTON_PIN)){       // 펜딩 확인
        count++;
		 if(count==20)
        {	 
			count=0;
        }
    } // LED 토글
        clear_pending_EXTI(BUTTON_PIN);    // 펜딩 클리어
    }

 