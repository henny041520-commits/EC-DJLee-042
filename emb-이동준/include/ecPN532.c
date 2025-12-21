/* ecPN532.c  -  PN532 NFC (UART) driver for EC_HAL / NUCLEO-F411RE */

#include "ecPN532.h"

/* 내부 버퍼 크기 */
#define PN532_FRAME_MAX_LEN  64

/* 내부 static 함수 프로토타입 */
static void PN532_UART_writeBytes(uint8_t *buf, uint32_t len);
static int  PN532_UART_readByteTimeout(uint8_t *b, uint32_t timeout_ms);
static int  PN532_ReadFrame(uint8_t *buf, uint8_t *len, uint32_t timeout_ms);
static int  PN532_WaitAck(uint32_t timeout_ms);

/* =========================
 *   Public Functions
 * ========================= */

/* UART2 + PN532 초기화 */
void PN532_UART_init(void)
{
    /* EC_HAL UART2 초기화 (PA2=TX, PA3=RX) */
    UART2_init();
    /* PN532 보드 기본값이 115200 또는 9600인 경우가 많음
       필요 시 9600으로 변경해도 됨 */
    UART2_baud(9600);
    
    /* SysTick 초기화 (delay_ms 사용) */
    SysTick_init();
}

/* SAMConfiguration: Normal Mode, USART, IRQ 사용 */
int PN532_SAMConfiguration(void)
{
    /* Command payload: TFI 이후 기준
     * D4 14 01 14 01
     *   14: SAMConfiguration
     *   01: Normal mode
     *   14: Timeout(0x14 * 50ms)
     *   01: Use IRQ
     */
    uint8_t cmd[4];
    cmd[0] = PN532_CMD_SAMCONFIG;
    cmd[1] = 0x01;   /* Normal mode */
    cmd[2] = 0x14;   /* Timeout */
    cmd[3] = 0x01;   /* IRQ 사용 */

    uint8_t resp[PN532_FRAME_MAX_LEN];
    uint8_t resp_len = 0;

    return PN532_SendCommand(cmd, 4, resp, &resp_len, 100);
}

/* GetFirmwareVersion */
int PN532_GetFirmwareVersion(uint8_t *ic,
                             uint8_t *ver,
                             uint8_t *rev,
                             uint8_t *support)
{
    if (ic == 0 || ver == 0 || rev == 0 || support == 0)
        return PN532_ERR_PARAM;

    uint8_t cmd[1];
    cmd[0] = PN532_CMD_GETFIRMWARE;

    uint8_t resp[PN532_FRAME_MAX_LEN];
    uint8_t resp_len = 0;

    int ret = PN532_SendCommand(cmd, 1, resp, &resp_len, 100);
    if (ret != PN532_OK)
        return ret;

    /* 응답 형식: TFI(0xD5), CMD+1(0x03), IC, Ver, Rev, Support */
    if (resp_len < 6)
        return PN532_ERR_PROTOCOL;

    if (resp[0] != PN532_TFI_PN5322HOST)  /* TFI */
        return PN532_ERR_PROTOCOL;
    if (resp[1] != (PN532_CMD_GETFIRMWARE + 1U)) /* 응답 CMD 코드 */
        return PN532_ERR_PROTOCOL;

    *ic      = resp[2];
    *ver     = resp[3];
    *rev     = resp[4];
    *support = resp[5];

    return PN532_OK;
}

