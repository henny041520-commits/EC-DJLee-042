// ecOLED2.h
#ifndef __EC_OLED2_H
#define __EC_OLED2_H

#include "stm32f411xe.h"
#include "ecI2C2.h"

// OLED 해상도 (일반적인 0.96" I2C OLED 기준)
#define OLED_WIDTH   128
#define OLED_HEIGHT   64

// SSD1306 I2C 7-bit 주소 (일반적으로 0x3C 또는 0x3D)
// 여기서는 0x3C 가정
#define SSD1306_ADDR  0x3C

#ifdef __cplusplus
extern "C" {
#endif

void OLED_init(void);                 // SSD1306 초기화 시퀀스
void OLED_clear(void);                // 화면 전체 클리어
void OLED_fill(uint8_t pattern);      // 전체 화면 패턴 채우기 (테스트용)
void OLED_testPattern(void);          // 간단한 테스트 패턴 (줄무늬)

// (필요 시 확장용) 페이지/컬럼 이동
void OLED_setPageCol(uint8_t page, uint8_t col);

#ifdef __cplusplus
}
#endif

#endif
