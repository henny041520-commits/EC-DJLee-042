/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : SSS LAB
Created          : 05-03-2021
Modified         : 08-23-2024
Language/ver     : C++ in Keil uVision

Description      : Distributed to Students for LAB_GPIO
/----------------------------------------------------------------*/



#include "stm32f4xx.h"
#include "stm32f411xe.h"
#include "ecGPIO2.h"
PinName_t svgpinsFND[12]={PB_7, PB_6, PB_5, PB_4, PB_3, PB_2, PB_1, PB_0, PC_3, PC_4, PA_11, PA_10};
PinName_t svgpinsnum[8]={PB_7, PB_6, PB_5, PB_4, PB_3, PB_2, PB_1, PB_0};
PinName_t svgpinsSelect[4]={ PC_3, PC_4, PA_11, PA_10};
 uint8_t svgvalue[10] = {
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00100111,
    0b01111111,
    0b01101111,
    };

void GPIO_init(PinName_t pinName, uint32_t mode){     
	GPIO_TypeDef * Port;
	unsigned int pin;
	ecPinmap(pinName, &Port, &pin);
	
	// mode  : Input(0), Output(1), AlterFunc(2), Analog(3)   
	if (Port == GPIOA)
		RCC_GPIOA_enable();
	if (Port == GPIOB)
		RCC_GPIOB_enable();
	if (Port == GPIOC)
		RCC_GPIOC_enable();
	if (Port == GPIOD)
		RCC_GPIOD_enable();
    if (Port == GPIOH)
		RCC_GPIOH_enable();
	//[TO-DO] YOUR CODE GOES HERE
	// Make it for GPIOB, GPIOD..GPIOH

	// You can also make a more general function of
	// void RCC_GPIO_enable(GPIO_TypeDef *Port); 

	GPIO_mode(pinName, mode);
}


// GPIO Mode          : Input(00), Output(01), AlterFunc(10), Analog(11)
void GPIO_mode(PinName_t pinName, uint32_t mode){
   GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
   Port->MODER &= ~(3UL<<(2*pin));     
   Port->MODER |= mode<<(2*pin);    
}


// GPIO Speed          : Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
void GPIO_ospeed(PinName_t pinName, int speed){
    GPIO_TypeDef * Port;
    unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	Port->OSPEEDR &=~(3UL<<(2*pin));
	Port->OSPEEDR |=speed<<(2*pin);
}

// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
void GPIO_otype(PinName_t pinName, int type){
   GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
   Port->OTYPER &= ~(1UL<<(pin));
   Port->OTYPER |= type <<(pin);
}

// GPIO Push-Pull    : No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
void GPIO_pupd(PinName_t pinName, int pupd){
   GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
   Port -> PUPDR &= ~(3UL<<(2*pin));
   Port -> PUPDR |= pupd<<(2*pin);
}

int GPIO_read(PinName_t pinName){
   GPIO_TypeDef * Port;
   unsigned int pin;
   unsigned int btval;
   ecPinmap(pinName,&Port,&pin);

   btval = (Port -> IDR) & (1UL << pin);
	return(btval);	
}

 void GPIO_write(PinName_t pinName, int state){
   GPIO_TypeDef * Port;
   unsigned int pin;

   ecPinmap(pinName,&Port,&pin);
   if(state == 1)
   {
   (Port -> ODR) &= ~(1UL << pin);
   (Port -> ODR) |= (1UL << pin);
   }
   else
   {
   (Port -> ODR) &= ~(1UL << pin);
   }

}
void seven_seg_FND_display(uint8_t  num, uint8_t select){
    GPIO_write(svgpinsSelect[select], 1); 
    for (int i=0; i<8;i++){
         GPIO_write(svgpinsnum[7-i], (svgvalue[num]>>i )& 0x01); 
    }
}
void seven_seg_FND_init(PinName_t pinName){	
    for(int i=0;i<8;i++)
    {
    GPIO_init(svgpinsnum[i], OUTPUT); 
    GPIO_otype(svgpinsnum[i], pushpull);
    GPIO_ospeed(svgpinsnum[i],mediumspeed);
    GPIO_pupd(svgpinsnum[i],nopupd);
    }
     for(int i=0;i<4;i++)
    {
    GPIO_init(svgpinsSelect[i], OUTPUT); 
    GPIO_otype(svgpinsSelect[i], pushpull);
    GPIO_ospeed(svgpinsSelect[i],mediumspeed);
    GPIO_pupd(svgpinsSelect[i],nopupd);
    }
    GPIO_pupd(pinName,pullup);
    GPIO_init(pinName, INPUT);
}
void delay(void){	
        for(volatile int i=0;i<160000;i++); 
}
int readButtonRising(PinName_t pinName){
    static int prev = 1;                 
    int now = GPIO_read(pinName) ? 1 : 0;
    int rising = 0;
    if(prev==0 && now==1){
        for(volatile int i=0;i<160000;i++); 
        rising = 1;
    }
    prev = now;
    return rising;
}
void sevenseg_clear(uint8_t  num)
{GPIO_write(svgpinsSelect[num], 0); }

