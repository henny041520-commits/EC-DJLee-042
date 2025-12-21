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
		// USART Receive: Use Interrupt only
		// USART Transmit:  Interrupt or Polling
		// sdsdsdsdfdasfsdafggfddfd
	}
}

void USART2_IRQHandler(){          		// USART2 RX Interrupt : Recommended
	if(is_USART2_RXNE()){
		PC_Data = USART2_read();
				// RX from UART2 (PC)
		USART1_write(&PC_Data,1);		// TX to USART2	 (PC)	 Echo of keyboard typing		
	}
}

void USART1_IRQHandler(){          		// USART2 RX Interrupt : Recommended
	if(is_USART1_RXNE()){
		BT_Data = USART1_read();
		if(&BT_Data == 0) GPIO_write(LED_PIN,LOW);
		else if(&BT_Data == 1) GPIO_write(LED_PIN,HIGH);		// RX from UART1 (BT)		
		USART2_write(&BT_Data,1); // TX to USART2(PC)
	}
}