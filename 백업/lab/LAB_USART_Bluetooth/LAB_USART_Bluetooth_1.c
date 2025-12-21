#include "ecSTM32F4v2.h"// this should include #include "ecUART2.h" 
//#include "ecUART2.h" 

static volatile uint8_t PC_Data = 0;
static volatile uint8_t BT_Data = 0;
uint8_t PC_string[]="Loop:\r\n";
void USART2_IRQHandler();
void USART1_IRQHandler();
void out(uint8_t PC_string[])
{
  USART1_write(PC_string, (uint32_t)strlen((char*)PC_string));

}
void setup(void){
  RCC_PLL_init();
  SysTick_init();

  // USART2
  UART2_init();
  UART2_baud(BAUD_9600);
            // ← 필요 시(라이브러리가 자동 안 켜줄 수 있음)
  NVIC_EnableIRQ(USART2_IRQn);       // ← 필요 시

  // USART1
  UART1_init();
  UART1_baud(BAUD_9600);
             // ← ***추가: RXNEIE 세팅(USART_CR1_RXNEIE)***
  NVIC_EnableIRQ(USART1_IRQn);       // ← ***추가: NVIC 인터럽트 허가***
  __enable_irq();                    // (대개 기본값이지만 확실히)
}

int main(void){	
	setup();
	printf("MCU Initialized\r\n");	
	while(1){
		out("Loop:\r\n");
		// USART Receive: Use Interrupt only
		// USART Transmit:  Interrupt or Polling
		USART2_write(PC_string, 7);
		delay_ms(2000);        
	}
}

void USART2_IRQHandler(){          		// USART2 RX Interrupt : Recommended
	if(is_USART2_RXNE()){
		PC_Data = USART2_read();		// RX from UART2 (PC)
		USART2_write(&PC_Data,1);		// TX to USART2	 (PC)	 Echo of keyboard typing
		USART1_write(&PC_Data,1);	
	}
}


void USART1_IRQHandler(){          		// USART2 RX Interrupt : Recommended
	if(is_USART1_RXNE()){
		BT_Data = USART1_read();
			USART1_write(&BT_Data,1);		// RX from UART1 (BT)		
		printf("RX: %c \r\n",BT_Data); // TX to USART2(PC)
	}
}