/* InListPassiveTarget(106 kbps, ISO14443A) */
int PN532_InListPassiveTarget14443A(uint8_t *uid_buf, uint8_t *uid_len)
{
    if (uid_buf == 0 || uid_len == 0)
        return PN532_ERR_PARAM;

    /* D4 4A 01 00
     * 4A : InListPassiveTarget
     * 01 : 최대 1개 타겟
     * 00 : 106kbps Type A
     */
    uint8_t cmd[3];
    cmd[0] = PN532_CMD_INLISTPASSIVE;
    cmd[1] = 0x01;
    cmd[2] = 0x00;

    uint8_t resp[PN532_FRAME_MAX_LEN];
    uint8_t resp_len = 0;

    int ret = PN532_SendCommand(cmd, 3, resp, &resp_len, 200);
    if (ret != PN532_OK)
        return ret;

    /* 응답 형식 (요약):
     *  resp[0] : TFI(0xD5)
     *  resp[1] : CMD+1 (0x4B)
     *  resp[2] : Targets Found (n)
     *  resp[3] : Target Number
     *  resp[4] : SENS_RES high
     *  resp[5] : SENS_RES low
     *  resp[6] : SEL_RES
     *  resp[7] : NFCID Length(UID 길이)
     *  resp[8..] : NFCID (UID)
     */
    if (resp_len < 8)
        return PN532_ERR_PROTOCOL;

    if (resp[0] != PN532_TFI_PN5322HOST)
        return PN532_ERR_PROTOCOL;
    if (resp[1] != (PN532_CMD_INLISTPASSIVE + 1U))
        return PN532_ERR_PROTOCOL;

    if (resp[2] == 0x00) {
        /* 태그 없음 */
        *uid_len = 0;
        return PN532_OK;
    }

    uint8_t len = resp[7];
    if (len > 7)  /* 7바이트까지 허용 (MIFARE 4/7바이트 UID) */
        len = 7;

    if (resp_len < (uint8_t)(8U + len))
        return PN532_ERR_PROTOCOL;

    for (uint8_t i = 0; i < len; i++)
        uid_buf[i] = resp[8 + i];

    *uid_len = len;
    return PN532_OK;
}

/* 상위 수준 명령 전송 + 응답 수신 */
int PN532_SendCommand(uint8_t *cmd, uint8_t cmd_len,
                      uint8_t *resp_buf, uint8_t *resp_len,
                      uint32_t timeout_ms)
{
    if (cmd == 0 || cmd_len == 0)
        return PN532_ERR_PARAM;

    /* 1) 프레임 생성: preamble + startcode + LEN + LCS + TFI + DATA + DCS + postamble */
    uint8_t frame[PN532_FRAME_MAX_LEN];
    uint8_t LEN  = (uint8_t)(cmd_len + 1U);     /* TFI 포함 길이 */
    uint8_t LCS  = (uint8_t)(0x100U - LEN);     /* LEN 보수 */
    uint8_t idx  = 0U;

    /* 헤더 */
    frame[idx++] = PN532_PREAMBLE;
    frame[idx++] = PN532_STARTCODE1;
    frame[idx++] = PN532_STARTCODE2;

    frame[idx++] = LEN;
    frame[idx++] = LCS;

    /* TFI */
    frame[idx++] = PN532_TFI_HOST2PN532;

    /* DATA (cmd 포함) */
    uint8_t sum = PN532_TFI_HOST2PN532;
    for (uint8_t i = 0; i < cmd_len; i++) {
        frame[idx++] = cmd[i];
        sum = (uint8_t)(sum + cmd[i]);
    }

    /* DCS */
    uint8_t DCS = (uint8_t)(0x100U - sum);
    frame[idx++] = DCS;

    /* Postamble */
    frame[idx++] = 0x00;

    /* 2) 프레임 전송 */
    PN532_UART_writeBytes(frame, idx);

    /* 3) ACK 대기 */
    int ret = PN532_WaitAck(timeout_ms);
    if (ret != PN532_OK)
        return ret;

    /* 4) 응답 프레임 수신 */
    if (resp_buf == 0 || resp_len == 0)
        return PN532_OK;   /* 응답 버퍼가 필요 없는 경우 */

    uint8_t rlen = 0;
    ret = PN532_ReadFrame(resp_buf, &rlen, timeout_ms);
    if (ret != PN532_OK)
        return ret;

    *resp_len = rlen;
    return PN532_OK;
}

/* =========================
 *   Static Functions
 * ========================= */

/* UART2로 바이트 배열 전송 */
static void PN532_UART_writeBytes(uint8_t *buf, uint32_t len)
{
    if (buf == 0 || len == 0)
        return;

    USART2_write(buf, len);
}

/* UART2에서 1바이트 읽기 (timeout_ms ms 내에 수신 없으면 에러) */
static int PN532_UART_readByteTimeout(uint8_t *b, uint32_t timeout_ms)
{
    if (b == 0)
        return PN532_ERR_PARAM;

    uint32_t start = 0U;
    uint32_t elapsed = 0U;

    /* SysTick 기반이 아니지만, 대략적 타임아웃을 위해
       ms 단위 루프를 사용 (간단 구현) */
    start = 0U;
    /* msTicks는 ecSysTick2.c 안에 있으므로 여기서는
       delay_ms(1)를 이용한 단순 루프 타이밍으로 처리 */
    while (elapsed < timeout_ms) {
        if (is_USART2_RXNE()) {
            *b = USART2_read();
            return PN532_OK;
        }
        delay_ms(1);
        elapsed++;
    }

    return PN532_ERR_TIMEOUT;
}

