# LAB: EXTI & SysTick_22201042_Dongjun,Lee

**Date:** 2025-09-30

**Author/Partner:** Dongjun, Lee

**Github:** https://github.com/henny041520-commits/EC-DJLee-042

**Demo Video:** 
LAB_LED EXTI & SysTick_1 : https://www.youtube.com/shorts/9lOCJE7rf2k
LAB_LED EXTI & SysTick_2 : https://www.youtube.com/shorts/7guBtK5udzo


**PDF version:** https://github.com/henny041520-commits/EC-DJLee-042/blob/main/report/LAB_LED%20EXTI%20%26%20SysTick_22201042_Dongjun%2CLee.pdf

## Introduction

In this lab, you are required to create two simple programs using interrupt:

(1) displaying the number counting from 0 to 19 with Button Press

(2) counting at a rate of 1 second

You must submit

* LAB Report (\*.md & \*.pdf)
* Zip source files(main\*.c, ecRCC2.h, ecGPIO2.h, ecSysTick2.c etc...).
  * Only the source files. Do not submit project files

### Requirement

#### Hardware

* MCU
  * NUCLEO-F411RE
* Actuator/Sensor/Others:
  * eval board

#### Software

* PlatformIO, CMSIS, EC\_HAL library

## Problem 1: Counting numbers on 7-Segment using EXTI Button

### Creating EXTI library

