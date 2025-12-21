/* ecPN532.h  -  PN532 NFC (UART) driver for EC_HAL / NUCLEO-F411RE */

#ifndef __EC_PN532_H
#define __EC_PN532_H

#include "stm32f411xe.h"
#include "ecUART2.h"
#include "ecSysTick2.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* PN532 기본 상수 */
#define PN532_PREAMBLE          0x00
#define PN532_STARTCODE1        0x00
#define PN532_STARTCODE2        0xFF

#define PN532_TFI_HOST2PN532    0xD4
#define PN532_TFI_PN5322HOST    0xD5

/* 주요 명령 */
#define PN532_CMD_GETFIRMWARE   0x02
#define PN532_CMD_SAMCONFIG     0x14
#define PN532_CMD_INLISTPASSIVE 0x4A

/* 반환값 */
#define PN532_OK                0
#define PN532_ERR_TIMEOUT      -1
#define PN532_ERR_PROTOCOL     -2
#define PN532_ERR_ACK          -3
#define PN532_ERR_PARAM        -4

/* 공개 API */

/* 1) UART2 + PN532 초기화 (115200bps 기준) */
void PN532_UART_init(void);

/* 2) SAMConfiguration: PN532를 Normal Mode + IRQ 사용으로 설정 */
int  PN532_SAMConfiguration(void);

/* 3) 펌웨어 버전 읽기 */
int  PN532_GetFirmwareVersion(uint8_t *ic,
                              uint8_t *ver,
                              uint8_t *rev,
                              uint8_t *support);

/* 4) ISO14443A 태그 UID 읽기 (최대 7바이트 UID) 
 *    uid_buf  : UID 저장 버퍼
 *    uid_len  : 읽어온 UID 길이
 *    return   : PN532_OK(0) 또는 에러 코드
 */
int  PN532_InListPassiveTarget14443A(uint8_t *uid_buf, uint8_t *uid_len);

/* 디버깅용 저수준 함수 (원하면 사용) */
int  PN532_SendCommand(uint8_t *cmd, uint8_t cmd_len,
                       uint8_t *resp_buf, uint8_t *resp_len,
                       uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* __EC_PN532_H */
