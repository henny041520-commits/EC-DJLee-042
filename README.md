# Tutorial: Documentation

## Tutorial: Documentation

## EC API Documentation

[See Class Github](https://github.com/ykkimhgu/EC-student/blob/main/docs/EC_HAL_Documentation.md) for the example documentation

[See Tutorial: Documentation with Markdown](https://ykkim.gitbook.io/ec/numerical-programming/ta-tutorial/tutorial-documentation-with-markdown#preparation)

You must submit your API documentation before the last day of class

### Embedded Controller - STM32F411 Driver Library

Written by: Your Name

Program: C/C++

IDE/Compiler: Keil uVision 5

OS: WIn10/11

MCU: STM32F411RE, Nucleo-64

**Table of Contents**

* [GPIO Digital In/Out](https://ykkim.gitbook.io/ec/ec-course/tutorial/..#gpio-digital-inout)
  * [Header File](https://ykkim.gitbook.io/ec/ec-course/tutorial/..#header-file)
  * [GPIO\_init()](https://ykkim.gitbook.io/ec/ec-course/tutorial/..#gpio-init----)
  * [GPIO\_mode()](https://ykkim.gitbook.io/ec/ec-course/tutorial/..#gpio-mode----)
  * [GPIO\_write()](https://ykkim.gitbook.io/ec/ec-course/tutorial/..#gpio-write----)
  * [GPIO\_read()](https://ykkim.gitbook.io/ec/ec-course/tutorial/..#gpio-read----)
  * [GPIO\_ospeed()](https://ykkim.gitbook.io/ec/ec-course/tutorial/..#gpio-ospeed----)
  * [GPIO\_otype()](https://ykkim.gitbook.io/ec/ec-course/tutorial/..#gpio-otype----)
  * [GPIO\_pupdr()](https://ykkim.gitbook.io/ec/ec-course/tutorial/..#gpio-pupdr----)
  * [Example Code](https://ykkim.gitbook.io/ec/ec-course/tutorial)
* [EXTI](https://ykkim.gitbook.io/ec/ec-course/tutorial/..#gpio-digital-inout)
  * [Header File](https://ykkim.gitbook.io/ec/ec-course/tutorial/..#header-file)

***

### GPIO Digital InOut

#### Header File

`#include "ecGPIO.h"`

```c++
/*----------------------------------------------------------------\

@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : SSS LAB
Created          : 05-03-2021
Modified         : 08-23-2024
Language/ver     : C++ in Keil uVision
Description      : Distributed to Students for LAB_GPIO

/----------------------------------------------------------------*/
#ifndef __ECGPIO2_H
#define __ECGPIO2_H
#include "stm32f411xe.h"
#include "ecRCC2.h"
#include "ecPinNames.h"
#define INPUT  0x00
#define OUTPUT 0x01
#define AF     0x02
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
// GPIO Push-Pull    : No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
// GPIO Speed          : Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
#define HIGH 1
#define LOW  0
//#define LED_PIN    0x05
//#define BUTTON_PIN 0x23
#define EC_PU 0
#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
void GPIO_init(PinName_t pinName, uint32_t mode);    
void GPIO_write(PinName_t pinName, int Output);
int  GPIO_read(PinName_t pinName);
void GPIO_mode(PinName_t pinName, uint32_t mode);
void GPIO_ospeed(PinName_t pinName, int speed);
void GPIO_otype(PinName_t pinName, int type);
void GPIO_pupd(PinName_t pinName, int pupd);
void seven_seg_FND_display(uint8_t  num, uint8_t select);
void seven_seg_FND_init(void);
void seven_seg_FND_display(uint8_t  num, uint8_t select);
int readButtonRising(PinName_t pinName);
void delay(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // __ECGPIO2_H
```

#### GPIO\_init()

Initializes GPIO pins with default setting and Enables GPIO Clock. Mode: In/Out/AF/Analog

```
void GPIO_init(GPIO_TypeDef *Port, int pin, int mode);
```

**Parameters**

* **Port:** Port Number, GPIOA\~GPIOH
* **pin**: pin number (int) 0\~15
* **mode**: INPUT(0), OUTPUT(1), AF(02), ANALOG (03)

**Example code**

```
GPIO_init(GPIOA, 5, OUTPUT);
GPIO_init(GPIOC, 13, INPUT); //GPIO_init(GPIOC, 13, 0);
```

#### GPIO\_mode()

Configures GPIO pin modes: In/Out/AF/Analog

```
void GPIO_init(GPIO_TypeDef *Port, int pin, int mode);
```

**Parameters**

* **Port:** Port Number, GPIOA\~GPIOH
* **pin**: pin number (int) 0\~15
* **mode**: INPUT (0), OUTPUT (1), AF(02), ANALOG (03)

**Example code**

```
GPIO_mode(GPIOA, 5, OUTPUT);
```

#### GPIO\_write()

Write the data to GPIO pin: High, Low

```
write(GPIO_TypeDef *Port, int pin, int output);
```

**Parameters**

* **Port:** Port Number, GPIOA\~GPIOH
* **pin**: pin number (int) 0\~15
* **output**: LOW(0), HIGH(1)

**Example code**

```
GPIO_write(GPIOA, 5, 1);  // 1: High
```

#### GPIO\_read()

Read the data from GPIO pin

```
int  GPIO_read(GPIO_TypeDef *Port, int pin);
```

**Parameters**

* **Port:** Port Number, GPIOA\~GPIOH
* **pin**: pin number (int) 0\~15

**Example code**

```
GPIO_read(GPIOC, 13);
```

#### GPIO\_ospeed()

Configures output speed of GPIO pin : Low, Mid, Fast, High

```
void GPIO_ospeed(GPIO_TypeDef* Port, int pin, int speed);
```

**Parameters**

* **Port:** Port Number, GPIOA\~GPIOH
* **pin**: pin number (int) 0\~15
* **speed**: LOW\_SPEED(0), MID\_SPEED(1), FAST\_SPEED(2) , HIGH\_SPEED(3)

**Example code**

```
GPIO_ospeed(GPIOA, 5, 2);  // 2: FAST_SPEED
```

#### GPIO\_otype()

Configures output type of GPIO pin: Push-Pull / Open-Drain

```
void GPIO_otype(GPIO_TypeDef* Port, int pin, int type);
```

**Parameters**

* **Port:** Port Number, GPIOA\~GPIOH
* **pin**: pin number (int) 0\~15
* **type**: PUSH\_PULL(0), OPEN\_DRAIN(1)

**Example code**

```
GPIO_otype(GPIOA, 5, 0);  // 0: Push-Pull
```

#### GPIO\_pupdr()

Configures Pull-up/Pull-down mode of GPIO pin: No Pull-up, Pull-down/ Pull-up/ Pull-down/ Reserved

```
void GPIO_pupdr(GPIO_TypeDef* Port, int pin, int pupd);
```

**Parameters**

* **Port:** Port Number, GPIOA\~GPIOH
* **pin**: pin number (int) 0\~15
* **pupd**: NO\_PUPD(0), PULL\_UP(1), PULL\_DOWN(2), RESERVED(3)

**Example code**

```
GPIO_pupdr(GPIOA, 5, 0);  // 0: No Pull-up, Pull-down
```

#### Example Code

```c++
#include "ecSTM32F411.h"

#define LED_PIN 5
#define BUTTON_PIN 13

// Initialiization 
void setup(void) {
	RCC_PLL_init();
	SysTick_init();
	GPIO_init(GPIOA, LED_PIN, OUTPUT);    
}
	
int main(void) { 
	setup();
	
	while(1){
		delay_ms(500);  
		GPIO_write(GPIOA, LED_PIN, LOW);
		delay_ms(500);  
		GPIO_write(GPIOA, LED_PIN, HIGH);
	}
}
```

***

### seven_seg_FND_display()

Show a single decimal digit on the selected position 
```c
void seven_seg_FND_display(uint8_t  num, uint8_t select);
```

**Parameters**

* ***num*** : display number (0~9)
* ***select*** : display position (0~3)

**Example code**

```c
#include "ecSTM32F4v2.h"

void setup(void){
    RCC_HSI_init();
    seven_seg_FND_init();
};
int main(void) {
    setup();
    uint8_t selectFND=0;  
    uint8_t count = 0;
    while (1) {
        if(readButtonRising(PA_4)){
        count++;
}
        if(count==10)
        {
            count=0;
        }
        seven_seg_FND_display(count,selectFND);
    }
}
```

### seven_seg_FND_init()

Configures  GPIO pin: pushpull/mediumspeed/nopupd(Output) , pullup(Input)
```c
void seven_seg_FND_init(PinName_t pinName);
```

**Parameters**

* pinName : Input pin name

**Example code**

```c
#include "ecSTM32F4v2.h"
void setup(void){
    RCC_HSI_init();
    seven_seg_FND_init(PA_4);
};
```

### readButtonRising()

Detect 0→1 transition on the given pin and return 1, else 0
```c
int readButtonRising(PinName_t pinName);
```

**Parameters**

* pinName : Pin name that user wants to detect transition (Input)

**Example code**

```c
#include "ecSTM32F4v2.h"
void setup(void){
    RCC_HSI_init();
    seven_seg_FND_init(PA_4);
};
```

### EXTI


```
```

**Parameters**

* p1
* p2

**Example code**

```
```

### EXTI


```
```

**Parameters**

* p1
* p2

**Example code**

```
```

### EXTI


```
```

**Parameters**

* p1
* p2

**Example code**

```
```

### EXTI


```
```

**Parameters**

* p1
* p2

**Example code**

```
```

### EXTI


```
```

**Parameters**

* p1
* p2

**Example code**

```
```

### EXTI


```
```

**Parameters**

* p1
* p2

**Example code**

```
```

