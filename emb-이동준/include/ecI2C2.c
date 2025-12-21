// ecI2C2.c
#include "ecI2C2.h"

// 내부 헬퍼: 간단한 타임아웃 카운터
static void I2C1_waitFlag(volatile uint32_t *reg, uint32_t mask){
    volatile uint32_t timeout = 1000000;
    while(((*reg) & mask) == 0){
        if(--timeout == 0) break;           // 무한루프 방지
    }
}

void I2C1_init(void){
    // 1. GPIOB 클록 활성화
    RCC_GPIOB_enable();                     // GPIOB AHB1ENR 비트 세트
    
    // 2. PB_8 (SCL), PB_9 (SDA) AF 모드 설정
    GPIO_init(PB_8, AF);                    // MODER = 10
    GPIO_init(PB_9, AF);
    GPIO_otype(PB_8, opendrain);            // 오픈드레인 출력  I2C1 SCL
    GPIO_otype(PB_9, opendrain);            // 오픈드레인 출력  I2C1 SDA
    GPIO_pupd(PB_8, pullup);                // 풀업 사용       (외부 풀업 권장)
    GPIO_pupd(PB_9, pullup);
    GPIO_ospeed(PB_8, highspeed);           // 고속 모드        (I2C 신호 에지)
    GPIO_ospeed(PB_9, highspeed);
    
    // AF4 선택 (I2C1)  AFR[1]에서 PIN 8,9 설정
    GPIOB->AFR[1] &= ~((0xF << ((8-8)*4)) | (0xF << ((9-8)*4)));
    GPIOB->AFR[1] |=  (0x4 << ((8-8)*4)) | (0x4 << ((9-8)*4)); // AF4 = I2C1
    
    // 3. I2C1 클록 활성화
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;     // APB1 I2C1 클록
    
    // 4. I2C1 비활성화 후 설정
    I2C1->CR1 &= ~I2C_CR1_PE;               // I2C1 비활성화
    
    // APB1 = 42.00 MHz 를 기준으로 설정 (표준 모드 100.00 kHz)
    // CR2[5:0] = PCLK1 주파수(MHz)
    I2C1->CR2 &= ~I2C_CR2_FREQ;
    I2C1->CR2 |= 42U;                       // 42.00 MHz
    
    // 표준모드: SCL = PCLK1 / (2 * CCR)
    // CCR = 42MHz / (2 * 100kHz) = 210
    I2C1->CCR = 0;
    I2C1->CCR |= 210U;                      // CCR = 210 → 약 100.00 kHz
    
    // TRISE = Fpclk(MHz) + 1 = 43
    I2C1->TRISE = 43U;
    
    // 5. I2C1 활성화
    I2C1->CR1 |= I2C_CR1_PE;
}

void I2C1_start(void){
    // BUSY 클리어 대기 (필요시)
    // while(I2C1->SR2 & I2C_SR2_BUSY);
    
    // START 비트 세트
    I2C1->CR1 |= I2C_CR1_START;            // START 조건 생성
    // SB(Start Bit) 세트 대기 (SR1)
    I2C1_waitFlag(&(I2C1->SR1), I2C_SR1_SB);
}

void I2C1_stop(void){
    // STOP 비트 세트 (전송 종료)
    I2C1->CR1 |= I2C_CR1_STOP;
}

void I2C1_writeByte(uint8_t data){
    // DR 에 데이터 기록
    I2C1->DR = data;
    // TXE(데이터 레지스터 비어 있음) 또는 BTF(바이트 전송 완료) 대기
    I2C1_waitFlag(&(I2C1->SR1), I2C_SR1_TXE);
}

void I2C1_writeTo(uint8_t addr7, const uint8_t *buf, int n){
    volatile uint32_t tmp;
    
    // 1) START 생성
    I2C1_start();
    
    // 2) 주소 전송 (7비트 주소 + W=0)
    I2C1->DR = (addr7 << 1);               // R/W=0 → Write
    // ADDR 플래그 대기
    I2C1_waitFlag(&(I2C1->SR1), I2C_SR1_ADDR);
    // ADDR 클리어: SR1 읽고 SR2 읽기
    tmp = I2C1->SR1;
    (void)tmp;
    tmp = I2C1->SR2;
    (void)tmp;
    
    // 3) 데이터 바이트 연속 전송
    for(int i = 0; i < n; i++){
        I2C1_writeByte(buf[i]);
    }
    
    // 마지막 바이트 BTF 대기 (선택적)
    I2C1_waitFlag(&(I2C1->SR1), I2C_SR1_BTF);
    
    // 4) STOP 생성
    I2C1_stop();
}
