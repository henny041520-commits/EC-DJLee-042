#include "ecDHT11.h"

// 내부 사용: TIM5를 이용한 1.00 μs 단위 딜레이
static void DHT11_delay_us(uint32_t us){
    // TIM5 클록 활성화
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;                // FIXED: DHT11 μs 지연을 위해 TIM5 클록 활성화

    // 타이머 설정: 84.00 MHz / 84 = 1.00 MHz → 1.00 μs 분해능
    TIM5->PSC = 84 - 1;
    TIM5->ARR = 0xFFFF;
    TIM5->CR1 |= TIM_CR1_CEN;

    TIM5->CNT = 0;
    while (TIM5->CNT < us);
}

// 내부 사용: 원하는 레벨이 될 때까지 대기 (μs 단위 타임아웃)
static int DHT11_wait_level(DHT11_t *dht, int level, uint32_t timeout_us){
    uint32_t cnt = 0;
    while ( (GPIO_read(dht->pin) ? 1 : 0) != level ){
        if (cnt++ >= timeout_us){
            return -1;   // 타임아웃
        }
        DHT11_delay_us(1);
    }
    return 0;            // 정상
}

void DHT11_init(DHT11_t *dht, PinName_t pinName){
    dht->pin = pinName;

    // 기본적으로 입력 풀업(또는 외부 풀업 저항 사용)
    GPIO_init(dht->pin, INPUT);
    GPIO_pupd(dht->pin, pullup);
}

int DHT11_read(DHT11_t *dht, float *temperature, float *humidity){
    uint8_t data[5] = {0,0,0,0,0};

    // 1. MCU → DHT11 스타트 신호
    GPIO_mode(dht->pin, OUTPUT);          // 출력모드
    GPIO_otype(dht->pin, pushpull);
    GPIO_pupd(dht->pin, nopupd);          // 외부 풀업저항 사용 권장

    GPIO_write(dht->pin, HIGH);
    delay_ms(1);                          // 대기 1.00 ms (안정화)

    GPIO_write(dht->pin, LOW);            // 스타트: LOW 유지 ≥ 18.00 ms
    delay_ms(20);

    GPIO_write(dht->pin, HIGH);           // HIGH 로 전환 후 짧게 대기
    DHT11_delay_us(30);

    // 2. 라인 입력 모드 전환, 풀업 활성화
    GPIO_mode(dht->pin, INPUT);
    GPIO_pupd(dht->pin, pullup);

    // 3. DHT11 응답 시퀀스:
    //    - 80.00 μs LOW
    //    - 80.00 μs HIGH
    if (DHT11_wait_level(dht, 0, 100) < 0) return -1;    // 첫 LOW (응답 시작)
    if (DHT11_wait_level(dht, 1, 100) < 0) return -2;    // 이후 HIGH
    if (DHT11_wait_level(dht, 0, 100) < 0) return -3;    // 데이터 전송 시작 전 LOW

    // 4. 40비트(5바이트) 수신
    for (int i = 0; i < 40; i++){
        // 각 비트:
        //   50.00 μs LOW 후 HIGH 시간 길이로 0/1 판별
        // (현재는 바로 LOW 상태에서 시작)

        // HIGH 시작까지 대기
        if (DHT11_wait_level(dht, 1, 70) < 0) return -4;

        // HIGH 유지 시간 중간(약 40.00 μs)에서 샘플링
        DHT11_delay_us(40);

        // HIGH이면 '1', LOW이면 '0'으로 간주
        uint8_t bit = (GPIO_read(dht->pin) ? 1 : 0);

        data[i/8] <<= 1;
        data[i/8] |= bit;

        // 다음 비트 시작을 위해 LOW 될 때까지 대기
        if (DHT11_wait_level(dht, 0, 80) < 0) return -5;
    }

    // 5. 체크섬 확인
    uint8_t sum = (uint8_t)(data[0] + data[1] + data[2] + data[3]);
    if (sum != data[4]){
        return -6;   // 체크섬 오류
    }

    // 6. DHT11 데이터 포맷
    // data[0] : 습도 정수부
    // data[1] : 습도 소수부 (일반 모듈은 0)
    // data[2] : 온도 정수부
    // data[3] : 온도 소수부 (일반 모듈은 0)
    if (humidity != 0){
        *humidity = (float)data[0] + (float)data[1] / 100.0f;
    }
    if (temperature != 0){
        *temperature = (float)data[2] + (float)data[3] / 100.0f;
    }

    return 0;   // 성공
}
