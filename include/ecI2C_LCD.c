// ecI2C_LCD.c  (I2C3 기반 구현, 핵심 부분)
#include "ecI2C_LCD.h"

#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_RW        0x02
#define LCD_RS        0x01

static void I2C3_writeByte(uint8_t data);
static void LCD1602_write4(uint8_t data);
static void LCD1602_send(uint8_t value, uint8_t mode);  // mode=0:cmd, 1:data

void I2C3_init_LCD(PinName_t scl, PinName_t sda){
    GPIO_TypeDef *portSCL, *portSDA;
    unsigned int pinSCL, pinSDA;

    ecPinmap(scl, &portSCL, &pinSCL);
    ecPinmap(sda, &portSDA, &pinSDA);

    // GPIO AF 설정 (AF4: I2C3)
    GPIO_init(scl, AF);
    GPIO_otype(scl, opendrain);
    GPIO_pupd(scl, pullup);
    GPIO_ospeed(scl, highspeed);

    GPIO_init(sda, AF);
    GPIO_otype(sda, opendrain);
    GPIO_pupd(sda, pullup);
    GPIO_ospeed(sda, highspeed);

    if(pinSCL < 8) portSCL->AFR[0] |= (4U << (4 * pinSCL));
    else           portSCL->AFR[1] |= (4U << (4 * (pinSCL - 8)));

    if(pinSDA < 8) portSDA->AFR[0] |= (4U << (4 * pinSDA));
    else           portSDA->AFR[1] |= (4U << (4 * (pinSDA - 8)));

    // I2C3 클록 인가 (APB1 = 42.00 MHz 기준)
    RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;      // FIXED: I2C3 클록 ON

    // I2C3 비활성화 후 설정
    I2C3->CR1 &= ~I2C_CR1_PE;

    // CR2: APB1 주파수(MHz) = 42.00
    I2C3->CR2 &= ~I2C_CR2_FREQ;
    I2C3->CR2 |= 42U;

    // 표준모드 100.00 kHz
    // CCR = Fpclk1 / (2 * Fscl) = 42 MHz / (2 * 100 kHz) = 210
    I2C3->CCR = 210U;

    // TRISE = Fpclk(MHz) + 1 = 43
    I2C3->TRISE = 43U;

    // I2C3 활성화
    I2C3->CR1 |= I2C_CR1_PE;
}

static void I2C3_writeByte(uint8_t data){
    // START
    I2C3->CR1 |= I2C_CR1_START;
    while(!(I2C3->SR1 & I2C_SR1_SB));

    // 주소 + W
    I2C3->DR = LCD_I2C_ADDR;
    while(!(I2C3->SR1 & I2C_SR1_ADDR));
    (void)I2C3->SR1;
    (void)I2C3->SR2;

    // 데이터
    while(!(I2C3->SR1 & I2C_SR1_TXE));
    I2C3->DR = data;
    while(!(I2C3->SR1 & I2C_SR1_BTF));

    // STOP
    I2C3->CR1 |= I2C_CR1_STOP;
}

static void LCD1602_write4(uint8_t data){
    I2C3_writeByte(data | LCD_ENABLE | LCD_BACKLIGHT);
    delay_ms(1);
    I2C3_writeByte((data & ~LCD_ENABLE) | LCD_BACKLIGHT);
    delay_ms(1);
}

static void LCD1602_send(uint8_t value, uint8_t mode){
    uint8_t high = (value & 0xF0) | LCD_BACKLIGHT | (mode ? LCD_RS : 0);
    uint8_t low  = ((value << 4) & 0xF0) | LCD_BACKLIGHT | (mode ? LCD_RS : 0);

    LCD1602_write4(high);
    LCD1602_write4(low);
}

void LCD1602_init(void){
    delay_ms(50);

    LCD1602_write4(0x30 | LCD_BACKLIGHT);
    delay_ms(5);
    LCD1602_write4(0x30 | LCD_BACKLIGHT);
    delay_ms(1);
    LCD1602_write4(0x30 | LCD_BACKLIGHT);
    delay_ms(1);
    LCD1602_write4(0x20 | LCD_BACKLIGHT);  // 4bit 모드

    LCD1602_send(0x28, 0);    // 2라인, 5x8
    LCD1602_send(0x0C, 0);    // Display ON, Cursor OFF
    LCD1602_send(0x01, 0);    // Clear
    delay_ms(2);
    LCD1602_send(0x06, 0);    // Entry mode
}

void LCD1602_clear(void){
    LCD1602_send(0x01, 0);
    delay_ms(2);
}

void LCD1602_setCursor(uint8_t row, uint8_t col){
    uint8_t addr = (row == 0) ? (0x00 + col) : (0x40 + col);
    LCD1602_send(0x80 | addr, 0);
}

void LCD1602_print(const char *str){
    while(*str){
        LCD1602_send((uint8_t)(*str), 1);
        str++;
    }
}

void LCD1602_printInt3(int value){
    char buf[4];
    if(value < 0) value = 0;
    if(value > 999) value = 999;
    buf[0] = (char)('0' + (value / 100) % 10);
    buf[1] = (char)('0' + (value / 10) % 10);
    buf[2] = (char)('0' + (value % 10));
    buf[3] = '\0';
    LCD1602_print(buf);
}
