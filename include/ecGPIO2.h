/*----------------------------------------------------------------\
@ Embedded Controller by Sang-Yoon Kim - Handong Global University
Author           : Sang Yoon Kim
Created          : 09-16-2025
Modified         : 09-16-2025
Language/ver     : C in Keil uVision

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
#define EC_NO_PUPD  0   // 00: no pull
#define EC_PU       1   // 01: pull-up
#define EC_PD       2   // 10: pull-down
#define EC_HIGH 11
#define pull-up 0
#define open-drain 1
#define EC_NONE 0

#define HIGH 1
#define LOW  0

#define LED_PIN 
#define BUTTON_PIN


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
#define EC_NONE 0
#define EC_LOW 0
#define EC_MEDIUM 0
#define EC_FAST 2
#define EC_HIGH 3

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
	 
void GPIO_init(PinName_t pinName, int mode);
void GPIO_write(PinName_t pinName, int Output);
int  GPIO_read(PinName_t pinName);
void GPIO_mode(PinName_t pinName, int mode);
void GPIO_ospeed(PinName_t pinName, int speed);
void GPIO_otype(PinName_t pinName, int type);
void GPIO_pupd(PinName_t pinName, int pupd);
// Initialize 7 DOUT pins for 7 segment leds
void seven_seg_FND_init(void); 

// Select display: 0 to 3
// Display a number 0 - 9 only
void seven_seg_FND_display(uint8_t  num, uint8_t select);
void sevensegment_display_MultiPlex(uint8_t num);
void sevenSegment_breadboard(int num);
void LED_toggle(PinName_t pinName);
 
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
