#ifndef __EC_MFRC522_H
#define __EC_MFRC522_H

#include "ecSTM32F4v2.h"

// 핀 매핑 (필요하면 프로젝트에 맞게 수정)
#define RC522_CS    PB_6      // Chip Select
#define RC522_RST   PB_7      // Reset

// MFRC522 레지스터 주소 (데이터시트 기준)
#define RC522_CommandReg      0x01
#define RC522_ComIEnReg       0x02
#define RC522_DivIEnReg       0x03
#define RC522_ComIrqReg       0x04
#define RC522_DivIrqReg       0x05
#define RC522_ErrorReg        0x06
#define RC522_Status1Reg      0x07
#define RC522_Status2Reg      0x08
#define RC522_FIFODataReg     0x09
#define RC522_FIFOLevelReg    0x0A
#define RC522_ControlReg      0x0C
#define RC522_BitFramingReg   0x0D
#define RC522_ModeReg         0x11
#define RC522_TxControlReg    0x14
#define RC522_TModeReg        0x2A
#define RC522_TPrescalerReg   0x2B
#define RC522_TReloadRegH     0x2C
#define RC522_TReloadRegL     0x2D
#define RC522_VersionReg      0x37

// 명령어 (데이터시트)
#define RC522_CMD_IDLE        0x00
#define RC522_CMD_MEM         0x01
#define RC522_CMD_GENERATE_A_RANDOM_ID 0x02
#define RC522_CMD_CALC_CRC    0x03
#define RC522_CMD_TRANSMIT    0x04
#define RC522_CMD_RECEIVE     0x08
#define RC522_CMD_TRANSCEIVE  0x0C
#define RC522_CMD_MFAUTHENT   0x0E
#define RC522_CMD_SOFTRESET   0x0F
// ------------------------
// 카드 명령 관련 상수
// ------------------------
#define PICC_REQIDL      0x26    // Request idle (REQA)
#define PICC_REQALL      0x52    // Request all (WUPA)
#define PICC_ANTICOLL    0x93    // Anticollision, cascade level 1

// 상태 코드
#define MI_OK            0
#define MI_NOTAGERR      1
#define MI_ERR           2

// 내부 공용 통신 함수
uint8_t RC522_toCard(uint8_t command,
                     uint8_t *sendData, uint8_t sendLen,
                     uint8_t *backData, uint16_t *backBits);

// 카드 감지 + UID 읽기용 API
uint8_t RC522_request(uint8_t reqMode, uint8_t *tagType);
uint8_t RC522_anticoll(uint8_t *serNum);
uint8_t RC522_isNewCardPresent(void);
uint8_t RC522_readCardUID(uint8_t *uid, uint8_t *uidSize);

#ifdef __cplusplus
extern "C" {
#endif

// 저수준 SPI + 레지스터 접근
void     RC522_SPI1_init(void);
uint8_t  RC522_SPI1_transfer(uint8_t data);

void     RC522_writeReg(uint8_t reg, uint8_t value);
uint8_t  RC522_readReg(uint8_t reg);
void     RC522_setBitMask(uint8_t reg, uint8_t mask);
void     RC522_clearBitMask(uint8_t reg, uint8_t mask);

// 상위 레벨 초기화 / 체크
void     RC522_reset(void);
void     RC522_antennaOn(void);
void     RC522_init(void);
uint8_t  RC522_readVersion(void);

#ifdef __cplusplus
}
#endif

#endif // __EC_MFRC522_H
