# Tutorial: Documentation

## EC API Documentation

[See Class Github](https://github.com/ykkimhgu/EC-student/blob/main/docs/EC_HAL_Documentation.md) for the example documentation

[See Tutorial: Documentation with Markdown](https://ykkim.gitbook.io/ec/numerical-programming/ta-tutorial/tutorial-documentation-with-markdown#preparation)

You must submit your API documentation before the last day of class

### Embedded Controller - STM32F411 Driver Library

Written by: Your Name

Program: C/C++

IDE/Compiler: Keil uVision 5

OS: Win10/11

MCU: STM32F411RE, Nucleo-64

**Table of Contents**

- [GPIO Digital In/Out](https://ykkim.gitbook.io/ec/ec-course/#gpio-digital-inout)
    
    - [Header File](https://ykkim.gitbook.io/ec/ec-course/#header-file)
        
    - [GPIO_init()](https://ykkim.gitbook.io/ec/ec-course/#gpio-init----)
        
    - [GPIO_mode()](https://ykkim.gitbook.io/ec/ec-course/#gpio-mode----)
        
    - [GPIO_write()](https://ykkim.gitbook.io/ec/ec-course/#gpio-write----)
        
    - [GPIO_read()](https://ykkim.gitbook.io/ec/ec-course/#gpio-read----)
        
    - [GPIO_ospeed()](https://ykkim.gitbook.io/ec/ec-course/#gpio-ospeed----)
        
    - [GPIO_otype()](https://ykkim.gitbook.io/ec/ec-course/#gpio-otype----)
        
    - [GPIO_pupdr()](https://ykkim.gitbook.io/ec/ec-course/#gpio-pupdr----)
        
    - [Example Code](https://ykkim.gitbook.io/ec/ec-course/tutorial)
        
- [EXTI](https://ykkim.gitbook.io/ec/ec-course/#gpio-digital-inout)
    
    - [Header File](https://ykkim.gitbook.io/ec/ec-course/#header-file)
        

---

## GPIO Digital InOut

### Header File

`#include "ecGPIO.h"`

```c++
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
// GPIO Speed        : Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
#define HIGH 1
#define LOW  0
//#define LED_PIN     0x05
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
void seven_seg_FND_init(void);
int readButtonRising(PinName_t pinName);
void delay(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // __ECGPIO2_H
```

### GPIO_init()

Initializes GPIO pins with default setting and Enables GPIO Clock. Mode: In/Out/AF/Analog

```
void GPIO_init(GPIO_TypeDef *Port, int pin, int mode);
```

**Parameters**

- **Port:** Port Number, GPIOA~GPIOH
    
- **pin**: pin number (int) 0~15
    
- **mode**: INPUT(0), OUTPUT(1), AF(02), ANALOG (03)
    

**Example code**

```
GPIO_init(GPIOA, 5, OUTPUT);
GPIO_init(GPIOC, 13, INPUT); //GPIO_init(GPIOC, 13, 0);
```

### GPIO_mode()

Configures GPIO pin modes: In/Out/AF/Analog

```
void GPIO_init(GPIO_TypeDef *Port, int pin, int mode);
```

**Parameters**

- **Port:** Port Number, GPIOA~GPIOH
    
- **pin**: pin number (int) 0~15
    
- **mode**: INPUT (0), OUTPUT (1), AF(02), ANALOG (03)
    

**Example code**

```
GPIO_mode(GPIOA, 5, OUTPUT);
```

### GPIO_write()

Write the data to GPIO pin: High, Low

```
write(GPIO_TypeDef *Port, int pin, int output);
```

**Parameters**

- **Port:** Port Number, GPIOA~GPIOH
    
- **pin**: pin number (int) 0~15
    
- **output**: LOW(0), HIGH(1)
    

**Example code**

```
GPIO_write(GPIOA, 5, 1);  // 1: High
```

### GPIO_read()

Read the data from GPIO pin

```
int  GPIO_read(GPIO_TypeDef *Port, int pin);
```

**Parameters**

- **Port:** Port Number, GPIOA~GPIOH
    
- **pin**: pin number (int) 0~15
    

**Example code**

```
GPIO_read(GPIOC, 13);
```

### GPIO_ospeed()

Configures output speed of GPIO pin : Low, Mid, Fast, High

```
void GPIO_ospeed(GPIO_TypeDef* Port, int pin, int speed);
```

**Parameters**

- **Port:** Port Number, GPIOA~GPIOH
    
- **pin**: pin number (int) 0~15
    
- **speed**: LOW_SPEED(0), MID_SPEED(1), FAST_SPEED(2) , HIGH_SPEED(3)
    

**Example code**

```
GPIO_ospeed(GPIOA, 5, 2);  // 2: FAST_SPEED
```

### GPIO_otype()

Configures output type of GPIO pin: Push-Pull / Open-Drain

```
void GPIO_otype(GPIO_TypeDef* Port, int pin, int type);
```

**Parameters**

- **Port:** Port Number, GPIOA~GPIOH
    
- **pin**: pin number (int) 0~15
    
- **type**: PUSH_PULL(0), OPEN_DRAIN(1)
    

**Example code**

```
GPIO_otype(GPIOA, 5, 0);  // 0: Push-Pull
```

### GPIO_pupdr()

Configures Pull-up/Pull-down mode of GPIO pin: No Pull-up, Pull-down/ Pull-up/ Pull-down/ Reserved

```
void GPIO_pupdr(GPIO_TypeDef* Port, int pin, int pupd);
```

**Parameters**

- **Port:** Port Number, GPIOA~GPIOH
    
- **pin**: pin number (int) 0~15
    
- **pupd**: NO_PUPD(0), PULL_UP(1), PULL_DOWN(2), RESERVED(3)
    

**Example code**

```
GPIO_pupdr(GPIOA, 5, 0);  // 0: No Pull-up, Pull-down
```

### Example Code

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

---

## seven_seg_FND_display()

Show a single decimal digit on the selected position

```c
void seven_seg_FND_display(uint8_t  num, uint8_t select);
```

**Parameters**

- **num** : display number (0~9)
    
- **select** : display position (0~3)
    

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

## seven_seg_FND_init()

Configures GPIO pin: pushpull/mediumspeed/nopupd(Output) , pullup(Input)

```c
void seven_seg_FND_init(PinName_t pinName);
```

**Parameters**

- pinName : Input pin name
    

**Example code**

```c
#include "ecSTM32F4v2.h"
void setup(void){
    RCC_HSI_init();
    seven_seg_FND_init(PA_4);
};
```

## readButtonRising()

Detect 0→1 transition on the given pin and return 1, else 0

```c
int readButtonRising(PinName_t pinName);
```

**Parameters**

- pinName : Pin name that user wants to detect transition (Input)
    

**Example code**

```c
#include "ecSTM32F4v2.h"
void setup(void){
    RCC_HSI_init();
    seven_seg_FND_init(PA_4);
};
```

---

## EXTI

### Header File

`#include "ecEXTI2.h"`

```c++
/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : SSS LAB
Language/ver     : C++ in Keil uVision
Description      : External Interrupt (EXTI) driver for STM32F411RE
/----------------------------------------------------------------*/
#ifndef __EC_EXTI2_H
#define __EC_EXTI2_H

#include "stm32f411xe.h"
#include "ecPinNames.h"

#define FALL 0
#define RISE 1
#define BOTH 2

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

void EXTI_init(PinName_t pinName, int trig_type, int priority);
void EXTI_enable(PinName_t pinName);
void EXTI_disable(PinName_t pinName);
uint32_t is_pending_EXTI(PinName_t pinName);
void clear_pending_EXTI(PinName_t pinName);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __EC_EXTI2_H
```

### EXTI_init()

Initializes EXTI line for the given pin: connects GPIO to EXTI via SYSCFG, sets trigger edge, unmasks interrupt, and enables NVIC IRQ.

```c
void EXTI_init(PinName_t pinName, int trig_type, int priority);
```

**Parameters**

- **pinName**: Pin name (PinName_t). EX) `PC_13`, `PA_0`
    
- **trig_type**: Trigger type
    
    - `FALL(0)` : Falling edge
        
    - `RISE(1)` : Rising edge
        
    - `BOTH(2)` : Both edges
        
- **priority**: NVIC priority level (int)
    
    - **Note**: In the current implementation, this parameter is **not applied** and the priority is set to **0.00** internally.
        

**Register behavior**

- Enables SYSCFG clock: `RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN`
    
- Maps pin to EXTI line: `SYSCFG->EXTICR[pin/4]`
    
- Trigger select: `EXTI->RTSR`, `EXTI->FTSR`
    
- Interrupt mask: `EXTI->IMR`
    
- NVIC enable: `NVIC_EnableIRQ(EXTIx_IRQn)`
    

**Example code**

```c
#include "ecSTM32F4v2.h"
#include "ecEXTI2.h"
#include "ecGPIO2.h"

#define BUTTON PC_13
#define LED    PA_5

void setup(void){
    RCC_PLL_init();
    SysTick_init();

    GPIO_init(LED, OUTPUT);
    GPIO_init(BUTTON, INPUT);
    GPIO_pupd(BUTTON, pullup);

    EXTI_init(BUTTON, FALL, 0);   // priority argument exists but not applied in current code
}

int main(void){
    setup();
    while(1){
        // main loop
    }
}

// PC_13 is EXTI13 -> handled by EXTI15_10_IRQHandler
void EXTI15_10_IRQHandler(void){
    if(is_pending_EXTI(BUTTON)){
        clear_pending_EXTI(BUTTON);
        GPIO_write(LED, !GPIO_read(LED));
    }
}
```

### EXTI_enable()

Unmasks (enables) EXTI interrupt for the given pin line by setting `EXTI->IMR`.

```c
void EXTI_enable(PinName_t pinName);
```

**Parameters**

- **pinName**: Pin name (PinName_t)
    

**Example code**

```c
EXTI_enable(PC_13);
```

### EXTI_disable()

Masks (disables) EXTI interrupt for the given pin line by clearing `EXTI->IMR`.

```c
void EXTI_disable(PinName_t pinName);
```

**Parameters**

- **pinName**: Pin name (PinName_t)
    

**Example code**

```c
EXTI_disable(PC_13);
```

### is_pending_EXTI()

Checks whether the EXTI pending flag is set for the given pin line by reading `EXTI->PR`.

```c
uint32_t is_pending_EXTI(PinName_t pinName);
```

**Parameters**

- **pinName**: Pin name (PinName_t)
    

**Return**

- Returns **1.00** if pending flag is set, else **0.00**
    

**Example code**

```c
if(is_pending_EXTI(PC_13)){
    // pending
}
```

### clear_pending_EXTI()

Clears the EXTI pending flag for the given pin line by writing `1.00` to `EXTI->PR`.

```c
void clear_pending_EXTI(PinName_t pinName);
```

**Parameters**

- **pinName**: Pin name (PinName_t)
    

**Example code**

```c
clear_pending_EXTI(PC_13);
```

### Example Code (EXTI Button Interrupt)

```c
#include "ecSTM32F4v2.h"
#include "ecGPIO2.h"
#include "ecEXTI2.h"

#define LED_PIN    PA_5
#define BUTTON_PIN PC_13

void setup(void){
    RCC_PLL_init();
    SysTick_init();

    GPIO_init(LED_PIN, OUTPUT);

    GPIO_init(BUTTON_PIN, INPUT);
    GPIO_pupd(BUTTON_PIN, pullup);

    EXTI_init(BUTTON_PIN, FALL, 0);
}

int main(void){
    setup();
    while(1){
        // CPU can stay in loop; LED toggles by interrupt
    }
}

void EXTI15_10_IRQHandler(void){
    if(is_pending_EXTI(BUTTON_PIN)){
        clear_pending_EXTI(BUTTON_PIN);
        GPIO_write(LED_PIN, !GPIO_read(LED_PIN));
    }
}
```

---

## I2C (I2C1)

### Header File

`#include "ecI2C2.h"`

```c++
#ifndef __EC_I2C2_H
#define __EC_I2C2_H

#include "stm32f411xe.h"
#include "ecGPIO2.h"
#include "ecRCC2.h"
#include "ecPinNames.h"

#ifdef __cplusplus
extern "C" {
#endif

// I2C speed constant (default 100.00 kHz)
#define I2C_SPEED_100K 100000UL

void I2C1_init(void);                                       // I2C1 init (PB_8 SCL, PB_9 SDA)
void I2C1_start(void);                                      // Generate START condition
void I2C1_stop(void);                                       // Generate STOP condition
void I2C1_writeByte(uint8_t data);                          // Transmit 1 byte
void I2C1_writeTo(uint8_t addr7, const uint8_t *buf, int n);// Burst write to 7-bit addressed device

#ifdef __cplusplus
}
#endif

#endif
```

### I2C1_init()

Initializes I2C1 peripheral and configures pins for I2C operation.

```c
void I2C1_init(void);
```

**Parameters**

- None
    

**Pin mapping (fixed in this library)**

- **SCL**: `PB_8`
    
- **SDA**: `PB_9`
    

**Register behavior (typical)**

- Enable I2C1 clock (RCC APB1)
    
- Configure GPIOB AF mode + Open-Drain + Pull-up for PB8/PB9
    
- Configure I2C timing and enable I2C1 (`I2C1->CR1`, `I2C1->CCR`, `I2C1->TRISE`)
    

**Example code**

```c
#include "ecSTM32F4v2.h"
#include "ecI2C2.h"

void setup(void){
    RCC_PLL_init();
    SysTick_init();
    I2C1_init();
}

int main(void){
    setup();
    while(1){
        // I2C usage in main loop
    }
}
```

### I2C1_start()

Generates START condition on I2C1 bus.

```c
void I2C1_start(void);
```

**Parameters**

- None
    

**Example code**

```c
I2C1_start();
```

### I2C1_stop()

Generates STOP condition on I2C1 bus.

```c
void I2C1_stop(void);
```

**Parameters**

- None
    

**Example code**

```c
I2C1_stop();
```

### I2C1_writeByte()

Transmits one byte through I2C1 (data register write).

```c
void I2C1_writeByte(uint8_t data);
```

**Parameters**

- **data**: 1-byte data to send
    

**Example code**

```c
I2C1_writeByte(0xA5);
```

### I2C1_writeTo()

Sends `n` bytes to a 7-bit addressed I2C slave device.

```c
void I2C1_writeTo(uint8_t addr7, const uint8_t *buf, int n);
```

**Parameters**

- **addr7**: 7-bit I2C address (example: `0x27`)
    
- **buf**: pointer to transmit buffer
    
- **n**: number of bytes to send
    

**Example code**

```c
uint8_t data[2] = {0x00, 0xFF};
I2C1_writeTo(0x27, data, 2);
```

### Example Code (I2C write burst)

```c
#include "ecSTM32F4v2.h"
#include "ecI2C2.h"

void setup(void){
    RCC_PLL_init();
    SysTick_init();
    I2C1_init();
}

int main(void){
    setup();

    uint8_t buf[3] = {0x01, 0x02, 0x03};
    while(1){
        I2C1_writeTo(0x50, buf, 3);
        delay_ms(100.00);
    }
}
```

---

## ICAP (Input Capture)

### Header File

`#include "ecICAP2.h"`

```c++
#ifndef __EC_ICAP2_H
#define __EC_ICAP2_H

#include "ecPinNames.h"
#include "stm32f411xe.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FIRST  1
#define SECOND 2

#define IC_RISE 0
#define IC_FALL 1
#define IC_BOTH 2

#define IC_1 1
#define IC_2 2
#define IC_3 3
#define IC_4 4

uint32_t ICAP_read(TIM_TypeDef *TIMx, int ch);

void ICAP_pinmap(PinName_t pinName, TIM_TypeDef **TIMx, int *chN);

void ICAP_init(PinName_t pinName);
void ICAP_setup(PinName_t pinName, int ICn, int edge_type);
void ICAP_counter_us(PinName_t pinName, int usec);
uint32_t ICAP_capture(TIM_TypeDef *TIMx, uint32_t ICn);

uint32_t is_CCIF(TIM_TypeDef *TIMx, uint32_t CCnum);
void clear_CCIF(TIM_TypeDef *TIMx, uint32_t CCnum);

#ifdef __cplusplus
}
#endif

#endif
```

### ICAP_pinmap()

Maps `PinName_t` to corresponding timer instance and channel number (CH1~CH4).

```c
void ICAP_pinmap(PinName_t pinName, TIM_TypeDef **TIMx, int *chN);
```

**Parameters**

- **pinName**: input pin
    
- **TIMx**: output pointer to TIM instance (e.g., `TIM2`)
    
- **chN**: output channel number (1~4)
    

**Example code**

```c
TIM_TypeDef *TIMx;
int ch;
ICAP_pinmap(PB_6, &TIMx, &ch);
```

### ICAP_init()

Initializes timer + GPIO AF for input capture on the selected pin.

```c
void ICAP_init(PinName_t pinName);
```

**Parameters**

- **pinName**: input capture pin
    

**Example code**

```c
ICAP_init(PB_6);
```

### ICAP_setup()

Configures capture edge and capture input selection (FIRST/SECOND) for the pin’s timer channel.

```c
void ICAP_setup(PinName_t pinName, int ICn, int edge_type);
```

**Parameters**

- **pinName**: input capture pin
    
- **ICn**: `FIRST(1)` or `SECOND(2)` (IC selection according to CHn)
    
- **edge_type**:
    
    - `IC_RISE(0)` rising
        
    - `IC_FALL(1)` falling
        
    - `IC_BOTH(2)` both edges
        

**Example code**

```c
ICAP_setup(PB_6, FIRST, IC_RISE);
```

### ICAP_counter_us()

Sets timer counter clock so that the timer counts in microseconds (1.00 us per tick) based on timer clock and prescaler.

```c
void ICAP_counter_us(PinName_t pinName, int usec);
```

**Parameters**

- **pinName**: input capture pin
    
- **usec**: tick in microseconds (e.g., `1` means 1.00 us/tick)
    

**Example code**

```c
ICAP_counter_us(PB_6, 1);
```

### ICAP_capture()

Returns captured CCR value of selected IC channel from a given TIM.

```c
uint32_t ICAP_capture(TIM_TypeDef *TIMx, uint32_t ICn);
```

**Parameters**

- **TIMx**: timer instance
    
- **ICn**: `IC_1(1)` ~ `IC_4(4)` (select CCR1~CCR4)
    

**Return**

- Captured counter value (uint32_t)
    

**Example code**

```c
uint32_t t = ICAP_capture(TIM2, IC_1);
```

### is_CCIF() / clear_CCIF()

Checks/clears capture compare interrupt flag (CCxIF) in `TIMx->SR`.

```c
uint32_t is_CCIF(TIM_TypeDef *TIMx, uint32_t CCnum);
void clear_CCIF(TIM_TypeDef *TIMx, uint32_t CCnum);
```

**Parameters**

- **TIMx**: timer instance
    
- **CCnum**: 1~4 (CC1IF~CC4IF)
    

**Example code**

```c
if(is_CCIF(TIM2, 1)){
    clear_CCIF(TIM2, 1);
}
```

### Example Code (Basic input capture polling)

```c
#include "ecSTM32F4v2.h"
#include "ecICAP2.h"

#define ECHO_PIN PB_6

void setup(void){
    RCC_PLL_init();
    SysTick_init();

    ICAP_init(ECHO_PIN);
    ICAP_counter_us(ECHO_PIN, 1);
    ICAP_setup(ECHO_PIN, FIRST, IC_RISE);
}

int main(void){
    setup();

    TIM_TypeDef *TIMx;
    int ch;
    ICAP_pinmap(ECHO_PIN, &TIMx, &ch);

    while(1){
        if(is_CCIF(TIMx, ch)){
            uint32_t cap = ICAP_read(TIMx, ch);
            clear_CCIF(TIMx, ch);
            // cap: captured time (tick = 1.00 us if configured)
        }
    }
}
```

---

## MFRC522 (RC522, SPI1)

### Header File

`#include "ecMFRC522.h"`

```c
#ifndef __EC_MFRC522_H
#define __EC_MFRC522_H

#include "ecSTM32F4v2.h"

// Pin mapping (edit if needed for your project)
#define RC522_CS    PB_6      // Chip Select
#define RC522_RST   PB_7      // Reset

// ... (register/command defines)

// Card detection + UID read API
uint8_t RC522_request(uint8_t reqMode, uint8_t *tagType);
uint8_t RC522_anticoll(uint8_t *serNum);
uint8_t RC522_isNewCardPresent(void);
uint8_t RC522_readCardUID(uint8_t *uid, uint8_t *uidSize);

#ifdef __cplusplus
extern "C" {
#endif

void     RC522_SPI1_init(void);
uint8_t  RC522_SPI1_transfer(uint8_t data);

void     RC522_writeReg(uint8_t reg, uint8_t value);
uint8_t  RC522_readReg(uint8_t reg);
void     RC522_setBitMask(uint8_t reg, uint8_t mask);
void     RC522_clearBitMask(uint8_t reg, uint8_t mask);

void     RC522_reset(void);
void     RC522_antennaOn(void);
void     RC522_init(void);
uint8_t  RC522_readVersion(void);

#ifdef __cplusplus
}
#endif

#endif
```

### RC522_init()

Initializes RC522 module: SPI1 init, reset sequence, timer/config registers setup, and antenna enable.

```c
void RC522_init(void);
```

**Parameters**

- None
    

**Example code**

```c
RC522_init();
```

### RC522_isNewCardPresent()

Checks whether a new card/tag is present in RF field.

```c
uint8_t RC522_isNewCardPresent(void);
```

**Return**

- Non-zero when card detected, else 0.00
    

**Example code**

```c
if(RC522_isNewCardPresent()){
    // card detected
}
```

### RC522_readCardUID()

Reads UID from detected ISO14443A tag (supports up to 7-byte UID by returning size).

```c
uint8_t RC522_readCardUID(uint8_t *uid, uint8_t *uidSize);
```

**Parameters**

- **uid**: UID output buffer (recommend 10 bytes)
    
- **uidSize**: output UID length (4 or 7 typically)
    

**Return**

- `MI_OK(0)` on success, else error code
    

**Example code**

```c
uint8_t uid[10] = {0};
uint8_t uidSize = 0;

if(RC522_isNewCardPresent()){
    if(RC522_readCardUID(uid, &uidSize) == MI_OK){
        // uid[0..uidSize-1]
    }
}
```

### RC522_readVersion()

Reads MFRC522 version register (`VersionReg`).

```c
uint8_t RC522_readVersion(void);
```

**Return**

- Version register value
    

**Example code**

```c
uint8_t ver = RC522_readVersion();
```

### Low-level SPI / Register access

```c
void    RC522_SPI1_init(void);
uint8_t RC522_SPI1_transfer(uint8_t data);

void    RC522_writeReg(uint8_t reg, uint8_t value);
uint8_t RC522_readReg(uint8_t reg);
void    RC522_setBitMask(uint8_t reg, uint8_t mask);
void    RC522_clearBitMask(uint8_t reg, uint8_t mask);
```

**Use case**

- Direct register configuration/debugging (e.g., FIFO/IRQ/ErrorReg inspection)
    

### Example Code (UID print style usage)

```c
#include "ecSTM32F4v2.h"
#include "ecMFRC522.h"

void setup(void){
    RCC_PLL_init();
    SysTick_init();
    RC522_init();
}

int main(void){
    setup();

    uint8_t uid[10] = {0};
    uint8_t uidSize = 0;

    while(1){
        if(RC522_isNewCardPresent()){
            if(RC522_readCardUID(uid, &uidSize) == MI_OK){
                // uidSize: 4 or 7
                // uid[0..uidSize-1]
            }
        }
        delay_ms(50.00);
    }
}
```

---

## PN532 (NFC, UART)

### Header File

`#include "ecPN532.h"`

```c
#ifndef __EC_PN532_H
#define __EC_PN532_H

#include "stm32f411xe.h"
#include "ecUART2.h"
#include "ecSysTick2.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PN532_OK                0
#define PN532_ERR_TIMEOUT      -1
#define PN532_ERR_PROTOCOL     -2
#define PN532_ERR_ACK          -3
#define PN532_ERR_PARAM        -4

void PN532_UART_init(void);
int  PN532_SAMConfiguration(void);

int  PN532_GetFirmwareVersion(uint8_t *ic,
                              uint8_t *ver,
                              uint8_t *rev,
                              uint8_t *support);

int  PN532_InListPassiveTarget14443A(uint8_t *uid_buf, uint8_t *uid_len);

int  PN532_SendCommand(uint8_t *cmd, uint8_t cmd_len,
                       uint8_t *resp_buf, uint8_t *resp_len,
                       uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
```

### PN532_UART_init()

Initializes UART interface for PN532 communication (UART2 based, typical 115200.00 bps use).

```c
void PN532_UART_init(void);
```

**Parameters**

- None
    

**Example code**

```c
PN532_UART_init();
```

### PN532_SAMConfiguration()

Configures PN532 SAM (Secure Access Module) to normal mode for passive target detection.

```c
int PN532_SAMConfiguration(void);
```

**Return**

- `PN532_OK(0)` on success, else error code
    

**Example code**

```c
if(PN532_SAMConfiguration() != PN532_OK){
    // handle error
}
```

### PN532_GetFirmwareVersion()

Reads PN532 firmware version information.

```c
int PN532_GetFirmwareVersion(uint8_t *ic,
                             uint8_t *ver,
                             uint8_t *rev,
                             uint8_t *support);
```

**Parameters**

- **ic**: IC identifier output
    
- **ver**: major version output
    
- **rev**: minor revision output
    
- **support**: supported features output
    

**Return**

- `PN532_OK(0)` or error code
    

**Example code**

```c
uint8_t ic=0, ver=0, rev=0, sup=0;
PN532_GetFirmwareVersion(&ic, &ver, &rev, &sup);
```

### PN532_InListPassiveTarget14443A()

Detects ISO14443A passive target (tag) and reads UID (up to 7 bytes).

```c
int PN532_InListPassiveTarget14443A(uint8_t *uid_buf, uint8_t *uid_len);
```

**Parameters**

- **uid_buf**: UID output buffer (recommend 10 bytes)
    
- **uid_len**: UID length output
    

**Return**

- `PN532_OK(0)` or error code
    

**Example code**

```c
uint8_t uid[10] = {0};
uint8_t uid_len = 0;

if(PN532_InListPassiveTarget14443A(uid, &uid_len) == PN532_OK){
    // uid[0..uid_len-1]
}
```

### PN532_SendCommand()

Low-level command/response transaction helper with timeout.

```c
int PN532_SendCommand(uint8_t *cmd, uint8_t cmd_len,
                      uint8_t *resp_buf, uint8_t *resp_len,
                      uint32_t timeout_ms);
```

**Parameters**

- **cmd**: command buffer
    
- **cmd_len**: command length
    
- **resp_buf**: response output buffer
    
- **resp_len**: response length output
    
- **timeout_ms**: timeout in milliseconds
    

**Return**

- `PN532_OK(0)` or error code (`PN532_ERR_TIMEOUT`, `PN532_ERR_PROTOCOL`, ...)
    

**Example code**

```c
uint8_t cmd[2] = {0xD4, 0x02};     // example frame payload style
uint8_t resp[64] = {0};
uint8_t rlen = 0;
PN532_SendCommand(cmd, 2, resp, &rlen, 100.00);
```

### Example Code (PN532 basic UID read)

```c
#include "ecSTM32F4v2.h"
#include "ecPN532.h"

void setup(void){
    RCC_PLL_init();
    SysTick_init();
    PN532_UART_init();
    PN532_SAMConfiguration();
}

int main(void){
    setup();

    uint8_t uid[10] = {0};
    uint8_t uid_len = 0;

    while(1){
        if(PN532_InListPassiveTarget14443A(uid, &uid_len) == PN532_OK){
            // uid[0..uid_len-1]
        }
        delay_ms(200.00);
    }
}
```

---

## RCC (Reset & Clock Control)

### Header File

`#include "ecRCC2.h"`

```c++
#ifndef __EC_RCC2_H
#define __EC_RCC2_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

void RCC_HSI_init(void);
void RCC_PLL_init(void);

void RCC_GPIOA_enable(void);
void RCC_GPIOB_enable(void);
void RCC_GPIOC_enable(void);
void RCC_GPIOD_enable(void);
void RCC_GPIOE_enable(void);
void RCC_GPIOH_enable(void);

#define EC_PU 01

extern int EC_SYSCL;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __EC_RCC2_H
```

### RCC_HSI_init()

Enables HSI (16.00 MHz) and selects it as the system clock source.

```c
void RCC_HSI_init(void);
```

**Parameters**

- None
    

**Clock result**

- SYSCLK = **16.00 MHz** (HSI)
    

**Register behavior**

- `RCC->CR` : HSION enable, wait for HSIRDY
    
- `RCC->CFGR` : switch SYSCLK to HSI (`SW=HSI`), wait until `SWS=HSI`
    

**Example code**

```c
#include "ecSTM32F4v2.h"

void setup(void){
    RCC_HSI_init();
    SysTick_init();
}

int main(void){
    setup();
    while(1){
        // run at 16.00 MHz
    }
}
```

### RCC_PLL_init()

Configures PLL using HSI as source and switches SYSCLK to PLL output.

```c
void RCC_PLL_init(void);
```

**Parameters**

- None
    

**Clock result (as coded)**

- PLL input = HSI = **16.00 MHz**
    
- PLLM = **8.00**
    
- PLLN = **84.00**
    
- PLLP = **2.00**
    
- SYSCLK = **84.00 MHz**
    
- APB1 prescaler = **2.00** → PCLK1 = **42.00 MHz**
    
- APB2 prescaler = **1.00** → PCLK2 = **84.00 MHz**
    

**Register behavior**

- `FLASH->ACR` latency set to 2WS
    
- `RCC->PLLCFGR` sets PLL source=HSI, PLLM/PLLN/PLLP
    
- `RCC->CR` PLLON enable, wait PLLRDY
    
- `RCC->CFGR` switch SYSCLK to PLL and set prescalers
    

**Example code**

```c
#include "ecSTM32F4v2.h"

void setup(void){
    RCC_PLL_init();   // SYSCLK = 84.00 MHz
    SysTick_init();
}

int main(void){
    setup();
    while(1){
        // run at 84.00 MHz
    }
}
```

### RCC_GPIOx_enable()

Enables AHB1 clock for the selected GPIO port.

```c
void RCC_GPIOA_enable(void);
void RCC_GPIOB_enable(void);
void RCC_GPIOC_enable(void);
void RCC_GPIOD_enable(void);
void RCC_GPIOE_enable(void);
void RCC_GPIOH_enable(void);
```

**Parameters**

- None
    

**Register behavior**

- `RCC->AHB1ENR |= RCC_AHB1ENR_GPIOxEN`
    

**Example code**

```c
RCC_GPIOA_enable();
RCC_GPIOC_enable();
```

---

## SysTick (1.00 ms time base)

### Header File

`#include "ecSysTick2.h"`

```c++
#ifndef __EC_SYSTICK2_H
#define __EC_SYSTICK2_H

#include "stm32f4xx.h"
#include "ecRCC2.h"
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

void SysTick_init(void);
void SysTick_Handler(void);
void SysTick_counter(void);
void delay_ms(uint32_t msec);
void SysTick_reset(void);
uint32_t SysTick_val(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __EC_SYSTICK2_H
```

### SysTick_init()

Initializes SysTick timer to generate an interrupt every **1.00 ms** (configured for **84.00 MHz** PLL clock in this code).

```c
void SysTick_init(void);
```

**Parameters**

- None
    

**Notes (important)**

- The reload value is fixed to `84.00 MHz / 1000 - 1`, so if SYSCLK is **16.00 MHz (HSI)**, the tick period will be wrong.
    

**Register behavior**

- `SysTick->CTRL` selects processor clock, enables interrupt, enables counter
    
- `SysTick->LOAD` sets reload for 1.00 ms tick (PLL basis)
    
- NVIC SysTick IRQ enable
    

**Example code**

```c
#include "ecSTM32F4v2.h"

void setup(void){
    RCC_PLL_init();
    SysTick_init();
}

int main(void){
    setup();
    while(1){
        delay_ms(500);
        // 500.00 ms delay
    }
}
```

### SysTick_Handler() / SysTick_counter()

SysTick ISR calls `SysTick_counter()` which increments the global millisecond counter.

```c
void SysTick_Handler(void);
void SysTick_counter(void);
```

**Parameters**

- None
    

### delay_ms()

Busy-wait delay using the SysTick millisecond counter.

```c
void delay_ms(uint32_t msec);
```

**Parameters**

- **msec**: delay time in milliseconds
    

**Behavior**

- Waits until `(msTicks - start) >= msec`
    
- Then resets `msTicks = 0`
    

**Example code**

```c
delay_ms(1000);   // 1000.00 ms delay
```

### SysTick_reset()

Resets current SysTick counter value register.

```c
void SysTick_reset(void);
```

**Example code**

```c
SysTick_reset();
```

### SysTick_val()

Returns current SysTick VAL register value.

```c
uint32_t SysTick_val(void);
```

**Return**

- Current down-counter value (`SysTick->VAL`)
    

**Example code**

```c
uint32_t now = SysTick_val();
```

---

## PWM (Timer PWM Output)

### Header File

`#include "ecPWM2.h"`

```c++
#ifndef __EC_PWM2_H
#define __EC_PWM2_H

#include "stm32f411xe.h"
#include "ecGPIO2.h"
#include "ecTIM2.h"
#include "ecPinNames.h"

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

// Default: 84.00 MHz PLL, 1.00 MHz CK_CNT, 50.00 % duty, 1.00 ms period
void PWM_init(PinName_t pinName);
void PWM_pinmap(PinName_t pinName, TIM_TypeDef **TIMx, int *chN);

// Period
void PWM_period(PinName_t pinName,  uint32_t msec);
void PWM_period_ms(PinName_t pinName,  uint32_t msec);
void PWM_period_us(PinName_t pinName, uint32_t usec);

// Pulse / Duty
void PWM_pulsewidth(PinName_t pinName, double pulse_width_ms);
void PWM_pulsewidth_ms(PinName_t pinName, double pulse_width_ms);
void PWM_pulsewidth_us(PinName_t pinName, double pulse_width_us);
void PWM_duty(PinName_t pinName, float duty);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __EC_PWM2_H
```

### PWM_init()

Initializes PWM output on the given pin (GPIO AF + Timer PWM mode).

```c
void PWM_init(PinName_t pinName);
```

**Parameters**

- **pinName**: PWM output pin (PinName_t)
    

**Default behavior (as coded)**

- Counter: Up-counting (`TIMx->CR1.DIR = 0`)
    
- Timer base initialized by `TIM_init(TIMx)` (library default)
    
- PWM Mode 1 on selected channel
    
- Default duty = **50.00 %** (`CCR = ARR/2`)
    
- Enables output compare channel and starts timer
    
- If `TIMx == TIM1`, enables `BDTR.MOE`
    

**Supported pin mapping (PWM_pinmap)**

- GPIOA: `PA_0(TIM2_CH1)`, `PA_1(TIM2_CH2)`, `PA_5(TIM2_CH1)`, `PA_6(TIM3_CH1)`, `PA_8(TIM1_CH1)`, `PA_9(TIM1_CH2)`, `PA_10(TIM1_CH3)`, `PA_15(TIM2_CH1)`
    
- GPIOB: `PB_3(TIM2_CH2)`, `PB_4(TIM3_CH1)`, `PB_5(TIM3_CH2)`, `PB_6(TIM4_CH1)`, `PB_7(TIM4_CH2)`, `PB_8(TIM4_CH3)`, `PB_9(TIM4_CH4)`, `PB_10(TIM2_CH3)`
    
- GPIOC: `PC_6(TIM3_CH1)`, `PC_7(TIM3_CH2)`, `PC_8(TIM3_CH3)`, `PC_9(TIM3_CH4)`
    

**Example code (1.00 kHz, 25.00 % duty)**

```c
#include "ecSTM32F4v2.h"
#include "ecPWM2.h"

#define PWM_OUT PB_6   // TIM4_CH1

void setup(void){
    RCC_PLL_init();
    SysTick_init();

    PWM_init(PWM_OUT);
    PWM_period_us(PWM_OUT, 1000);   // 1000.00 us = 1.00 kHz
    PWM_duty(PWM_OUT, 0.25f);       // 25.00 %
}

int main(void){
    setup();
    while(1){
    }
}
```

### PWM_period_ms() / PWM_period()

Sets PWM period in milliseconds using `TIM_period_ms(TIMx, msec)`.

```c
void PWM_period_ms(PinName_t pinName, uint32_t msec);
void PWM_period(PinName_t pinName, uint32_t msec);
```

**Parameters**

- **pinName**: PWM pin
    
- **msec**: period in ms (comment range: **1.00 ~ 2000.00 ms**)
    

**Example code**

```c
PWM_period_ms(PB_6, 20);   // 20.00 ms period
```

### PWM_period_us()

Sets PWM period in microseconds using `TIM_period_us(TIMx, usec)`.

```c
void PWM_period_us(PinName_t pinName, uint32_t usec);
```

**Parameters**

- **pinName**: PWM pin
    
- **usec**: period in us (comment range: **1.00 ~ 1000.00 us**)
    

**Example code**

```c
PWM_period_us(PB_6, 2000);   // (range note: library comment says up to 1000.00 us)
```

### PWM_pulsewidth_ms() / PWM_pulsewidth()

Sets the high pulse width in **milliseconds** by writing CCRx.

```c
void PWM_pulsewidth(PinName_t pinName, double pulse_width_ms);
void PWM_pulsewidth_ms(PinName_t pinName, double pulse_width_ms);
```

**Parameters**

- **pinName**: PWM pin
    
- **pulse_width_ms**: high time in ms
    

**Clock assumption**

- If SYSCLK source is PLL → uses **84.00 MHz** basis (fsys=84000 for ms-scale)
    
- If SYSCLK source is HSI → uses **16.00 MHz** basis (fsys=16000 for ms-scale)
    

**Example code (servo pulse style)**

```c
PWM_period_ms(PA_8, 20);        // 20.00 ms period
PWM_pulsewidth_ms(PA_8, 1.50);  // 1.50 ms high
```

### PWM_pulsewidth_us()

Sets the high pulse width in **microseconds** by writing CCRx.

```c
void PWM_pulsewidth_us(PinName_t pinName, double pulse_width_us);
```

**Parameters**

- **pinName**: PWM pin
    
- **pulse_width_us**: high time in us
    

**Example code**

```c
PWM_period_ms(PA_8, 20);        // 20.00 ms
PWM_pulsewidth_us(PA_8, 1500);  // 1500.00 us
```

### PWM_duty()

Sets duty ratio (0.00 ~ 1.00) by `CCR = (ARR+1)*duty`.

```c
void PWM_duty(PinName_t pinName, float duty);
```

**Parameters**

- **pinName**: PWM pin
    
- **duty**: duty ratio (0.00 ~ 1.00)
    

**Clamping**

- duty ≤ 0.00 → CCR = 0.00
    
- duty ≥ 1.00 → CCR = ARR + 1.00
    

**Example code**

```c
PWM_duty(PB_6, 0.50f);  // 50.00 %
```

---

## Stepper Motor

### Header File

`#include "ecStepper2.h"`

```c++
#include "stm32f411xe.h"
#include "ecGPIO2.h"
#include "ecSysTick2.h"

#ifndef __EC_STEPPER2_H
#define __EC_STEPPER2_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

#define HALF 0
#define FULL 1

typedef struct{
    PinName_t pin1;
    PinName_t pin2;
    PinName_t pin3;
    PinName_t pin4;
    int _step_num;
} Stepper_t;

void Stepper_init(PinName_t pinName1, PinName_t pinName2, PinName_t pinName3, PinName_t pinName4);
void Stepper_setSpeed(long whatSpeed);
void Stepper_step(int steps, int direction, int mode);
void Stepper_stop(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __EC_STEPPER2_H
```

### Stepper_init()

Initializes 4 GPIO pins as stepper coil outputs (digital output).

```c
void Stepper_init(PinName_t pinName1, PinName_t pinName2, PinName_t pinName3, PinName_t pinName4);
```

**Parameters**

- **pinName1~4**: 4 coil driver pins (A, A’, B, B’ order)
    

**GPIO configuration (as coded)**

- Output mode
    
- No pull-up/pull-down
    
- Push-pull
    
- Fast speed (writes `10` to OSPEED)
    

**Example code**

```c
#include "ecSTM32F4v2.h"
#include "ecStepper2.h"

#define IN1 PA_0
#define IN2 PA_1
#define IN3 PA_4
#define IN4 PA_5

void setup(void){
    RCC_PLL_init();
    SysTick_init();
    Stepper_init(IN1, IN2, IN3, IN4);
}
```

### Stepper_setSpeed()

Sets motor speed in RPM by calculating step delay (ms) per step.

```c
void Stepper_setSpeed(long whatSpeed);
```

**Parameters**

- **whatSpeed**: speed in **rev/min (RPM)**
    

**Internal behavior (as coded)**

- `step_per_rev = 64.00 * 32.00 = 2048.00` steps/rev (half-step constant)
    
- `ms_per_rev = 60000.00 / RPM`
    
- `step_delay = ms_per_rev / step_per_rev`
    

**Example code**

```c
Stepper_setSpeed(15);   // 15.00 RPM
```

### Stepper_step()

Rotates stepper by a given number of steps with direction and stepping mode.

```c
void Stepper_step(int steps, int direction, int mode);
```

**Parameters**

- **steps**: number of steps to move
    
- **direction**: 0 or 1 (FSM index)
    
    - 0: forward (code uses next[0])
        
    - 1: reverse (code uses next[1])
        
- **mode**:
    
    - `FULL(1)` : 4-state full-step FSM
        
    - `HALF(0)` : 8-state half-step FSM
        

**Timing**

- Uses `delay_ms(step_delay)` for each step (SysTick required)
    

**Example code (half-step, 1 rev)**

```c
Stepper_setSpeed(10);               // 10.00 RPM
Stepper_step(2048, 0, HALF);        // 2048.00 steps ≈ 1.00 rev (half-step)
```

### Stepper_stop()

Stops stepping and turns all 4 coil outputs off (all pins LOW).

```c
void Stepper_stop(void);
```

**Example code**

```c
Stepper_stop();
```

### Example Code (basic stepper rotation)

```c
#include "ecSTM32F4v2.h"
#include "ecStepper2.h"

#define IN1 PB_0
#define IN2 PB_1
#define IN3 PB_2
#define IN4 PB_3

void setup(void){
    RCC_PLL_init();
    SysTick_init();

    Stepper_init(IN1, IN2, IN3, IN4);
    Stepper_setSpeed(12);     // 12.00 RPM
}

int main(void){
    setup();

    while(1){
        Stepper_step(2048, 0, HALF);   // forward 1.00 rev
        delay_ms(500);
        Stepper_step(2048, 1, HALF);   // reverse 1.00 rev
        delay_ms(500);
    }
}
```

---

## TIM (General Timer / Update Interrupt)

### Header File

`#include "ecTIM2.h"`

```c++
#ifndef __EC_TIM2_H 
#define __EC_TIM2_H
#include "stm32f411xe.h"

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Timer Configuration */
void TIM_init(TIM_TypeDef* TIMx);

void TIM_period(TIM_TypeDef* TIMx, uint32_t msec);
void TIM_period_ms(TIM_TypeDef* TIMx, uint32_t msec);
void TIM_period_us(TIM_TypeDef* TIMx, uint32_t usec);

/* Timer UI Interrupt Configuration */
void TIM_UI_init(TIM_TypeDef* TIMx, uint32_t msec); 
void TIM_UI_enable(TIM_TypeDef* TIMx);
void TIM_UI_disable(TIM_TypeDef* TIMx);

uint32_t is_UIF(TIM_TypeDef *TIMx);
void clear_UIF(TIM_TypeDef *TIMx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __EC_TIM2_H 
```

### TIM_init()

Enables TIMx clock, sets default period, sets up-counter mode, and starts the timer counter.

```c
void TIM_init(TIM_TypeDef* TIMx);
```

**Parameters**

- **TIMx**: Timer instance pointer (`TIM1`, `TIM2`, `TIM3`, `TIM4`, `TIM5`, `TIM9`, `TIM11`)
    

**Default behavior (as coded)**

- Calls `TIM_period_ms(TIMx, 1)` internally (default 1.00 ms “intended”)
    
- Upcounter: `TIMx->CR1.DIR = 0`
    
- Counter enable: `TIMx->CR1.CEN = 1`
    

**Example code**

```c
TIM_init(TIM2);
```

### TIM_period_ms()

Sets TIMx update event period in milliseconds by configuring `PSC` and `ARR`.

```c
void TIM_period_ms(TIM_TypeDef* TIMx, uint32_t msec);
```

**Parameters**

- **TIMx**: Timer instance
    
- **msec**: update period in ms
    

**Clock assumption**

- If SYSCLK source is PLL → **84.00 MHz**
    
- If SYSCLK source is HSI → **16.00 MHz**
    

**Important note (matches current code)**

- `TIM2` and `TIM5` use 32-bit ARR calculation path.
    
- Other timers use 16-bit ARR calculation path **but the code sets PSC = Sys_CLK/1000 (f_cnt = 1.00 kHz)**, so `ARR ≈ msec`.  
    → This is different from the header comment (“100.00 kHz”) and affects achievable resolution/range.
    

**Example code**

```c
TIM_period_ms(TIM2, 10);   // 10.00 ms
```

### TIM_period_us()

Sets TIMx update event period in microseconds by configuring `PSC` and `ARR` for ~1.00 MHz counter clock.

```c
void TIM_period_us(TIM_TypeDef* TIMx, uint32_t usec);
```

**Parameters**

- **TIMx**: Timer instance
    
- **usec**: update period in us
    

**Clock behavior (as coded)**

- `PSC = Sys_CLK / 1,000,000` → counter tick = 1.00 us
    
- `ARR ≈ usec`
    

**Example code**

```c
TIM_period_us(TIM2, 100);   // 100.00 us
```

### TIM_period()

Wrapper for millisecond period setting.

```c
void TIM_period(TIM_TypeDef* TIMx, uint32_t msec);
```

**Example code**

```c
TIM_period(TIM3, 5);   // 5.00 ms
```

### TIM_UI_init()

Initializes TIMx and enables update interrupt (UIE), then enables NVIC IRQ and sets priority.

```c
void TIM_UI_init(TIM_TypeDef* TIMx, uint32_t msec);
```

**Parameters**

- **TIMx**: Timer instance
    
- **msec**: update interrupt period in ms
    

**NVIC behavior (as coded)**

- Enables corresponding IRQn (TIM2_IRQn, TIM3_IRQn, TIM4_IRQn, TIM5_IRQn, TIM1_BRK_TIM9_IRQn, TIM1_TRG_COM_TIM11_IRQn, etc.)
    
- Sets priority to **2.00**
    

**Example code**

```c
TIM_UI_init(TIM2, 10);     // 10.00 ms update interrupt
TIM_UI_enable(TIM2);
```

### TIM_UI_enable() / TIM_UI_disable()

Enables/disables timer update interrupt (UIE bit in `TIMx->DIER`).

```c
void TIM_UI_enable(TIM_TypeDef* TIMx);
void TIM_UI_disable(TIM_TypeDef* TIMx);
```

**Example code**

```c
TIM_UI_enable(TIM4);
TIM_UI_disable(TIM4);
```

### is_UIF() / clear_UIF()

Checks/clears update interrupt flag UIF in `TIMx->SR`.

```c
uint32_t is_UIF(TIM_TypeDef *TIMx);
void clear_UIF(TIM_TypeDef *TIMx);
```

**Return (is_UIF)**

- Returns **1.00** if UIF is set, else **0.00**
    

**Example code**

```c
if(is_UIF(TIM2)){
    clear_UIF(TIM2);
    // do periodic task
}
```

### Example Code (Polling UIF)

```c
#include "ecSTM32F4v2.h"
#include "ecTIM2.h"
#include "ecGPIO2.h"

#define LED PA_5

void setup(void){
    RCC_PLL_init();
    SysTick_init();

    GPIO_init(LED, OUTPUT);

    TIM_init(TIM2);
    TIM_period_ms(TIM2, 500);   // 500.00 ms
}

int main(void){
    setup();
    while(1){
        if(is_UIF(TIM2)){
            clear_UIF(TIM2);
            GPIO_write(LED, !GPIO_read(LED));
        }
    }
}
```

---

## UART / USART (USART1, USART2, USART6)

### Header File

`#include "ecUART2.h"`

```c
#ifndef __EC_USART_H
#define __EC_USART_H

#include <stdio.h>
#include "stm32f411xe.h"
#include "ecGPIO2.h"
#include "ecRCC2.h"

#define BAUD_9600    9600
#define BAUD_19200   19200
#define BAUD_38400   38400
#define BAUD_57600   57600
#define BAUD_115200  115200
#define BAUD_921600  921600

void UART1_init(void);
void UART2_init(void);
void UART6_init(void);

void UART1_baud(uint32_t baud);
void UART2_baud(uint32_t baud);
void UART6_baud(uint32_t baud);

void USART1_write(uint8_t* buffer, uint32_t nBytes);
void USART2_write(uint8_t* buffer, uint32_t nBytes);

uint8_t USART1_read(void);
uint8_t USART2_read(void);

uint32_t is_USART1_RXNE(void);
uint32_t is_USART2_RXNE(void);
uint32_t is_USART6_RXNE(void);

#endif
```

### UART1_init()

Initializes USART1 with default pins and 9600.00 bps.

```c
void UART1_init(void);
```

**Default pins**

- **TX**: `PA_9`
    
- **RX**: `PA_10`
    
- AF7, High speed, Push-pull
    

**Clock domain**

- APB2 (typically 84.00 MHz when PLL is SYSCLK)
    

**Example code**

```c
UART1_init();
UART1_baud(BAUD_115200);
```

### UART2_init()

Initializes USART2 with default pins and 9600.00 bps.

```c
void UART2_init(void);
```

**Default pins**

- **TX**: `PA_2`
    
- **RX**: `PA_3`
    
- AF7, High speed, Push-pull
    

**Clock domain**

- APB1 (typically 42.00 MHz when PLL is SYSCLK)
    

**Example code**

```c
UART2_init();
UART2_baud(BAUD_115200);
```

### UART6_init()

Initializes USART6 using fixed pins and 9600.00 bps (safe init).

```c
void UART6_init(void);
```

**Default pins (fixed in this function)**

- **TX**: `PC_6`
    
- **RX**: `PC_7`
    
- AF8 used for USART6
    

**Important note (matches current code)**

- RXNE interrupt is enabled by `USART_setting()` and NVIC is enabled for USART6.
    

**Example code**

```c
UART6_init();
UART6_baud(BAUD_57600);
```

### UARTx_baud()

Changes baud rate by recalculating `USARTx->BRR` based on bus clock.

```c
void UART1_baud(uint32_t baud);
void UART2_baud(uint32_t baud);
void UART6_baud(uint32_t baud);
```

**Parameters**

- **baud**: baud rate (e.g., `BAUD_115200`)
    

**Clock basis (as coded)**

- USART1/USART6: fCK = **84.00 MHz**
    
- USART2: fCK = **42.00 MHz**
    

**Example code**

```c
UART2_baud(BAUD_921600);
```

### USARTx_write()

Blocking transmit (polls TXE, waits TC at end).

```c
void USART1_write(uint8_t* buffer, uint32_t nBytes);
void USART2_write(uint8_t* buffer, uint32_t nBytes);
```

**Parameters**

- **buffer**: data buffer pointer
    
- **nBytes**: number of bytes
    

**Example code**

```c
uint8_t msg[] = "Hello\r\n";
USART2_write(msg, 7);
```

### USARTx_read()

Blocking receive (waits RXNE then reads `USARTx->DR`).

```c
uint8_t USART1_read(void);
uint8_t USART2_read(void);
```

**Return**

- Received byte (uint8_t)
    

**Example code**

```c
uint8_t c = USART2_read();
```

### is_USARTx_RXNE()

Checks RXNE flag (`USARTx->SR.RXNE`).

```c
uint32_t is_USART1_RXNE(void);
uint32_t is_USART2_RXNE(void);
uint32_t is_USART6_RXNE(void);
```

**Return**

- Returns non-zero when RXNE is set, else 0.00
    

**Example code (non-blocking style)**

```c
if(is_USART2_RXNE()){
    uint8_t c = USART2_read();
}
```

### Example Code (Echo on USART2)

```c
#include "ecSTM32F4v2.h"
#include "ecUART2.h"

void setup(void){
    RCC_PLL_init();
    SysTick_init();

    UART2_init();
    UART2_baud(BAUD_115200);
}

int main(void){
    setup();

    while(1){
        if(is_USART2_RXNE()){
            uint8_t c = USART2_read();
            USART2_write(&c, 1);
        }
    }
}
```

---

