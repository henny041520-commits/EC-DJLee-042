#include "ecSTM32F4v2.h"// this should include #include "ecUART2.h" 
//#include "ecUART2.h" 

static volatile uint8_t PC_Data = 0;
static volatile uint8_t BT_Data = 0;
#define LED_PIN PA_5
uint8_t PC_string[]="Loop:\r\n";

void setup(void){
	RCC_PLL_init();
	SysTick_init();
	
	// USART2: USB serial init
	UART2_init();
	UART2_baud(BAUD_9600);
	GPIO_init(LED_PIN,OUTPUT);
	// USART1: BT/MCU2 serial init 
	UART1_init();
	UART1_baud(BAUD_9600);
}

int main(void){	
	setup();
	printf("MCU Initialized\r\n");	
	while(1){
		
     
	}
}

void USART2_IRQHandler(){          		// USART2 RX Interrupt : Recommended
	if(is_USART2_RXNE()){
		PC_Data = USART2_read();

		if(PC_Data == 'H'||PC_Data == 'h') GPIO_write(LED_PIN,HIGH);
		USART2_write(&PC_Data,1);
        if(PC_Data == 'L'||PC_Data == 'l') GPIO_write(LED_PIN,LOW);
		USART2_write(&PC_Data,1);
	}
}

// void USART1_IRQHandler(){          		// USART2 RX Interrupt : Recommended
// 	if(is_USART1_RXNE()){
// 		BT_Data = USART1_read();
// 		if(&BT_Data == 0) GPIO_write(LED_PIN,LOW);
// 		else if(&BT_Data == 1) 		// RX from UART1 (BT)		
// 		USART2_write(&BT_Data,1); // TX to USART2(PC)
// 	}
// }