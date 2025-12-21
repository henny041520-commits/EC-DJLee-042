#include "ecSTM32F4v2.h"
#include "ecMFRC522.h"
#define zero_bit PC_9
#define one_bit PB_8
#define RC522_IRQ   PC_13  

volatile uint32_t adc_raw = 0;
volatile float pressure = 0;
volatile int joy =100;
volatile int fullness = 100;
volatile int prs_cnt = 0;
volatile uint32_t sec_cnt = 0;  
volatile int feed_uid_cnt =0;
volatile int play_uid_cnt =0;
uint8_t feed_uid[4] = {0xDF, 0x12, 0x05, 0x04};  
uint8_t play_uid[4] = {0x31, 0x0A, 0xDC, 0x94};
void setup(void){
    RCC_PLL_init();
    SysTick_init();

    GPIO_init(zero_bit, OUTPUT);
    GPIO_init(one_bit, OUTPUT);

    GPIO_otype(zero_bit, pushpull);
    GPIO_pupd(zero_bit,nopupd);        
    GPIO_ospeed(zero_bit, mediumspeed);  

    GPIO_otype(one_bit, pushpull);
    GPIO_pupd(one_bit, nopupd);        
    GPIO_ospeed(one_bit, mediumspeed); 

    TIM_UI_init(TIM2, 1000);      // 1.00초마다 업데이트 이벤트 발생
    TIM_UI_enable(TIM2);  

    ADC_init(PA_0);     // 압력센서 핀
    UART2_init();         // USB 시리얼 (PA_2, PA_3)
    UART2_baud(9600);
           // printf 사용 (TeraTerm 등)
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
     state(0);
    while(1){
        float voltage = adc_raw * 3.30 / 4095.00;
        pressure = (voltage - 0.50) * (100.00 / 4.00);  // 0.5~4.5V → 0~100kPa 예시
        if(pressure>=0)
        {
            prs_cnt++;
            if(prs_cnt>=10)
            {
                prs_cnt=0;
                state(3);
            }
        }
      
        printf("Pressure: %d\r\n", prs_cnt);
           if (RC522_isNewCardPresent()){
            if (RC522_readCardUID(uid, &uidSize) == MI_OK){
                printf("Card UID: ");
                for (int i = 0; i < uidSize; i++){
                    printf("%02X ", uid[i]);
                    if(uid[i]==feed_uid[i])
                    {
                        feed_uid_cnt++;
                        
                    }
                    if(uid[i]==play_uid[i])
                    {
                        play_uid_cnt++;
                        
                    }
                }
                printf("\r\n");
            }
            if(play_uid_cnt==uidSize)
            {
                printf("play\r\n");
                play_uid_cnt=0;
            }
            else if(feed_uid_cnt == uidSize)
            {
                printf("feed\r\n");
                feed_uid_cnt=0;
            }
        }
        delay_ms(500);
    }
}

void ADC_IRQHandler(void){
    if(is_ADC_EOC()){
        adc_raw = ADC_read();  // 최신 ADC 값 저장
    }
    clear_ADC_OVR();
}

void TIM2_IRQHandler(void){
    if(is_UIF(TIM2)){              // TIM2 업데이트 이벤트 발생 여부 확인
        sec_cnt++;                 // 1.00초마다 +1

        if(sec_cnt >= 10){         // 60.00초(1.00분)마다 실행
            sec_cnt = 0;

            if(fullness > 0){
                fullness -= 1;     // 분당 1.00씩 감소 (0.00 미만으로 내려가지 않도록 보호)
                printf("fullness : %d\r\n", fullness);
            }
            if(joy > 0){
                joy -= 1;          // 분당 1.00씩 감소
                printf("joy : %d\r\n", joy);
            }
        }
        clear_UIF(TIM2);           // UIF 플래그 클리어로 다음 인터럽트 준비
    }
}


void state(int state)
{
    if(state==0)
    {
        GPIO_write(zero_bit,LOW);
        GPIO_write(one_bit,LOW);
    }
     if(state==1)
    {
        GPIO_write(zero_bit,HIGH);
        GPIO_write(one_bit,LOW);
    }
     if(state==2)
    {
        GPIO_write(zero_bit,LOW);
        GPIO_write(one_bit,HIGH);
    }
     if(state==3)
    {
        GPIO_write(zero_bit,HIGH);
        GPIO_write(one_bit,HIGH);
    }
    
}