/* PN532 ACK 프레임 대기 */
static int PN532_WaitAck(uint32_t timeout_ms)
{
    /* ACK 프레임: 00 00 FF 00 FF 00 */
    uint8_t b;
    int ret;

    /* 1) preamble(0x00) 찾기 */
    ret = PN532_UART_readByteTimeout(&b, timeout_ms);
    if (ret != PN532_OK)
        return ret;
    if (b != 0x00)
        return PN532_ERR_PROTOCOL;

    /* 2) 00 FF 00 FF 00 순서 확인 */
    ret = PN532_UART_readByteTimeout(&b, timeout_ms);
    if (ret != PN532_OK || b != 0x00)
        return PN532_ERR_PROTOCOL;

    ret = PN532_UART_readByteTimeout(&b, timeout_ms);
    if (ret != PN532_OK || b != 0xFF)
        return PN532_ERR_PROTOCOL;

    ret = PN532_UART_readByteTimeout(&b, timeout_ms);
    if (ret != PN532_OK || b != 0x00)
        return PN532_ERR_PROTOCOL;

    ret = PN532_UART_readByteTimeout(&b, timeout_ms);
    if (ret != PN532_OK || b != 0xFF)
        return PN532_ERR_PROTOCOL;

    ret = PN532_UART_readByteTimeout(&b, timeout_ms);
    if (ret != PN532_OK || b != 0x00)
        return PN532_ERR_PROTOCOL;

    return PN532_OK;
}

/* PN532 응답 프레임 수신 (데이터 영역만 buf에 저장) */
static int PN532_ReadFrame(uint8_t *buf, uint8_t *len, uint32_t timeout_ms)
{
    if (buf == 0 || len == 0)
        return PN532_ERR_PARAM;

    uint8_t b;
    int ret;

    /* 1) preamble (0x00) */
    ret = PN532_UART_readByteTimeout(&b, timeout_ms);
    if (ret != PN532_OK) return ret;
    if (b != 0x00)      return PN532_ERR_PROTOCOL;

    /* 2) start code (00 FF) */
    ret = PN532_UART_readByteTimeout(&b, timeout_ms);
    if (ret != PN532_OK || b != 0x00) return PN532_ERR_PROTOCOL;

    ret = PN532_UART_readByteTimeout(&b, timeout_ms);
    if (ret != PN532_OK || b != 0xFF) return PN532_ERR_PROTOCOL;

    /* 3) LEN, LCS */
    uint8_t LEN, LCS;
    ret = PN532_UART_readByteTimeout(&LEN, timeout_ms);
    if (ret != PN532_OK) return ret;

    ret = PN532_UART_readByteTimeout(&LCS, timeout_ms);
    if (ret != PN532_OK) return ret;

    if ((uint8_t)(LEN + LCS) != 0x00U)
        return PN532_ERR_PROTOCOL;

    if (LEN == 0U)
        return PN532_ERR_PROTOCOL;

    /* 4) TFI + DATA */
    uint8_t sum = 0U;
    uint8_t data_len = LEN;  /* TFI 포함 */

    if (data_len > PN532_FRAME_MAX_LEN)
        data_len = PN532_FRAME_MAX_LEN;

    for (uint8_t i = 0; i < data_len; i++) {
        ret = PN532_UART_readByteTimeout(&b, timeout_ms);
        if (ret != PN532_OK) return ret;

        buf[i] = b;
        sum = (uint8_t)(sum + b);
    }

    /* 5) DCS */
    uint8_t DCS;
    ret = PN532_UART_readByteTimeout(&DCS, timeout_ms);
    if (ret != PN532_OK) return ret;

    if ((uint8_t)(sum + DCS) != 0x00U)
        return PN532_ERR_PROTOCOL;

    /* 6) postamble(0x00) */
    ret = PN532_UART_readByteTimeout(&b, timeout_ms);
    if (ret != PN532_OK) return ret;
    if (b != 0x00)       return PN532_ERR_PROTOCOL;

    *len = data_len;
    return PN532_OK;
}
