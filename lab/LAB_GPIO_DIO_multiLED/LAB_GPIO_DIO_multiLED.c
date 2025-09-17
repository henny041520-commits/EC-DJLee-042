#include "ecRCC2.h"
#include "ecGPIO2.h"

#define LED_PIN2 PB_12
#define LED_PIN3 PB_13
#define LED_PIN4 PB_14
#define LED_PIN5 PB_15
#define BUTTON_PIN PA_4

// Initialiization 
void setup(void) {
	RCC_HSI_init();
	// initialize the pushbutton pin as an input:
	GPIO_init(BUTTON_PIN, INPUT);  
	// initialize the LED pin as an output:
	GPIO_init(LED_PIN2, OUTPUT);   
    GPIO_init(LED_PIN3, OUTPUT); 
    GPIO_init(LED_PIN4, OUTPUT); 
    GPIO_init(LED_PIN5, OUTPUT); 
    // GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
    GPIO_otype(LED_PIN2, pushpull);
    GPIO_otype(LED_PIN3, pushpull);
    GPIO_otype(LED_PIN4, pushpull);
    GPIO_otype(LED_PIN5, pushpull);
    // GPIO Push-Pull    : No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
    GPIO_pupd(LED_PIN2, pullup);
    GPIO_pupd(LED_PIN3, pullup);
    GPIO_pupd(LED_PIN4, pullup);
    GPIO_pupd(LED_PIN5, pullup);
    GPIO_pupd(BUTTON_PIN, pullup);
    // GPIO Speed          : Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
    GPIO_ospeed(LED_PIN2, mediumspeed);
    GPIO_ospeed(LED_PIN3, mediumspeed);
    GPIO_ospeed(LED_PIN4, mediumspeed);
    GPIO_ospeed(LED_PIN5, mediumspeed);

// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
// GPIO Push-Pull    : No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
// GPIO Speed          : Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
}
	
int main(void) { 
    setup();
    int buttonState = 1;   // 초기 상태: 버튼 안눌림 (Active-Low, 풀업)
    int prevState   = 1;   // 이전 상태 기억
    int count = 0;
    int state = 1;

    while(1){
        buttonState = GPIO_read(BUTTON_PIN) ? 1 : 0;  // 0/1 정규화

        // rising edge: 눌렸다가(0) 떼는 순간(1)
        if(prevState == 0 && buttonState == 1){
            count++;
        }
        switch(count){
       case 1 :
            GPIO_write(LED_PIN2, 1);
            break;
        case 2 :
            GPIO_write(LED_PIN2, 0);
            GPIO_write(LED_PIN3,1);   // LED ON
            break;
        case 3 :
            GPIO_write(LED_PIN3, 0);
            GPIO_write(LED_PIN4, 1);   // LED ON
            break;
        case 4 :
            GPIO_write(LED_PIN4, 0);
            GPIO_write(LED_PIN5,1);   // LED ON
            break;
        case 5 :
            GPIO_write(LED_PIN5, 0);
            count=1; 
            break;
        }
         prevState = buttonState;      // 이전 상태 갱신
    }
}
