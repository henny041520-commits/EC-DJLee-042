// LAB_ADC_IR.c  (NUCLEO-F411RE, EC_HAL v2)
// Author: Your Name
#include "ecSTM32F4v2.h"
// #include "ecADC2.h"   // ecSTM32F4v2.h 에 포함되어 있다면 생략

// ---- 사용자 조정 파라미터 (현장 캘리브레이션 권장) ----
#define TH1 2000.00f   // FIXED: PB0 임계값(검은색 < TH1)  // 배선/센서 모듈에 따라 조정
#define TH2 2000.00f   // FIXED: PB1 임계값(검은색 < TH2)

// ---- 전역 측정값 ----
volatile uint32_t adc1_raw = 0;     // PB0(ADC1_CH8)
volatile uint32_t adc2_raw = 0;     // PB1(ADC1_CH9)

// ---- 함수 선언 ----
static void setup(void);
static void print_status_every_1s(void);

int main(void){
  setup();
  while(1){
    print_status_every_1s();  // 1.00 s 주기 출력
  }
}

static void setup(void){
  RCC_PLL_init();                 // 84.00 MHz
  SysTick_init();                 // 1.00 ms
  UART2_init();
  UART2_baud(BAUD_9600);          // 이미 9600으로 사용 중

  GPIO_mode(PB_0, ANALOG);
  GPIO_pupd(PB_0, nopupd);
  GPIO_mode(PB_1, ANALOG);
  GPIO_pupd(PB_1, nopupd);

  JADC_init(PB_0);                // CH8
  JADC_init(PB_1);                // CH9
  PinName_t seqJ[2] = {PB_0, PB_1};
  JADC_sequence(seqJ, 2);
  JADC_start();                   // FIXED: Injected 변환 시작 (JEOC 발생 유도)
}

void ADC_IRQHandler(void){
  if(is_ADC_OVR()) clear_ADC_OVR();              // FIXED: 오버런 클리어
  if(is_ADC_JEOC()){                             // FIXED: Injected EOC
    // 채널별 독립 데이터 레지스터에서 즉시 읽기
    adc1_raw = JADC_read(1);                     // FIXED: JSQ1 -> PB0
    adc2_raw = JADC_read(2);                     // FIXED: JSQ2 -> PB1
    clear_ADC_JEOC();                            // FIXED: JEOC 플래그 클리어
  }
}

static void print_status_every_1s(void){
  static uint32_t t = 0;
  delay_ms(1000);
  t += 1000;

  // 원시값(12-bit) 그대로 사용
  uint32_t v1u = adc1_raw;
  uint32_t v2u = adc2_raw;

  int left_is_black  = (v1u < (uint32_t)TH1) ? 1 : 0;
  int right_is_black = (v2u < (uint32_t)TH2) ? 1 : 0;

  const char* cmd = "SEARCH";
  if(left_is_black && !right_is_black)      cmd = "GO LEFT";
  else if(!left_is_black && right_is_black) cmd = "GO RIGHT";
  else if(!left_is_black && !right_is_black)  cmd = "CENTER";
  else                                      cmd = "SEARCH";

  // FIXED: float printf 미지원 대비 → 정수/고정소수 출력
  printf("IR1(PB0) = %lu.00  IR2(PB1) = %lu.00\r\n", (unsigned long)v1u, (unsigned long)v2u); // FIXED
  printf("TH1 = %.2f  TH2 = %.2f\r\n", TH1, TH2);                                             // 임계는 상수이므로 그대로 OK
  printf("%s\r\n\r\n", cmd);
}