#include "ecMFRC522.h"

// 내부 GPIO 핸들링용
static void RC522_CS_low(void){
    GPIO_write(RC522_CS, LOW);
}
static void RC522_CS_high(void){
    GPIO_write(RC522_CS, HIGH);
}
static void RC522_RST_low(void){
    GPIO_write(RC522_RST, LOW);
}
static void RC522_RST_high(void){
    GPIO_write(RC522_RST, HIGH);
}

// -----------------------------
// 1) SPI1 초기화 (Mode 0, 8bit, ~1.31 MHz)
// -----------------------------
void RC522_SPI1_init(void){
    GPIO_TypeDef *port;
    unsigned int pin;

    // SCK  : PA_5 (SPI1_SCK, AF5)
    // MISO : PA_6 (SPI1_MISO, AF5)
    // MOSI : PA_7 (SPI1_MOSI, AF5)

    GPIO_init(PA_5, AF);
    GPIO_ospeed(PA_5, EC_HIGH);
    GPIO_otype(PA_5, pushpull);
    GPIO_pupd(PA_5, EC_PU);

    GPIO_init(PA_6, AF);
    GPIO_ospeed(PA_6, EC_HIGH);
    GPIO_otype(PA_6, pushpull);
    GPIO_pupd(PA_6, EC_PU);

    GPIO_init(PA_7, AF);
    GPIO_ospeed(PA_7, EC_HIGH);
    GPIO_otype(PA_7, pushpull);
    GPIO_pupd(PA_7, EC_PU);

    // AF5 설정
    ecPinmap(PA_5, &port, &pin);
    port->AFR[pin >> 3] &= ~(0xF << (4 * (pin % 8)));
    port->AFR[pin >> 3] |=  0x5 << (4 * (pin % 8));

    ecPinmap(PA_6, &port, &pin);
    port->AFR[pin >> 3] &= ~(0xF << (4 * (pin % 8)));
    port->AFR[pin >> 3] |=  0x5 << (4 * (pin % 8));

    ecPinmap(PA_7, &port, &pin);
    port->AFR[pin >> 3] &= ~(0xF << (4 * (pin % 8)));
    port->AFR[pin >> 3] |=  0x5 << (4 * (pin % 8));

    // CS, RST 핀 출력 설정
    GPIO_init(RC522_CS, OUTPUT);
    GPIO_otype(RC522_CS, pushpull);
    GPIO_ospeed(RC522_CS, EC_HIGH);
    GPIO_pupd(RC522_CS, EC_PU);
    RC522_CS_high();

    GPIO_init(RC522_RST, OUTPUT);
    GPIO_otype(RC522_RST, pushpull);
    GPIO_ospeed(RC522_RST, EC_HIGH);
    GPIO_pupd(RC522_RST, EC_PU);
    RC522_RST_high();

    // SPI1 클록 활성화
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // SPI1 CR1 설정
    // MSTR=1, BR=F_PCLK/64, CPOL=0, CPHA=0, DFF=8bit, SSM=1, SSI=1
    SPI1->CR1 = 0;
    SPI1->CR1 |= SPI_CR1_MSTR;          // Master
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;  // 소프트웨어 NSS
    SPI1->CR1 |= (0x5 << SPI_CR1_BR_Pos);    // 분주 64 → 약 1.31 MHz
    SPI1->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);
    SPI1->CR1 &= ~SPI_CR1_DFF;          // 8-bit

    // SPI1 Enable
    SPI1->CR1 |= SPI_CR1_SPE;
}

// -----------------------------
// 2) SPI 전송 1바이트
// -----------------------------
uint8_t RC522_SPI1_transfer(uint8_t data){
    // TXE 대기
    while(!(SPI1->SR & SPI_SR_TXE));
    *((__IO uint8_t*)&SPI1->DR) = data;

    // RXNE 대기
    while(!(SPI1->SR & SPI_SR_RXNE));
    return *((__IO uint8_t*)&SPI1->DR);
}

// -----------------------------
// 3) 레지스터 쓰기/읽기 (SPI 프로토콜)
// -----------------------------
// SPI write: [addr<<1 & 0x7E] , [data]
// SPI read : [(addr<<1 & 0x7E) | 0x80] , [dummy] → 응답 :contentReference[oaicite:8]{index=8}

void RC522_writeReg(uint8_t reg, uint8_t value){
    RC522_CS_low();
    RC522_SPI1_transfer((reg << 1) & 0x7E);
    RC522_SPI1_transfer(value);
    RC522_CS_high();
}

