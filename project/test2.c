#include "ecSTM32F4v2.h"

volatile uint32_t adc_raw = 0;
volatile float pressure = 0;

void setup(void){
    RCC_PLL_init();
    SysTick_init();

    ADC_init(PA_0);     // 압력센서 핀
     UART2_init();         // USB 시리얼 (PA_2, PA_3)
    UART2_baud(9600);

}

int main(void){
    setup();

    while(1){
        float voltage = adc_raw * 3.30 / 4095.00;
        pressure = (voltage - 0.50) * (100.00 / 4.00);  // 0.5~4.5V → 0~100kPa 예시

        printf("Pressure: %.2f kPa\r\n", pressure);
        delay_ms(500);
    }
}

void ADC_IRQHandler(void){
    if(is_ADC_EOC()){
        adc_raw = ADC_read();  // 최신 ADC 값 저장
    }
    clear_ADC_OVR();
}
