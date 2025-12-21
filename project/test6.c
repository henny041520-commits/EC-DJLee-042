#include "ecSTM32F4v2.h"
#include "ecMFRC522.h"
#include "ecI2C_LCD.h"



volatile uint32_t adc_raw = 0;
volatile float pressure = 0;
volatile int joy = 70;
volatile int fullness = 70;
volatile int prs_cnt = 0;
volatile uint32_t sec_cnt = 0;  
volatile int feed_uid_cnt = 0;
volatile int play_uid_cnt = 0;
volatile int state = 0;

// FIXED: 수신측에 "유지"시킬 상태 (state==1 용도)
volatile int hold_state = 0;                 // FIXED: 0 또는 1만 사용
volatile int prev_hold_state = 0;            // FIXED: 상태 변화 감지용

// FIXED: RC522 장시간 동작 중 폴링 실패 누적 시 자동 복구용
volatile uint32_t rc522_fail_cnt = 0;        // FIXED: 연속 실패 카운터(루프 기반)

uint8_t feed_uid[4] = {0x22, 0x36, 0x87, 0x90};
uint8_t play_uid[4] = {0xE1, 0x11, 0x15, 0x0E};

// FIXED: 이벤트(state=2,3,4) 전송 함수 (전송 후 상태 리셋은 "호출부"에서 결정)
static void send_event_usart6(uint8_t code){
    USART_write(USART6, &code, 1);           // FIXED: 1바이트 이벤트 코드 전송
}

// FIXED: 유지(state=1) 전송/해제 함수 (변화 있을 때만 전송)
static void update_hold_usart6(void){
    if(hold_state != prev_hold_state){
        uint8_t tx = (uint8_t)hold_state;    // FIXED: 1이면 유지 시작, 0이면 유지 해제
        USART_write(USART6, &tx, 1);
        prev_hold_state = hold_state;
    }
}

// FIXED: RC522가 일정 시간 이상 연속 실패하면 리셋/재초기화로 복구
static void RC522_recover_if_stuck(uint8_t present){
    if(present) rc522_fail_cnt = 0;                          // FIXED: 카드 감지되면 실패 카운터 리셋
    else{
        rc522_fail_cnt++;                                    // FIXED: 감지 실패 누적
        if(rc522_fail_cnt >= 40){                            // FIXED: 0.50 s * 40 = 20.00 s 연속 실패 시 복구
            RC522_init();                                    // FIXED: SPI 포함 전체 재초기화(가장 확실)
            rc522_fail_cnt = 0;                              // FIXED: 복구 후 카운터 리셋
        }
    }
}

void setup(void){
    RCC_PLL_init();
    SysTick_init();

    

    TIM_UI_init(TIM2, 1000);
    TIM_UI_enable(TIM2);

    ADC_init(PA_0);

    UART2_init();
    UART2_baud(9600);

    UART6_init();
    UART6_baud(9600);

    USART6->CR1 &= ~USART_CR1_RXNEIE;   // FIXED: 수신 인터럽트 OFF (TX만 사용하면 필요 없음)
    NVIC_DisableIRQ(USART6_IRQn);       // FIXED: USART6 IRQ 자체 Disable (TX만이면 불필요)

    RC522_init();

    I2C3_init_LCD(PA_8, PC_9);
    LCD1602_init();
    LCD1602_clear();
    LCD1602_setCursor(0, 0);
    LCD1602_print("J:");
    LCD1602_setCursor(1, 0);
    LCD1602_print("F:");

    uint8_t ver = RC522_readVersion();
    printf("RC522 VersionReg = 0x%02X\r\n", ver);

    if(ver == 0x91 || ver == 0x92) printf("MFRC522 SPI OK\r\n");
    else                           printf("MFRC522 SPI FAIL\r\n");
}