1. [Download sample header files](https://github.com/ykkimhgu/EC-student/tree/main/include/lib-student):&#x20;

   **`ecEXTI2_student.h, ecEXTI2_student.c`**
2. Rename these files as **ecEXTI2.h, ecEXTI2.c**
   * You MUST write your name and other information at the top of the library code files.
   * Save these files in your directory `EC \include\`.
3. Declare and define the following functions in your library  `ecEXTI2.h`

**ecEXTI.h**

```c
void EXTI_init(PinName_t pinName, int trig_type, int priority);
void EXTI_enable(uint32_t pin);  // mask in IMR
void EXTI_disable(uint32_t pin);  // unmask in IMR
uint32_t  is_pending_EXTI(uint32_t pin);
void clear_pending_EXTI(uint32_t pin);
```

### Procedure

1. Create a new project under the directory `\EC\lab\LAB_EXTI`

* The project name is “**LAB\_EXTI”.**
* Create a new source file named as “**LAB\_EXTI.c”**

> You MUST write your name on the source file inside the comment section.

2\. Include your updated library in `\EC\include\` to your project.

* **ecGPIO2.h, ecGPIO2.c**
* **ecRCC2.h, ecRCC2.c**
* **ecEXTI2.h, ecEXTI2.c**

3. First, check if every number, 0 to 9, can be displayed properly on each 7-segment (there are a total of 4 7-segment display on the evaluation board).
4. Then, create a code to display the number counting from 0 to 19 and repeating.
   * Count up only by pressing the push button
   * Must use External Interrupt
   * Refer to [sample codes](https://ykkim.gitbook.io/ec/firmware-programming/example-code#button-interrupt)

### Configuration

| Digital In for Button (B1) | Digital Out for FND-7-Segment                                                                                             |
| -------------------------- | ------------------------------------------------------------------------------------------------------------------------- |
| Digital In                 | Digital Out                                                                                                               |
| PA4                        | <p>PB7,PB6,PB5,PB4,PB3,PB2,PB1,PB0<br>('a'~~'h', respectively)<br>PC3,PC4,PA11,PA10<br>('LED1'~~'LED4', respectively)</p> |
| PULL-UP                    | Push-Pull, No PullUp-PullDown, Medium Speed                                                                               |
## Algorithm

### Overview

#### Mealy FSM Table — Counting numbers on 7-Segment using EXTI Button
## FSM Table (Mealy Machine for 2-Digit FND Multiplexing)

| Current State            | Input Condition        | Next State         | Outputs (SEL_T, SEL_O, SEG[7:0])            | Actions (Mealy)                          |
| ------------------------ | ---------------------- | ------------------ | ------------------------------------------- | ---------------------------------------- |
| **S0: TENS_ON**          | `tick_1ms`             | **S1**             | `SEL_T=1, SEL_O=0, SEG = pattern(count/10)` | —                                        |
| **S0: TENS_ON**          | `btn_fall` *(anytime)* | **S0** (no change) | unchanged                                   | `count ← (count+1) % 20` *(in EXTI ISR)* |
| **S1: BLANK_AFTER_TENS** | `tick_1ms`             | **S2**             | `SEL_T=0, SEL_O=0, SEG = cleared`           | —                                        |
| **S1: BLANK_AFTER_TENS** | `btn_fall`             | **S1**             | unchanged                                   | `count ← (count+1) % 20` *(in EXTI ISR)* |
| **S2: ONES_ON**          | `tick_1ms`             | **S3**             | `SEL_T=0, SEL_O=1, SEG = pattern(count%10)` | —                                        |
| **S2: ONES_ON**          | `btn_fall`             | **S2**             | unchanged                                   | `count ← (count+1) % 20` *(in EXTI ISR)* |
| **S3: BLANK_AFTER_ONES** | `tick_1ms`             | **S0**             | `SEL_T=0, SEL_O=0, SEG = cleared`           | —                                        |
| **S3: BLANK_AFTER_ONES** | `btn_fall`             | **S3**             | unchanged                                   | `count ← (count+1) % 20` *(in EXTI ISR)* |
### Flow chart
<p align="center">
  <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/Flow_Chart_Table_Counting%20numbers%20on%207-Segment%20using%20EXTI%20Button.png" width="600">
</p>

### 
### Discussion

1. We can use two different methods to detect an external signal: polling and interrupt. What are the advantages and disadvantages of each approach?

   >**Polling** is a method where the CPU repeatedly checks the input pin to detect a signal change.  
     The advantage is that it is simple to implement and easy to debug.  
     However, the disadvantage is that the CPU wastes time constantly checking the input,  
     and it may **miss short signals** while performing other tasks.
     **Interrupt-based detection (EXTI)** allows the CPU to respond only when an external event occurs.  
     Its advantages include better CPU efficiency and immediate response to events.  
     The drawbacks are that configuration is more complex, and **priority management** is required when multiple interrupts occur simultaneously.



2. What would happen if the EXTI interrupt handler does not clear the interrupt pending flag? Check with your code

   >If the interrupt pending flag is **not cleared** inside **EXTI4_IRQHandler()**,  
     the pending bit remains set (1), so the interrupt handler will be called repeatedly.  
     As a result, even a single button press will cause the ISR to **retrigger infinitely**,  
     and the system will not return to the main loop.
     In practice, commenting out **clear_pending_EXTI(PA_4)** leads to continuous flashing  
     on the LED or 7-segment display because the EXTI controller believes the interrupt is still active.



### Code 
**ecEXTI2.c** : 
 https://github.com/henny041520-commits/EC-DJLee-042/blob/main/include/ecEXTI2.c
 
**LAB_LED EXTI & SysTick_1.c : 
https://github.com/henny041520-commits/EC-DJLee-042/blob/main/lab/LAB_EXTI/LAB_LED%20EXTI%20%26%20SysTick_1.c

**ecEXTI2.c**
```c
#include "ecGPIO2.h"      // GPIO helpers (PinName_t mapping, init/read/write, etc.)
#include "ecSysTick2.h"   // (Not required for EXTI itself; included in your original code)
#include "ecEXTI2.h"      // EXTI function prototypes (this file's declarations)
#include "stm32f411xe.h"  // Device header (register definitions)
//------------------------------------------------------------------------------
// EXTI_init
// - Connect a given GPIO pin to its EXTI line via SYSCFG->EXTICR
// - Configure edge trigger (RTSR/FTSR)
// - Unmask interrupt (IMR)
// - Select NVIC IRQ (EXTI0..4, EXTI9_5, EXTI15_10), set priority, enable
// Parameters:
//   pinName   : logical pin (e.g., PA_4)
//   trig_type : FALL / RISE / BOTH
//   priority  : NVIC priority value (lower number = higher priority)
//------------------------------------------------------------------------------
void EXTI_init(PinName_t pinName, int trig_type,int priority){
	GPIO_TypeDef *port;   // GPIO port pointer (GPIOA, GPIOB, ...)
	unsigned int pin;     // Pin number (0..15)
	ecPinmap(pinName,&port,&pin);  // Map PinName_t to (port, pin)
	// Enable SYSCFG peripheral clock (EXTI routing is configured via SYSCFG)
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;		
	// Determine EXTICR port code:
	//   0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD, else:4 (e.g., GPIOH or fallback)
	int EXTICR_port;
	if		(port == GPIOA) EXTICR_port = 0;
	else if	(port == GPIOB) EXTICR_port = 1;
	else if	(port == GPIOC) EXTICR_port = 2;
	else if	(port == GPIOD) EXTICR_port = 3;
	else 		EXTICR_port = 4;
	// Route the selected GPIO to its EXTI line:
	// - EXTICR index = pin/4 (0: pins 0..3, 1: 4..7, 2: 8..11, 3: 12..15)
	// - Within EXTICR[idx], each pin uses a 4-bit field, shifted by (pin%4)*4
	SYSCFG->EXTICR[pin/4] &= ~(15<<((pin % 4) * 4));           // clear previous 4 bits
	SYSCFG->EXTICR[pin/4] |= EXTICR_port<<((pin % 4) * 4);     // write new port code
	// Configure trigger edge(s):
	//  - FTSR: falling-edge enable
	//  - RTSR: rising-edge enable
	if (trig_type == FALL) EXTI->FTSR |= 1 << pin;      // Falling trigger enable 
	else if	(trig_type == RISE) EXTI->RTSR |= 1<<pin ;  // Rising trigger enable 
	else if	(trig_type == BOTH) {                       // Both edges
		EXTI->RTSR |= 1 << pin; 
		EXTI->FTSR |= 1<<pin;
	} 
	// Unmask EXTI line in IMR (i.e., enable interrupt generation for this line)
	EXTI->IMR  |= 1<<pin;     // not masked
	// NVIC (IRQ) selection based on pin number:
	// - Pins 0..4  : dedicated IRQs EXTI0_IRQn .. EXTI4_IRQn
	// - Pins 5..9  : shared IRQ EXTI9_5_IRQn
	// - Pins 10..15: shared IRQ EXTI15_10_IRQn
	int EXTI_IRQn = 0;
	if (pin < 5) 	EXTI_IRQn = EXTI0_IRQn + pin;
	else if	(pin < 10) 	EXTI_IRQn = EXTI9_5_IRQn;
	else 			EXTI_IRQn = EXTI15_10_IRQn;
	// Set NVIC priority and enable the selected EXTI IRQ
 	NVIC_SetPriority(EXTI_IRQn, 0);	 // EXTI priority (NOTE: using fixed 0 per         your code)
	NVIC_EnableIRQ(EXTI_IRQn); 	 // Enable EXTI IRQ in NVIC
}

//------------------------------------------------------------------------------
// EXTI_enable
// - Unmask the EXTI line in IMR for the given pin
//------------------------------------------------------------------------------
void EXTI_enable(PinName_t pinName) {
	GPIO_TypeDef *port;
	unsigned int pin;
	ecPinmap(pinName,&port,&pin);
	EXTI->IMR |= 1 << pin;     // not masked (i.e., Interrupt enabled)
}
//------------------------------------------------------------------------------
// EXTI_disable
// - Mask the EXTI line in IMR for the given pin
//------------------------------------------------------------------------------
void EXTI_disable(PinName_t pinName) {
	GPIO_TypeDef *port;
	unsigned int pin;
	ecPinmap(pinName,&port,&pin);
	EXTI->IMR &= ~(1<< pin);   // masked (i.e., Interrupt disabled)
}
//------------------------------------------------------------------------------
// is_pending_EXTI
// - Check if the EXTI pending bit is set for the given pin
// - Returns non-zero if pending, 0 otherwise
//------------------------------------------------------------------------------
uint32_t is_pending_EXTI(PinName_t pinName) {
	GPIO_TypeDef *port;
	unsigned int pin;
	ecPinmap(pinName,&port,&pin);
	uint32_t EXTI_PRx = EXTI->PR;     	// Read EXTI pending register (optional                                                                        local copy)
	return ((EXTI->PR & (1UL << pin)) == (1UL << pin));
}
//------------------------------------------------------------------------------
// clear_pending_EXTI
// - Clear the EXTI pending bit for the given pin
// - NOTE: EXTI->PR uses write-1-to-clear semantics
//------------------------------------------------------------------------------
void clear_pending_EXTI(PinName_t pinName) {
	GPIO_TypeDef *port;
	unsigned int pin;
	ecPinmap(pinName,&port,&pin);
	EXTI->PR |= 1<<pin;     // clear EXTI pending 
}
```


**LAB_LED EXTI & SysTick_1.c
```c
#include "ecSTM32F4v2.h"
#define BUTTON_PIN   PA_4               // External button on PA4 (B1, pull-up assumed)
volatile uint8_t selectFND  = 0;        // Digit index for the tens place  (DIG0)
volatile uint8_t selectFND2 = 1;        // Digit index for the ones place  (DIG1)
volatile uint8_t count      = 0;        // 0..19 counter incremented by EXTI (ISR)
//---------------------------------------------------------
// System setup:
// 1) Initialize clocks (HSI, then PLL to 84 MHz per library default)
// 2) Initialize SysTick for millisecond delay
// 3) Initialize 7-segment (GPIO directions, etc.)
// 4) Initialize EXTI on BUTTON_PIN: falling-edge, NVIC prio=2
// 5) Show an initial digit (0) on tens place
//---------------------------------------------------------
void setup(void){
    RCC_HSI_init();                     // Switch to internal HSI (16 MHz) first
    RCC_PLL_init();                     // Then configure/enable PLL (e.g., 84 MHz)
    SysTick_init();                     // 1 ms system tick (used by delay_ms)
    seven_seg_FND_init(BUTTON_PIN );    // 7-seg GPIOs + button pin basic config
    EXTI_init(BUTTON_PIN, FALL, 2);     // Falling edge on PA4 → EXTI4, NVIC priority 2
    seven_seg_FND_display(0, selectFND);// Draw initial '0' on the tens digit
}
int main(void){
    setup();
    // Main loop: time-multiplex 2 digits (tens, ones) with short on-times (1 ms each).
    // This yields a ~500 Hz refresh for each digit (assuming minimal overhead),
    // which appears steady and flicker-free to the eye.
    //It is not possible to light up multiple digits independently at the same           time.  Instead, the two FND digits are alternately turned on and off very          quickly.  Although in reality only one digit is lit at a time, the switching       is so fast that it creates a visual illusion, making them appear to be             illuminated simultaneously.
    while(1){
        // --- Tens digit (count/10) ---
        seven_seg_FND_display(count/10, selectFND);  // Show tens digit
        delay_ms(1);                                 // Short ON time (persistence)
	    //When the delay was set to 5ms, no flickering was visible to the                    naked eye, but flickering became noticeable when recorded with a camera.
        sevenseg_clear(selectFND);                   // Blank the tens digit
        // --- Ones digit (count%10) ---
        seven_seg_FND_display(count%10, selectFND2); // Show ones digit
        delay_ms(1);                                 // Short ON time (persistence)
        sevenseg_clear(selectFND2);                  // Blank the ones digit
    }
}
// --- EXTI interrupt handler for PA4 (wired to EXTI4) ---
// On each falling edge (button press with pull-up), increment the 0..19 counter.
// Wrap back to 0 after 19 → (i.e., when it reaches 20).
void EXTI4_IRQHandler(void){
    if (is_pending_EXTI(BUTTON_PIN)) {  // Check EXTI pending bit for PA4
        count++;                        // Increment the counter (ISR context)
        if (count == 20) {              // Wrap at 20 → back to 0
            count = 0;
        }
    }
    clear_pending_EXTI(BUTTON_PIN);     // write-1-to-clear EXTI pending
}
```

### Results

## LAB_EXTI & SysTick — Image Table (Full Sequence)

| No | State | Press | Release |
|----|--------|--------|----------|
| **00 start** | No Input | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/00start%20no%20input.jpg" width="200"> | — |
| **00 start** | Input Sequence | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/00start%20press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/00start%20release.jpg" width="200"> |
| **01** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/01press%20.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/01release.jpg" width="200"> |
| **02** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/02press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/02release.jpg" width="200"> |
| **03** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/03press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/03release.jpg" width="200"> |
| **04** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/04press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/04release.jpg" width="200"> |
| **05** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/05press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/05release.jpg" width="200"> |
| **06** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/06press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/06release.jpg" width="200"> |
| **07** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/07press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/07release.jpg" width="200"> |
| **08** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/08press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/08release.jpg" width="200"> |
| **09** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/09press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/09release.jpg" width="200"> |
| **10** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/10press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/10release.jpg" width="200"> |
| **11** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/11press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/11release.jpg" width="200"> |
| **12** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/12press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/12release.jpg" width="200"> |
| **13** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/13press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/13release.jpg" width="200"> |
| **14** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/14press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/14release.jpg" width="200"> |
| **15** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/15press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/15release.jpg" width="200"> |
| **16** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/16press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/16release.jpg" width="200"> |
| **17** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/17press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/17release.jpg" width="200"> |
| **18** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/18press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/18release.jpg" width="200"> |
| **19 end** | Count Step | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/19end%20press.jpg" width="200"> | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20exti/19end%20release.jpg" width="200"> |

**Youtube link : https://youtube.com/shorts/9lOCJE7rf2k?feature=share

## Problem 2: Counting numbers on 7-Segment using SysTick

Display the number 0 to 9 on the 7-segment LED at the rate of 1 sec.

After displaying up to 9, then it should display ‘0’ and continue counting.

When the button is pressed, the number should be reset ‘0’ and start counting again.

### SysTick Library&#x20;

1. [Download sample header files](https://github.com/ykkimhgu/EC-student/tree/main/include/lib-student): **ecSysTick\_student.h, ecSysTick\_student.c**
2. Rename these files as **ecSysTick2.h, ecSysTick2.c**
   * You MUST write your name and other information at the top of the library code files.
   * Save these files in your directory `EC \include\`.
3. Declare and define the following functions in your library : **ecSysTick2.h**

**ecSysTick.h**
```c
void SysTick_init(uint32_t msec);
void delay_ms(uint32_t msec);
uint32_t SysTick_val(void);
void SysTick_reset (void);
void SysTick_enable(void);
void SysTick_disable (void)
```

### 2-2. Procedure

1. Create a new project under the directory

   `\EC\lab\LAB_EXTI_SysTick`

* The project name is “**LAB\_EXTI\_SysTick”.**
* Create a new source file named as “**LAB\_EXTI\_SysTick.c”**

> You MUST write your name on the source file inside the comment section.

2\. Include your updated library in `\EC\include\` to your project.

* **ecGPIO2.h, ecGPIO2.c**
* **ecRCC2.h, ecRCC2.c**
* **ecEXTI2.h, ecEXTI2.c**
* **ecSysTick2.h, ecSysTick2.c**

3. First, check if every number, 0 to 9, can be displayed properly on the 7-segment.
4. Then, create a code to display the number counting from 0 to 9 and repeat at the rate of 1 second. (Use only one digit)
5. When the button is pressed, it should start from '0' again.

   > Use EXTI for this button reset.

### Configuration

| Digital In for Button (B1) | Digital Out for FND-7-Segment                                                                        |
| -------------------------- | ---------------------------------------------------------------------------------------------------- |
| Digital In                 | Digital Out                                                                                          |
| PA4                        | <p>PB7,PB6,PB5,PB4,PB3,PB2,PB1,PB0<br>('a'\~'h', respectively)<br>PA10<br>('LED4', respectively)</p> |
| PULL-UP                    | Push-Pull, No Pull-up-Pull-down, Medium Speed                                                        |
## Algorithm

### Overview
#### Mealy FSM Table — Counting numbers on 7-Segment using SysTick
## FSM Table — Digit 7-Segment Counter (Moore)

| Current State  | Input / Condition    | Next State     | Outputs (SEG on DIG0)       | Actions                                                        |
| -------------- | -------------------- | -------------- | --------------------------- | -------------------------------------------------------------- |
| **S0_INIT**    | —                    | **S1_DELAY**   | `SEG = pattern(count % 10)` | Initialize system (HSI→PLL, SysTick, FND). Show initial digit. |
| **S1_DELAY**   | `delay_done_1s == 1` | **S2_UPDATE**  | (unchanged)                 | Call `delay_ms(1000)`; when done, raise `delay_done_1s`.       |
| **S2_UPDATE**  | —                    | **S3_DISPLAY** | (unchanged)                 | `count = (count + 1) % 10`                                     |
| **S3_DISPLAY** | —                    | **S1_DELAY**   | `SEG = pattern(count % 10)` | `seven_seg_FND_display(count % 10, DIG0)`                      |

**Notes**
- This program uses a single digit (e.g., `selectFND = 0`) and increments once per second.
- `delay_done_1s` represents the completion of `delay_ms(1000)` (implemented by SysTick).
- Moore form: outputs depend on state (and the stored `count`), not on the instantaneous input.
### Flowchart 
<p align="center">
  <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/Flow_Chart_Counting%20numbers%20on%207-Segment%20using%20SysTick.png" width="600">
</p>

### Code

**ecSysTick2.c** : 
 https://github.com/henny041520-commits/EC-DJLee-042/blob/main/include/ecSysTick2.c
 
**LAB_LED EXTI & SysTick_2.c : 
https://github.com/henny041520-commits/EC-DJLee-042/blob/main/lab/LAB_EXTI/LAB_LED%20EXTI%20%26%20SysTick_2.c

**ecSysTick2.c**
```c
#include "ecSysTick2.h"
//------------------------------------------------------------------------------
// System clock definitions (used to compute 1 ms tick period)
#define MCU_CLK_PLL 84000000   // System clock when PLL is used (84 MHz)
#define MCU_CLK_HSI 16000000   // Internal HSI clock (16 MHz)
// Global millisecond tick counter (incremented in SysTick ISR)
volatile uint32_t msTicks;
//------------------------------------------------------------------------------
// SysTick_init
// - Configure Cortex-M SysTick to generate an interrupt every 1 ms
// - Uses processor clock as SysTick source and enables NVIC entry
//------------------------------------------------------------------------------
void SysTick_init(void){	
	// SysTick Control and Status Register: start from a known (disabled) state
	SysTick->CTRL = 0;                                       // Disable SysTick                                                                    IRQ and counter
	// Select SysTick clock source:
	// CLKSOURCE = 1 → processor clock (MCU clock)
	// CLKSOURCE = 0 → external reference clock
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	// SysTick Reload Value Register:
	// For 1 ms period at 84 MHz: LOAD = 84,000 - 1 = 83,999
	// (Your code targets PLL=84 MHz; adjust if you run at a different clock.)
	SysTick->LOAD = MCU_CLK_PLL / 1000 - 1;                  // 1 ms at 84 MHz
	// Clear current counter value so counting restarts from LOAD
	SysTick->VAL = 0;
	// Enable SysTick exception request (interrupt on count-to-zero)
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	// Enable SysTick timer (starts counting down)
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	// NVIC configuration for SysTick
	// (Priority value as given; lower numbers mean higher priority on Cortex-M.)
	NVIC_SetPriority(SysTick_IRQn, 16);      // Set priority (value per your code)
	NVIC_EnableIRQ(SysTick_IRQn);            // Enable SysTick interrupt in NVIC
}
//------------------------------------------------------------------------------
// SysTick_Handler
// - Invoked every 1 ms when SysTick counts down to zero
// - Delegates to SysTick_counter() to keep ISR body minimal
//------------------------------------------------------------------------------
void SysTick_Handler(void){
	SysTick_counter();	
}
//------------------------------------------------------------------------------
// SysTick_counter
// - ISR-side counter update: increments global millisecond tick
//------------------------------------------------------------------------------
void SysTick_counter(){
	msTicks++;
}	
//------------------------------------------------------------------------------
// delay_ms
// - Busy-wait (blocking) delay using the msTicks software timer
// - Captures the current tick and spins until the requested interval elapses
//   NOTE: This consumes CPU cycles while waiting (non-preemptive delay).
//------------------------------------------------------------------------------
void delay_ms (uint32_t mesc){
  uint32_t curTicks;

  curTicks = msTicks;                           // Snapshot current tick
  while ((msTicks - curTicks) < mesc);          // Wait until elapsed >= mesc
  msTicks = 0;                                  // Reset global tick (per your design)
}
//------------------------------------------------------------------------------
// Alternative delay implementation using SysTick->VAL (commented out)
// - Less common; relies on reading the hardware down-counter value
// - Kept here exactly as in your source
//------------------------------------------------------------------------------
//void delay_ms(uint32_t msec){
//	uint32_t now=SysTick_val(); 
//	if (msec>5000) msec=5000;
//	if (msec<1) msec=1;
//	while ((now - SysTick_val()) < msec);
//}
//------------------------------------------------------------------------------
// SysTick_reset
// - Manually clear the current SysTick counter value (VAL)
//------------------------------------------------------------------------------
void SysTick_reset(void)
{
	// SysTick Current Value Register
	SysTick->VAL = 0;
}
//------------------------------------------------------------------------------
// SysTick_val
// - Read the current SysTick counter (hardware down-counter)
//------------------------------------------------------------------------------
uint32_t SysTick_val(void) {
	return SysTick->VAL;
}
//------------------------------------------------------------------------------
// Alternative counter logic example (commented out)
// - Shows how to derive 1-second events from msTicks inside the ISR
//------------------------------------------------------------------------------
//void SysTick_counter(){
//	msTicks++;
//	if(msTicks%1000 == 0) count++;
//}	
```

**LAB_LED EXTI & SysTick_2 

```c
#include "ecSTM32F4v2.h"
//---------------------------------------------------------
// setup()
// - Initialize system clocks (HSI, then PLL for 84 MHz system)
// - Initialize SysTick timer for 1 ms tick (used by delay_ms)
// - Initialize 7-segment display GPIOs
//---------------------------------------------------------
void setup(void){
    RCC_HSI_init();                 // Enable internal HSI oscillator (16 MHz)
    RCC_PLL_init();                 // Switch system clock to PLL (84 MHz)
    SysTick_init();                 // Configure SysTick for 1 ms interrupts
    seven_seg_FND_init(PA_4);       // Initialize FND display GPIOs, button on PA4
}
//---------------------------------------------------------
// main()
// - Simple 1-digit counter displayed on FND
// - Increments every 1 second (1000 ms)
// - Displays count modulo 10 (0..9) on digit 0
//---------------------------------------------------------
int main(void) {
    setup();
    uint8_t selectFND = 0;          // Use first digit (DIG0)
    uint8_t count = 0;              // Counter variable
    while (1) {
        delay_ms(1000);             // Wait for 1 second
        seven_seg_FND_display(count % 10, selectFND); // Show last digit only                                                    ex(11 -> 1 12 -> 2 ..... 99 -> 9)
        count++;                    // Increment counter
    }
}
```
### Results

## LAB_SysTick — Image Table (Counting Numbers on 7-Segment)

| Step | Image | Description |
|------|--------|-------------|
| **(0)** | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20systick/(0)start.jpg" width="220"> | Start — system initialized |
| **(1)** | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20systick/(1)0.jpg" width="220"> | Displaying number 0 |
| **(2)** | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20systick/(2)1.jpg" width="220"> | Displaying number 1 |
| **(3)** | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20systick/(3)2.jpg" width="220"> | Displaying number 2 |
| **(4)** | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20systick/(4)3.jpg" width="220"> | Displaying number 3 |
| **(5)** | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20systick/(5)4.jpg" width="220"> | Displaying number 4 |
| **(6)** | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20systick/(6)5.jpg" width="220"> | Displaying number 5 |
| **(7)** | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20systick/(7)6.jpg" width="220"> | Displaying number 6 |
| **(8)** | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20systick/(8)7.jpg" width="220"> | Displaying number 7 |
| **(9)** | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20systick/(9)8.jpg" width="220"> | Displaying number 8 |
| **(10)** | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20systick/(10)9.jpg" width="220"> | Displaying number 9 |
| **(11)** | <img src="https://raw.githubusercontent.com/henny041520-commits/EC-DJLee-042/main/report/img/LaB_EXTI%20%26%20SysTick/lab%20systick/(11)0.jpg" width="220"> | Loop restarts — back to 0 |


**Youtube link : https://youtube.com/shorts/7guBtK5udzo?feature=share

## Reference

Complete list of all references used (github, blog, paper, etc)

```
친절한 임베디드 시스템 개발자 되기 강좌 :
https://recipes.tistory.com/
```


