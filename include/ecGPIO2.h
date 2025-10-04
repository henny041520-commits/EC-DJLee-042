/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : SSS LAB
Created          : 05-03-2021
Modified         : 08-23-2024
Language/ver     : C++ in Keil uVision

Description      : Distributed to Students for LAB_GPIO
/----------------------------------------------------------------*/


#ifndef __ECGPIO2_H
#define __ECGPIO2_H

#include "stm32f411xe.h"
#include "ecRCC2.h"
#include "ecPinNames.h"

#define INPUT  0x00
#define OUTPUT 0x01
#define AF     0x02
#define ANALOG 0x03
#define lowspeed 00
#define mediumspeed 01
#define fastspeed 10
#define highspeed 11
#define pushpull 0
#define opendrain 1
#define pullup 01
#define pulldown 10
#define reversed 11
#define nopupd 00
// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
// GPIO Push-Pull    : No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
// GPIO Speed          : Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
#define HIGH 1
#define LOW  0

//#define LED_PIN    0x05
//#define BUTTON_PIN 0x23

#define EC_PU 0

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

void GPIO_init(PinName_t pinName, uint32_t mode);     
void GPIO_write(PinName_t pinName, int Output);
int  GPIO_read(PinName_t pinName);
void GPIO_mode(PinName_t pinName, uint32_t mode);
void GPIO_ospeed(PinName_t pinName, int speed);
void GPIO_otype(PinName_t pinName, int type);
void GPIO_pupd(PinName_t pinName, int pupd);
void seven_seg_FND_display(uint8_t  num, uint8_t select);
void seven_seg_FND_init(PinName_t pinName);
int readButtonRising(PinName_t pinName);
void sevenseg_clear(uint8_t  num);
void delay(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __ECGPIO2_H