int main(void){
    setup();
    uint8_t uid[10];
    uint8_t uidSize;
    state = 0;

    while(1){
        float voltage = adc_raw * 3.30f / 4095.00f;
        pressure = (voltage - 0.50f) * (100.00f / 4.00f);

        // ---------------- 이벤트 1) pressure 10회마다 state=4 이벤트 전송 ----------------
        if(pressure >= 0.00f){
            prs_cnt++;
            printf("Pressure: %d\r\n", prs_cnt);
            printf("pressure = %d.%02d\r\n", (int)pressure, (int)((pressure - (int)pressure) * 100.00f));
            if(prs_cnt >= 10){
                prs_cnt = 0;
                send_event_usart6(5);                     // FIXED: state로 미루지 말고 즉시 4 전송(2/3 덮어쓰기 방지)
                // state = 4;                              // FIXED: 제거 - 아래 카드 로직에서 state가 덮일 수 있음
            }
        }

        LCD1602_setCursor(0, 2);
        LCD1602_printInt3(joy);
        LCD1602_print("   ");

        LCD1602_setCursor(1, 2);
        LCD1602_printInt3(fullness);
        LCD1602_print("   ");
                LCD1602_setCursor(0, 2);
        LCD1602_printInt3(joy);
        LCD1602_print("   ");

        LCD1602_setCursor(1, 2);
        LCD1602_printInt3(fullness);
        LCD1602_print("   ");

        // FIXED: LCD 0행 오른쪽에 PRS CNT 표시 (예: "P:010")
        LCD1602_setCursor(0, 10);            // FIXED: 0행 10열부터 출력(남는 공간 사용)
        LCD1602_print("P:");                 // FIXED: 라벨
        LCD1602_printInt3(prs_cnt);          // FIXED: 3자리 정수로 prs_cnt 표시


        // ---------------- 이벤트 2) 카드 태깅마다 state=2 또는 3 이벤트 전송 ----------------
        // FIXED: present 결과를 따로 저장해서 "연속 실패 시 RC522 복구"에 사용
        uint8_t present = RC522_isNewCardPresent();       // FIXED: 카드 감지 결과 저장
        RC522_recover_if_stuck(present);                  // FIXED: 장시간 실패 시 자동 복구

        if (present){

            feed_uid_cnt = 0;                            // FIXED: 태깅 시도마다 초기화
            play_uid_cnt = 0;                            // FIXED: 태깅 시도마다 초기화
            uidSize = 0;                                 // FIXED: 이전 uidSize 잔류 방지

            // FIXED: UID read는 RF/거리/각도에 따라 1회 실패가 흔함 → 짧게 3회 재시도
            int ok = 0;                                  // FIXED: UID 성공 여부 플래그
            for(int retry = 0; retry < 3; retry++){      // FIXED: 재시도 3회
                if (RC522_readCardUID(uid, &uidSize) == MI_OK){
                    ok = 1;                               // FIXED: 성공
                    break;
                }
                delay_ms(10);                             // FIXED: 다음 시도 전 10.00 ms 대기
            }

            if (ok){

                printf("Card UID: ");
                for (int i = 0; i < uidSize; i++){
                    printf("%02X ", uid[i]);
                    if(uid[i] == feed_uid[i]) feed_uid_cnt++;
                    if(uid[i] == play_uid[i]) play_uid_cnt++;
                }
                printf("\r\n");

                if(play_uid_cnt == uidSize){
                    if(joy >= 70) state = 2;             // FIXED: play 이벤트
                    else{
                        joy = 100;
                        state = 3;                       // FIXED: play 이벤트(리필)
                    }
                    printf("play\r\n");
                }
                else if(feed_uid_cnt == uidSize){
                    if(fullness >= 70) state = 2;        // FIXED: feed 이벤트
                    else{
                        fullness = 100;
                        state = 3;                       // FIXED: feed 이벤트(리필)
                    }
                    printf("feed\r\n");
                }
                else{
                    feed_uid_cnt = 0;                    // FIXED: 실패 시 클리어
                    play_uid_cnt = 0;                    // FIXED: 실패 시 클리어
                    uidSize = 0;                         // FIXED: 실패 시 클리어
                }

                delay_ms(200);                           // FIXED: 같은 태그를 연속으로 중복 인식하는 것 완화
            }
            else{
                feed_uid_cnt = 0;                        // FIXED: UID read 실패 시 잔류 방지
                play_uid_cnt = 0;                        // FIXED: UID read 실패 시 잔류 방지
                uidSize = 0;                             // FIXED: UID read 실패 시 잔류 방지
            }
        }

        // ---------------- 유지 상태(state=1): 조건 동안 수신측에서 계속 유지 ----------------
        if(fullness <= 50 || joy <= 50){
            hold_state = 1;                               // FIXED: 유지 ON
        }
        else{
            hold_state = 0;                               // FIXED: 유지 OFF(해제)
        }

        // FIXED: hold_state 변화가 있을 때만 1/0 전송 (수신측 래치/해제 용도)
        update_hold_usart6();

        // FIXED: 카드 이벤트(2/3)만 여기서 전송 (4는 위에서 즉시 전송됨)
        if(state == 2 || state == 3){                     // FIXED: 4 제거
            send_event_usart6((uint8_t)state);            // FIXED: 이벤트는 발생할 때마다 전송
            state = 0;                                    // FIXED: 이벤트 전송 후에만 0으로 리셋
        }

        delay_ms(500);
    }
}

void ADC_IRQHandler(void){
    if(is_ADC_EOC()){
        adc_raw = ADC_read();
    }
    clear_ADC_OVR();
}

void TIM2_IRQHandler(void){
    if(is_UIF(TIM2)){
        sec_cnt++;

        if(sec_cnt >=20){
            sec_cnt = 0;

            if(fullness > 0){
                fullness -= 1;
                printf("fullness : %d\r\n", fullness);
            }
            if(joy > 0){
                joy -= 1;
                printf("joy : %d\r\n", joy);
            }
        }
        clear_UIF(TIM2);
    }
}
