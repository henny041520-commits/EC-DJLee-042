// ecI2C2.h
#ifndef __EC_I2C2_H
#define __EC_I2C2_H

#include "stm32f411xe.h"
#include "ecGPIO2.h"
#include "ecRCC2.h"
#include "ecPinNames.h"

#ifdef __cplusplus
extern "C" {
#endif

// I2C 속도 상수 (기본 100.00 kHz)
#define I2C_SPEED_100K 100000UL

void I2C1_init(void);                                       // I2C1 초기화 (PB_8 SCL, PB_9 SDA)
void I2C1_start(void);                                      // START 조건 생성
void I2C1_stop(void);                                       // STOP 조건 생성
void I2C1_writeByte(uint8_t data);                          // 1바이트 전송
void I2C1_writeTo(uint8_t addr7, const uint8_t *buf, int n);// 7비트 주소 장치로 연속 전송

#ifdef __cplusplus
}
#endif

#endif