uint8_t RC522_readReg(uint8_t reg){
    uint8_t value;
    RC522_CS_low();
    RC522_SPI1_transfer(((reg << 1) & 0x7E) | 0x80);
    value = RC522_SPI1_transfer(0x00);
    RC522_CS_high();
    return value;
}

void RC522_setBitMask(uint8_t reg, uint8_t mask){
    uint8_t tmp = RC522_readReg(reg);
    RC522_writeReg(reg, tmp | mask);
}

void RC522_clearBitMask(uint8_t reg, uint8_t mask){
    uint8_t tmp = RC522_readReg(reg);
    RC522_writeReg(reg, tmp & (~mask));
}

// -----------------------------
// 4) 리셋 / 안테나ON / 초기화
// -----------------------------
void RC522_reset(void){
    // 하드웨어 리셋 펄스
    RC522_RST_low();
    delay_ms(10);
    RC522_RST_high();
    delay_ms(10);

    // 소프트웨어 리셋
    RC522_writeReg(RC522_CommandReg, RC522_CMD_SOFTRESET);
    // 내부 리셋 완료까지 대기 (최대 수 ms)
    delay_ms(50);
}

void RC522_antennaOn(void){
    uint8_t value = RC522_readReg(RC522_TxControlReg);
    if ((value & 0x03) != 0x03){
        RC522_setBitMask(RC522_TxControlReg, 0x03);
    }
}

// 데이터시트, 대표 라이브러리가 사용하는 초기값 참조 :contentReference[oaicite:9]{index=9}
void RC522_init(void){
    RC522_SPI1_init();
    RC522_reset();

    // Timer 설정 (TMode, TPrescaler, TReload)
    RC522_writeReg(RC522_TModeReg,       0x80);   // TAuto=1
    RC522_writeReg(RC522_TPrescalerReg,  0xA9);   // 분주 설정
    RC522_writeReg(RC522_TReloadRegL,    0xE8);   // Reload = 0x03E8 (1000)
    RC522_writeReg(RC522_TReloadRegH,    0x03);

    // ModeReg: CRC preset, Tx/Rx 설정
    RC522_writeReg(RC522_ModeReg, 0x3D);

    // 인터럽트, FIFO, 기타 기본값 정리 (필요 최소만)
    RC522_writeReg(RC522_TxControlReg, RC522_readReg(RC522_TxControlReg));

    // 안테나 ON
    RC522_antennaOn();
}

// -----------------------------
// 5) VersionReg 읽기 (디버깅 핵심)
// -----------------------------
uint8_t RC522_readVersion(void){
    return RC522_readReg(RC522_VersionReg);
}
// ------------------------------------------------------
// 6) RC522_toCard : RC522 ↔ 카드 간 송수신 공용 함수
// ------------------------------------------------------
uint8_t RC522_toCard(uint8_t command,
                     uint8_t *sendData, uint8_t sendLen,
                     uint8_t *backData, uint16_t *backBits)
{
    uint8_t status = MI_ERR;
    uint8_t irqEn = 0;
    uint8_t waitIRq = 0;
    uint8_t n;
    uint32_t i;

    if (command == RC522_CMD_MFAUTHENT) {
        irqEn   = 0x12;           // Bit 1,4
        waitIRq = 0x10;           // Bit 4
    }
    else if (command == RC522_CMD_TRANSCEIVE) {
        irqEn   = 0x77;           // 모든 인터럽트
        waitIRq = 0x30;           // RxIRq | IdleIRq
    }

    // 인터럽트 설정
    RC522_writeReg(RC522_ComIEnReg, irqEn | 0x80);  // IRQ pin 활성
    RC522_clearBitMask(RC522_ComIrqReg, 0x80);      // 인터럽트 플래그 클리어
    RC522_setBitMask(RC522_FIFOLevelReg, 0x80);     // FIFO 포인터 reset

    // STOP
    RC522_writeReg(RC522_CommandReg, RC522_CMD_IDLE);

    // 송신 데이터 FIFO에 기록
    for (i = 0; i < sendLen; i++) {
        RC522_writeReg(RC522_FIFODataReg, sendData[i]);
    }

    // 명령 설정
    RC522_writeReg(RC522_CommandReg, command);

    if (command == RC522_CMD_TRANSCEIVE) {
        RC522_setBitMask(RC522_BitFramingReg, 0x80);   // StartSend = 1
    }

    // 명령 완료 대기 (최대 루프)
    i = 2000;
    do {
        n = RC522_readReg(RC522_ComIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitIRq));

    // StartSend 클리어
    RC522_clearBitMask(RC522_BitFramingReg, 0x80);

    if (i != 0) {
        // 에러 확인
        uint8_t errorReg = RC522_readReg(RC522_ErrorReg);
        if (!(errorReg & 0x1B)) {          // BufferOvfl, ParityErr, ProtocolErr 비트
            status = MI_OK;

            if (n & 0x01) {                // Timer interrupt
                status = MI_NOTAGERR;
            }

            if (command == RC522_CMD_TRANSCEIVE) {
                uint8_t fifoLevel = RC522_readReg(RC522_FIFOLevelReg);
                uint8_t lastBits  = RC522_readReg(RC522_ControlReg) & 0x07;
                if (lastBits) {
                    *backBits = (fifoLevel - 1) * 8 + lastBits;
                } else {
                    *backBits = fifoLevel * 8;
                }

                if (fifoLevel == 0) {
                    status = MI_NOTAGERR;
                }

                if (backData != 0 && fifoLevel) {
                    for (i = 0; i < fifoLevel; i++) {
                        backData[i] = RC522_readReg(RC522_FIFODataReg);
                    }
                }
            }
        }
        else {
            status = MI_ERR;
        }
    }

    return status;
}

