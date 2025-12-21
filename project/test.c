#include "ecSTM32F4v2.h"
#include "ecDHT11.h"

DHT11_t dht;

void setup(void);

int main(void){
    setup();
    printf("Start DHT11 Test\r\n");

    while(1){
        float temp = 0.0f;
        float humi = 0.0f;

        int ret = DHT11_read(&dht, &temp, &humi);
        if(ret == 0){
            // 소수점 둘째 자리까지 출력
            printf("Temperature = %.2f [C], Humidity = %.2f [%%]\r\n", temp, humi);
        }else{
            printf("DHT11 error (code = %d)\r\n", ret);
        }

        delay_ms(1000);   // 1.00 s 마다 측정
    }
}

void setup(void){
    RCC_PLL_init();       // 시스템 클록 84.00 MHz
    SysTick_init();       // 1.00 ms SysTick

    UART2_init();         // USB 시리얼 (PA_2, PA_3)
    UART2_baud(9600);

    // DHT11 데이터 핀 설정 (예: PB_5, 필요 시 다른 핀으로 변경)
    DHT11_init(&dht, PB_5);

    printf("DHT11 Initialized\r\n");
}
