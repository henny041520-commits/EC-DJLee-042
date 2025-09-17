#include "ecRCC2.h"
#include "ecGPIO2.h"

#define LED_PIN PB_12
#define BUTTON_PIN PA_4

// Initialiization 
void setup(void) {
	RCC_HSI_init();
	// initialize the pushbutton pin as an input:
	GPIO_init(BUTTON_PIN, INPUT);  
	// initialize the LED pin as an output:
	GPIO_init(LED_PIN, OUTPUT);    

}
	
int main(void) { 
    setup();
    int buttonState = 1;   // 초기 상태: 버튼 안눌림 (Active-Low, 풀업)
    int prevState   = 1;   // 이전 상태 기억
    int count = 0;

    while(1){
        buttonState = GPIO_read(BUTTON_PIN) ? 1 : 0;  // 0/1 정규화

        // rising edge: 눌렸다가(0) 떼는 순간(1)
        if(prevState == 0 && buttonState == 1){
            count++;
        }

        if(count % 2 == 1)
            GPIO_write(LED_PIN, 1);   // LED ON
        else
            GPIO_write(LED_PIN, 0);   // LED OFF

        prevState = buttonState;      // 이전 상태 갱신
    }
}