// ------------------------------------------------------
// 7) 카드 존재 여부 확인 (REQA / WUPA)
// ------------------------------------------------------
uint8_t RC522_request(uint8_t reqMode, uint8_t *tagType)
{
    uint8_t status;
    uint16_t backBits;       // 수신 비트 수
    uint8_t buf[2];

    // BitFramingReg: TxLastBits=7 (7비트만 전송)
    RC522_writeReg(RC522_BitFramingReg, 0x07);

    buf[0] = reqMode;        // 0x26 (REQA) 또는 0x52 (WUPA)
    backBits = 0;

    status = RC522_toCard(RC522_CMD_TRANSCEIVE,
                          buf, 1,
                          tagType, &backBits);

    // ATQA = 2바이트 → 16비트가 정상
    if ((status != MI_OK) || (backBits != 0x10)) {
        status = MI_ERR;
    }

    return status;
}

// ------------------------------------------------------
// 8) 안티콜리전(UID 4바이트 + BCC 읽기)
// ------------------------------------------------------
uint8_t RC522_anticoll(uint8_t *serNum)
{
    uint8_t status;
    uint8_t i;
    uint8_t serNumCheck = 0;
    uint16_t unLen;
    uint8_t buf[2];

    // BitFramingReg: Tx/Rx 모두 바이트 정렬
    RC522_writeReg(RC522_BitFramingReg, 0x00);

    // 안티콜리전 명령
    buf[0] = PICC_ANTICOLL;   // 0x93
    buf[1] = 0x20;

    unLen = 0;
    status = RC522_toCard(RC522_CMD_TRANSCEIVE,
                          buf, 2,
                          serNum, &unLen);

    // UID[0..4] (4바이트 UID + BCC) → 총 40비트 = 5바이트
    if (status == MI_OK) {
        if (unLen != 0x28) {   // 40비트 = 0x28
            status = MI_ERR;
        } else {
            // BCC 체크 (serNum[0..3] XOR 합이 serNum[4])
            for (i = 0; i < 4; i++) {
                serNumCheck ^= serNum[i];
            }
            if (serNumCheck != serNum[4]) {
                status = MI_ERR;
            }
        }
    }

    return status;
}

// ------------------------------------------------------
// 9) 상위에서 쓰기 쉬운 래퍼들
// ------------------------------------------------------

// 카드를 "새로" 감지하고 있는지 간단 확인
uint8_t RC522_isNewCardPresent(void)
{
    uint8_t status;
    uint8_t tagType[2];

    status = RC522_request(PICC_REQIDL, tagType);
    if (status == MI_OK) return 1;
    else                 return 0;
}

// UID 읽기 (4바이트, UID 크기 반환)
uint8_t RC522_readCardUID(uint8_t *uid, uint8_t *uidSize)
{
    uint8_t status;
    uint8_t serNum[5];

    status = RC522_anticoll(serNum);
    if (status != MI_OK) {
        return MI_ERR;
    }

    // UID 4바이트만 상위에 전달
    for (int i = 0; i < 4; i++) {
        uid[i] = serNum[i];
    }
    if (uidSize) *uidSize = 4;

    return MI_OK;
}

