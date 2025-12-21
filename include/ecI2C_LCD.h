// ecI2C_LCD.h  (핵심 부분만)
#ifndef __EC_I2C_LCD_H
#define __EC_I2C_LCD_H

#include "stm32f411xe.h"
#include "ecGPIO2.h"
#include "ecSysTick2.h"
#include "ecPinNames.h"

#define LCD_I2C_ADDR 0x4E    // 0x27(7bit) → 8bit write 주소 0x4E

void I2C3_init_LCD(PinName_t scl, PinName_t sda);   // FIXED: I2C3 전용 초기화
void LCD1602_init(void);
void LCD1602_clear(void);
void LCD1602_setCursor(uint8_t row, uint8_t col);
void LCD1602_print(const char *str);
void LCD1602_printInt3(int value);

#endif
