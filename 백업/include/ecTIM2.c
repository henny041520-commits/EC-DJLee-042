/**
******************************************************************************
* @author  SSSLAB
* @Mod	   2021-8-12 by YKKIM  	
* @brief   Embedded Controller:  EC_HAL_for_student_exercise 
* 
******************************************************************************
*/


#include "ecTIM2.h"
#include "ecGPIO2.h"

/* Timer Configuration */

// Default Setting:  1 msec of TimerUEV with Counter_Clk 100kHz / PSC=840-1, ARR=100-1
void TIM_init(TIM_TypeDef* TIMx, uint32_t msec){     
    // Previous version:  void TIM_init(TIM_TypeDef* TIMx, uint32_t msec) 	
    // 1. Enable Timer CLOCK
	if(TIMx ==TIM1) RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	else if(TIMx ==TIM2) RCC->APB1ENR |= 1<<0;
	else if(TIMx ==TIM3) RCC->APB1ENR |= 1<<1;
	// repeat for TIM4, TIM5, TIM9, TIM11
	else if(TIMx ==TIM4) RCC->APB1ENR |= 1<<2;
	else if(TIMx ==TIM5) RCC->APB1ENR |= 1<<3;
    else if(TIMx ==TIM9) RCC->APB2ENR |= 1<<16;
	else if(TIMx ==TIM11) RCC->APB2ENR |= 1<<18;
	
    // 2. Set CNT period
	//  uint32_t msec=1;
	TIM_period_ms(TIMx, msec); 
	
	
    // 3. CNT Direction
	TIMx->CR1 &=~(1<<4);					// Upcounter	
	
    // 4. Enable Timer Counter
	TIMx->CR1 |= TIM_CR1_CEN;		
}

// Timer Update Event Period  1~6000 usec  with 1MHz Couter / ARR=1* usec
void TIM_period_us(TIM_TypeDef *TIMx, uint32_t usec){   

	uint16_t PSCval;
	uint32_t Sys_CLK;

	
	if((RCC->CFGR & RCC_CFGR_SW_PLL) == RCC_CFGR_SW_PLL)
		Sys_CLK = 84000000;
	
	else if((RCC->CFGR & RCC_CFGR_SW_HSI) == RCC_CFGR_SW_HSI) 
		Sys_CLK = 16000000;
	
	
	if (TIMx == TIM2 || TIMx == TIM5){
		uint32_t ARRval;
		
		PSCval = Sys_CLK/1000000;						// 84 or 16	--> f_cnt = 1MHz
		ARRval = Sys_CLK/PSCval/1000000 * usec;						// ARRval= 1*usec
		TIMx->PSC = PSCval - 1;
		TIMx->ARR = ARRval - 1;				
	}
	else{
		uint16_t ARRval;

		PSCval = Sys_CLK/1000000;						// 84 or 16	--> f_cnt = 1MHz
		ARRval = Sys_CLK/PSCval/1000000 * usec;						// ARRval= 1*usec
		TIMx->PSC = PSCval - 1;
		TIMx->ARR = ARRval - 1;
	}			
}


// Timer Update Event Period  1~600 msec  with 100kHz Couter / ARR=100*msec
void TIM_period_ms(TIM_TypeDef* TIMx, uint32_t msec){ 
	
	
	
	uint16_t PSCval;
	uint32_t Sys_CLK;
	
	if((RCC->CFGR & RCC_CFGR_SW_PLL) == RCC_CFGR_SW_PLL )
		 Sys_CLK = 84000000;
	
	else if((RCC->CFGR & RCC_CFGR_SW_HSI) == RCC_CFGR_SW_HSI) 
		Sys_CLK = 16000000;
	
	
	if (TIMx == TIM2 || TIMx == TIM5){
		uint32_t ARRval;		
		PSCval = Sys_CLK/100000;		// 840 or 160	--> PSC_clk=f_cnt = 100kHz
		ARRval = (Sys_CLK/PSCval/1000) * msec;		// 100kHz*msec,  ARRval=100 for 1msec
		TIMx->PSC = PSCval - 1;
		TIMx->ARR = ARRval - 1;
	}
	else{
		uint16_t ARRval;

		      PSCval = Sys_CLK / 1000;                   // ★ f_cnt = 1 kHz
        ARRval = (uint16_t)((Sys_CLK / PSCval / 1000) * msec); // = 1 * msec			
		TIMx->PSC = PSCval - 1;
		TIMx->ARR = ARRval - 1;
	}
}

// msec = 1 to 655
void TIM_period(TIM_TypeDef* TIMx, uint32_t msec){
	TIM_period_ms(TIMx, msec);
}



// Update Event Interrupt
void TIM_UI_init(TIM_TypeDef* TIMx, uint32_t msec){
    // 1. Initialize Timer	
	TIM_init(TIMx,1);
	 
    // 2. Enable Update Interrupt
	TIM_UI_enable(TIMx);
	TIM_period_ms(TIMx, msec);
    // 3. NVIC Setting
	uint32_t IRQn_reg =0;
	if(TIMx == TIM1)       IRQn_reg = TIM1_UP_TIM10_IRQn;
	else if(TIMx == TIM2)  IRQn_reg = TIM2_IRQn;//< TIM2 global Interrupt                                             */
	// repeat for TIM3, TIM4, TIM5, TIM9, TIM10, TIM11
    else if(TIMx == TIM3)  IRQn_reg =  TIM3_IRQn; //< TIM3 global Interrupt
	else if(TIMx == TIM4)  IRQn_reg = TIM4_IRQn;  //< TIM4 global Interrupt
	else if(TIMx == TIM5)  IRQn_reg = TIM5_IRQn; //< TIM4 global Interrupt
    else if(TIMx == TIM9)  IRQn_reg = TIM1_BRK_TIM9_IRQn;
    else if(TIMx == TIM10)  IRQn_reg = TIM1_UP_TIM10_IRQn;
    else if(TIMx == TIM11)  IRQn_reg = TIM1_TRG_COM_TIM11_IRQn;
	NVIC_EnableIRQ(IRQn_reg);				
	NVIC_SetPriority(IRQn_reg,2);
}



void TIM_UI_enable(TIM_TypeDef* TIMx){
	TIMx->DIER |= 1<<0;			// Enable Timer Update Interrupt		
}

void TIM_UI_disable(TIM_TypeDef* TIMx){
	TIMx->DIER &= ~(1<<0);				// Disable Timer Update Interrupt		
}

uint32_t is_UIF(TIM_TypeDef *TIMx){
	return (TIMx->SR & 1);
}

void clear_UIF(TIM_TypeDef *TIMx){
	TIMx->SR &= ~1;
}
