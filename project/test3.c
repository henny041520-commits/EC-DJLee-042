#include "ecSTM32F4v2.h"
#include "ecMFRC522.h"

void setup(void){
    RCC_PLL_init();        // 84.00 MHz
    SysTick_init();
    UART2_init();          // printf 사용 (TeraTerm 등)

    RC522_init();

    uint8_t ver = RC522_readVersion();
    printf("RC522 VersionReg = 0x%02X\r\n", ver);

    if(ver == 0x91 || ver == 0x92){
        printf("MFRC522 SPI OK\r\n");
    } else {
        printf("MFRC522 SPI FAIL\r\n");
    }
}

int main(void){
    setup();

    uint8_t uid[10];
    uint8_t uidSize;

    while(1){
        if (RC522_isNewCardPresent()){
            if (RC522_readCardUID(uid, &uidSize) == MI_OK){
                printf("Card UID: ");
                for (int i = 0; i < uidSize; i++){
                    printf("%02X ", uid[i]);
                }
                printf("\r\n");
            }
        }
        delay_ms(200);
    }
}
