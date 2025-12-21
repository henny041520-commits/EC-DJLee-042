#ifndef __EC_DHT11_H
#define __EC_DHT11_H

#include "ecSTM32F4v2.h"   // GPIO, TIM, SysTick, UART 등 EC_HAL 전체 포함

typedef struct{
    PinName_t pin;        // DHT11 데이터 핀
} DHT11_t;

// DHT11 초기화 (핀 지정)
void DHT11_init(DHT11_t *dht, PinName_t pinName);

// DHT11에서 온도, 습도 읽기
// 성공 시 0, 실패 시 음수 반환
int DHT11_read(DHT11_t *dht, float *temperature, float *humidity);

#endif
