#include "ecSTM32F4v2.h"// this should include #include "ecUART2.h" 
//#include "ecUART2.h" 

static volatile uint8_t PC_Data = 0;
static volatile uint8_t BT_Data = 0;

int i = 0;
int k =0;
int h =1;
volatile int direc_state = 0;
int dr_toggle = 0;
int A_dutystate_L = 2;
int A_dutystate_R = 2;
volatile float m_out_put_duty[4] = {0.1, 0.3, 0.8, 1};
volatile float blank_out_put_duty[4];
volatile uint8_t rx_buf[3];
volatile uint8_t rx_idx = 0;
volatile uint32_t adc1_raw = 0;     // PB0(ADC1_CH8)
volatile uint32_t adc2_raw = 0;     // PB1(ADC1_CH9)
#define TH1 3000.00f 
#define TH2 3000.00f
#define LED_PIN PA_5
#define DC_PWMA  PA_0
#define DC_PWMB  PA_1
#define DIR_PINA PC_2
#define DIR_PINB PC_3
#define PWM_PIN  PA_8
#define anal0 PB_0
#define anal1 PB_1

#define LINE_STATE_STRAIGHT 0        // 직선 정상 주행 상태
#define LINE_STATE_CURVE    1        // 코너 회전 중 상태
#define LINE_STATE_EXIT     2        // 코너 → 직선 정렬 상태
static int line_state = LINE_STATE_STRAIGHT;  // FIXED: 현재 라인 상태
static int last_turn_dir = 0;                 // FIXED: 마지막 회전 방향 (-1: LEFT, +1: RIGHT)
static int exit_center_count = 0;             // FIXED: CENTER 패턴 유지 카운트
static uint32_t base_right = 0;               // FIXED: 오른쪽 센서(ADC1, IR1) 환경 기준값
static uint32_t base_left  = 0;               // FIXED: 왼쪽  센서(ADC2, IR2) 환경 기준값
static int      base_init  = 0;               // FIXED: 기준값 초기화 플래그

// ================== [FIXED: 초음파 + 장애물 STOP 추가 전역] ==================
uint32_t ovf_cnt = 0;        // FIXED: 초음파 센서1(TIM4 CH1/2) overflow 카운트
uint32_t ovf_cnt2 = 0;       // FIXED: 초음파 센서2(TIM4 CH3/4) overflow 카운트
float timeInterval = 0.0f;   // FIXED: 센서1 왕복 시간 [ms]
float timeInterval2 = 0.0f;  // FIXED: 센서2 왕복 시간 [ms]
float time1 = 0.0f;          // FIXED: 센서1 상승엣지
float time2 = 0.0f;          // FIXED: 센서1 하강엣지
float time3 = 0.0f;          // FIXED: 센서2 상승엣지
float time4 = 0.0f;          // FIXED: 센서2 하강엣지
volatile float distance = 0.0f;       // FIXED: 센서1 거리 [cm]
volatile float distance2 = 0.0f;      // FIXED: 센서2 거리 [cm]
volatile int GoOrStopMode = 1;  // FIXED: 0 = STOP(장애물 근접), 1 = GO
volatile state = 0;      
 char buf[64];
int n;
     char buf2[64];
          int n2;

#define TRIG   PA_6          // FIXED: 초음파1 Trig (TIM3 CH1 PWM)
#define ECHO   PB_6          // FIXED: 초음파1 Echo (TIM4 CH1/2 ICAP)
#define TRIG2  PB_5          // FIXED: 초음파2 Trig (PWM) // TIM3 CH2
#define ECHO2  PB_8          // FIXED: 초음파2 Echo (TIM4 CH3/4 ICAP)

// ======================================================================

PinName_t seqJ[2] = {anal0, anal1};

//-------------------- 장애물 + 초음파 함수 선언 (원형) -----------------// FIXED
void update_ultrasonic_and_stop(void);       // FIXED: 초음파 거리 측정 + Go/Stop 설정
void TIM4_IRQHandler(void);                  // FIXED: TIM4 입력 캡처 ISR

float ps = 0.0;

float angle(int angle){
   float a = 0.5 + (2.0 * angle / 180);
   return a;
}
//----------------------------------------------
 void out(uint8_t PC_string[])
{
  USART1_write(PC_string, (uint32_t)strlen((char*)PC_string));

}
//-----------------------------------------------
void Steer(volatile uint8_t single)
{
  if(h<0)
  { check(h);
    out("Arrow: LEFT\r\n");  
    // FIXED: 장애물 있을 때는 강제 STOP
    if(GoOrStopMode == 0){
      PWM_duty(DC_PWMA,1);
      PWM_duty(DC_PWMB,0);
    }else{
      PWM_duty(DC_PWMA,1- blank_out_put_duty[0]);
      PWM_duty(DC_PWMB,blank_out_put_duty[abs(h)]);
    }
  }
  if(h>0)
  { check(h);
    out("Arrow: RIGHT\r\n");
    if(GoOrStopMode == 0){
      PWM_duty(DC_PWMA,1);
      PWM_duty(DC_PWMB,0);
        // 멈출 때 각도 (예: 오른쪽으로 꺾기)
     
    }else{
      PWM_duty(DC_PWMA,1- blank_out_put_duty[h]);
      PWM_duty(DC_PWMB,blank_out_put_duty[0]);
       // 갈 때 각도 (예: 정면)
      
    }
  }
  if(h==0)
  {
    check(h);
    out("Straight\r\n"); 
    if(GoOrStopMode == 0){
      PWM_duty(DC_PWMA,1);
      PWM_duty(DC_PWMB,0);
    }else{
      PWM_duty(DC_PWMA,0);
      PWM_duty(DC_PWMB,1);
    }
  }

}
//-----------------------------------------------
void direction_choice(volatile uint8_t single)
{
  if (single==0)
  {
  GPIO_write(DIR_PINA,1);
  GPIO_write(DIR_PINB,0);
  out("Forward\r\n");
  for(k=0;k<=3;k++)
  {
    blank_out_put_duty[k]=m_out_put_duty[k];
  }
  }
  else
  {
  GPIO_write(DIR_PINA,0);
  GPIO_write(DIR_PINB,1);
  out("Backward\r\n");
  for(k=0;k<=3;k++)
  {
    blank_out_put_duty[k]=m_out_put_duty[3-k];
  }
  }
}


//-----------------------------------------------
void handle_char_for_M(volatile uint8_t single)
{
  // FIXED: 장애물 있을 때, STOP 이외 명령은 무시하고 강제 정지
    if(GoOrStopMode == 0 && single != 'S' && single != 's'
                       && single != 'A' && single != 'a'){   // FIXED: A는 통과
    out("Obstacle STOP (M mode)\r\n");     // FIXED
    PWM_duty(DC_PWMA,1);
    PWM_duty(DC_PWMB,0);
    return;
  }

if(single == 'S'||single =='s')
{  out("STOP\r\n");
  PWM_duty(DC_PWMA,1);
  PWM_duty(DC_PWMB,0);
} 
else if(single == 'F'||single =='f')
{
  direc_state = 0;
  direction_choice(direc_state);
  if(GoOrStopMode == 0){                  // FIXED
    PWM_duty(DC_PWMA,1);
    PWM_duty(DC_PWMB,0);
  }else{
    PWM_duty(DC_PWMA,0);
    PWM_duty(DC_PWMB,1);
  }

  }
else if(single == 'B'||single =='b')
{
  direc_state = 1;
  direction_choice(direc_state);
  if(GoOrStopMode == 0){                  // FIXED
    PWM_duty(DC_PWMA,1);
    PWM_duty(DC_PWMB,0);
  }else{
    PWM_duty(DC_PWMA,1);
    PWM_duty(DC_PWMB,0);
  }
  }
else if (single == 'A'||single == 'a' )
{
  state=0;
  out("Mode A\r\n");   
} 
                
} 
//-----------------------------------------------
void check(int i)
{
if(i== 0)
{
    out("0");
} 
else if(i==1)
{
  out("1");
}
else if(i==2)
{
  out("2");
}
else if(i==3)
{
  out("3");
}
else if(i==-1)
{
  out("-1");
}
else if(i==-2)
{
  out("-2");
}
else if(i==-3)
{
  out("-3");
}

}
//-------------------------------------------
static inline void handle_arrow(uint8_t code){
  // FIXED: 장애물 있으면 방향키도 무시하고 STOP
  if(GoOrStopMode == 0){
    out("Obstacle STOP (arrow)\r\n");
    PWM_duty(DC_PWMA,1);
    PWM_duty(DC_PWMB,0);
    return;
  }

  switch(code){
    case 'A': 
    
    i++;
    if(i>=3)
    {i=3;}
    PWM_duty(DC_PWMA,1- blank_out_put_duty[i]);
    PWM_duty(DC_PWMB,blank_out_put_duty[i]);
    check(i);
    out("Arrow: UP\r\n");
    break;
    case 'B': 
    
    i--;
    if(i<=0)
    {
      i=0;
    }
    PWM_duty(DC_PWMA,1- blank_out_put_duty[i]);
    PWM_duty(DC_PWMB,blank_out_put_duty[i]);
   check(i);
    out("Arrow: DOWN\r\n");
    break;
    case 'C': 
    
    h++;
    if(h>=3)
    {h=3;}
   
    Steer(h);
    break;
    case 'D': 
    h--;
    if(h<=-3)
    {h=-3;}
    
    Steer(h);
    break;
    default: break;
  }
}
//--------------------------------------------------------
static void print_status_every_1s(void){
  static uint32_t t = 0;
  delay_ms(10);
  t += 1000;

  uint32_t v1u = adc1_raw;
  uint32_t v2u = adc2_raw;


   if(base_init == 0){                                      // FIXED: 첫 진입 시 현재 값을 기준으로 초기화
    base_right = v1u;                                      // FIXED: 오른쪽 센서 기준값
    base_left  = v2u;                                      // FIXED: 왼쪽  센서 기준값
    base_init  = 1;                                        // FIXED
  }
  else{
    // FIXED: 이동 평균(저역통과 필터)로 서서히 환경 변화에 따라감 (계수 0.90 / 0.10)
    base_right = (base_right * 9 + v1u) / 10;              // FIXED
    base_left  = (base_left  * 9 + v2u) / 10;              // FIXED
  }

  // ===== 자동 임계값 계산 (기준값의 120.00 %) =====
  uint32_t THR = base_right + base_right / 5;              // FIXED: 오른쪽 = 기준값 × 1.20
  uint32_t THL = base_left  + base_left  / 5;              // FIXED: 왼쪽  = 기준값 × 1.20

  // ===== 검정/흰색 판별 (센서별 자동 임계값 사용) =====
  int left_is_black  = (v2u > 1700) ? 1 : 0;                //THL FIXED: 왼쪽 IR2
  int right_is_black = (v1u > 1700) ? 1 : 0;    //THR

  const char* cmd = "SEARCH";

  // FIXED: IR 패턴 + 상태 기반으로 듀티를 결정하는 FSM 추가
  if(left_is_black && !right_is_black){
    // 왼쪽 센서만 검정 → 왼쪽으로 도는 곡선 상태
    cmd = "GO LEFT";
    last_turn_dir = -1;                    // FIXED: 마지막 회전 방향 저장
    line_state = LINE_STATE_CURVE;         // FIXED: 곡선 상태로 진입
    exit_center_count = 0;                 // FIXED: 정렬 카운터 초기화

    A_dutystate_L = 0;                     // 원래 사용하던 강한 회전 듀티 유지
    A_dutystate_R = 2;
  }
  else if(!left_is_black && right_is_black){
    // 오른쪽 센서만 검정 → 오른쪽 곡선
    cmd = "GO RIGHT";
    last_turn_dir = +1;                    // FIXED
    line_state = LINE_STATE_CURVE;         // FIXED
    exit_center_count = 0;                 // FIXED

    A_dutystate_L = 2;
    A_dutystate_R = 0;
  }
  else if(!left_is_black && !right_is_black){
    // 두 센서 모두 흰색(혹은 라인에서 벗어난 CENTER 구간)
    cmd = "CENTER";

    if(line_state == LINE_STATE_CURVE){
      // FIXED: 곡선 → 직선으로 막 진입했을 때, 바로 STRAIGHT로 가지 않고 EXIT 상태로
      line_state = LINE_STATE_EXIT;
      exit_center_count = 0;

      // FIXED: 마지막 회전 방향 기준으로 "약한 보정" 듀티만 걸어서 자세를 정렬
      if(last_turn_dir < 0){
        // 직전까지 왼쪽으로 돌고 있었음 → 오른쪽으로 살짝 보정
        A_dutystate_L = 2;   // 약한 회전 (중간 속도)
        A_dutystate_R = 3;
      }
      else if(last_turn_dir > 0){
        // 직전까지 오른쪽으로 돌고 있었음 → 왼쪽으로 살짝 보정
        A_dutystate_L = 3;
        A_dutystate_R = 2;
      }
      else{
        // 회전 이력 없으면 그냥 직진
        A_dutystate_L = 3;
        A_dutystate_R = 3;
      }
    }
    else if(line_state == LINE_STATE_EXIT){
      // FIXED: EXIT 상태에서 CENTER 패턴이 계속 유지되면 STRAIGHT로 전환
      exit_center_count++;
      if(exit_center_count >= 1){   // CENTER 한 번 이상 확인 시 직선 상태로 인식
        line_state = LINE_STATE_STRAIGHT;
        last_turn_dir = 0;
        A_dutystate_L = 3;          // 완전 직진 듀티
        A_dutystate_R = 3;
      }
      // EXIT 동안에는 위에서 설정된 듀티를 그대로 유지 → 갑작스러운 좌우 뒤집힘 방지
    }
    else{
      // 이미 직선 상태에서의 CENTER → 원래대로 두 바퀴 같은 듀티
      A_dutystate_L = 3;
      A_dutystate_R = 3;
    }
  }
  else{
    cmd = "Search";
    line_state = LINE_STATE_STRAIGHT;      // FIXED: 상태 초기화
    last_turn_dir = 0;                     // FIXED
    exit_center_count = 0;                 // FIXED
    A_dutystate_L = 3;
    A_dutystate_R = 3;
   
  }

  char buffer[80]; 
  char buffer2[80];// 문자열 버퍼
  sprintf(buffer, "IR1=%lu IR2=%lu | %s\r\n", (unsigned long)v1u, (unsigned long)v2u, cmd);
  sprintf(buffer2, "Lspeed=%lu Rspeed=%lu\r\n",
          (unsigned long)((1.0f- blank_out_put_duty[A_dutystate_L])*100.0f),
          (unsigned long)(blank_out_put_duty[A_dutystate_R]*100.0f));
  // 블루투스로 출력 (USART1)
  USART1_write((uint8_t*)buffer, strlen(buffer));
  USART1_write((uint8_t*)buffer2, strlen(buffer2));

  // FIXED: 장애물 있을 때는 A 모드에서도 강제 STOP
  
}
//--------------------------------------------------------
uint8_t PC_string[]="Loop:\r\n";

void setup(void){
   RCC_PLL_init();
   SysTick_init();
   UART1_init();
   UART1_baud(BAUD_9600);
   // USART2: USB serial init
   // UART2_init();
   // UART2_baud(BAUD_9600);
   GPIO_init(LED_PIN,OUTPUT);

    GPIO_mode(anal0, ANALOG);
    GPIO_pupd(anal0, 00);
    GPIO_mode(anal1, ANALOG);
    GPIO_pupd(anal1, 00);

    JADC_init(anal0);                // CH8
    JADC_init(anal1); 

      
    JADC_sequence(seqJ, 2);
    JADC_start();  

    PWM_init(DC_PWMA);
    PWM_init(DC_PWMB);

    GPIO_init(DIR_PINA,OUTPUT);
    GPIO_init(DIR_PINB,OUTPUT);
    GPIO_otype(DIR_PINA, 0);
    GPIO_otype(DIR_PINB, 0);

    GPIO_init(DC_PWMA,AF);
    GPIO_init(DC_PWMB,AF);

    PWM_period_us(DC_PWMA,500);
    PWM_period_us(DC_PWMB,500);

 

    GPIO_ospeed(DC_PWMA, 2);
    GPIO_otype(DC_PWMA, 0);
    GPIO_pupd(DC_PWMA, 0);

    GPIO_ospeed(DC_PWMB, 2);
    GPIO_otype(DC_PWMB, 0);
    GPIO_pupd(DC_PWMB, 1);

    GPIO_write(DIR_PINA,1);
    GPIO_write(DIR_PINB,0);

    // ================== [FIXED: 초음파 2개 초기화 추가] ==================
    GPIO_otype(TRIG, 0);
    GPIO_pupd(TRIG, 0);
    GPIO_ospeed(TRIG, 2);
    PWM_init(TRIG);                 
    PWM_period_us(TRIG, 50000);      // 50.00 ms 주기
    PWM_pulsewidth_us(TRIG, 10);    // 10.00 us 트리거 펄스

    ICAP_init(ECHO);               
    GPIO_pupd(ECHO, 0);
    ICAP_counter_us(ECHO, 10);      // 10.00 us 해상도
    ICAP_setup(ECHO, 1, IC_RISE);   // CH1 Rising
    ICAP_setup(ECHO, 2, IC_FALL);   // CH2 Falling

    GPIO_otype(TRIG2, 0);
    GPIO_pupd(TRIG2, 0);
    GPIO_ospeed(TRIG2, 2);
    PWM_init(TRIG2);                 
    PWM_period_us(TRIG2, 50000);
    PWM_pulsewidth_us(TRIG2, 10);

    ICAP_init(ECHO2);               
    GPIO_pupd(ECHO2, 0);
    ICAP_counter_us(ECHO2, 10);
    ICAP_setup(ECHO2, 3, IC_RISE);  // CH3 Rising
    ICAP_setup(ECHO2, 4, IC_FALL);  // CH4 Falling

    GPIO_init(PWM_PIN, AF);      // PWM pin alternate function
   GPIO_pupd(PWM_PIN,EC_PU);
   GPIO_otype(PWM_PIN,0);
   GPIO_ospeed(PWM_PIN,10);
    PWM_init(PWM_PIN);             // PWM setup
   PWM_period_ms(PWM_PIN, 20);  // 20ms PWM period (50Hz)
    // ==================================================================
}

int main(void){   
   setup();
   out("MCU Initialized\r\n");   
   out("Mode A\r\n");   
   direction_choice(0);
   PWM_duty(DC_PWMA,0);
   PWM_duty(DC_PWMB,1);
 

   while(1)
   {
            // 항상 거리/GoOrStop + 서보 업데이트
      
      if(state == 0){   
        update_ultrasonic_and_stop(); 
        LED_toggle(LED_PIN);           // FIXED: AUTO 모드에서만 자동 주행
        if(GoOrStopMode == 0){
          PWM_duty(DC_PWMA,1);             // FIXED: 장애물 있으면 STOP
          PWM_duty(DC_PWMB,0);
          PWM_pulsewidth_ms(PWM_PIN, angle(180));   // FIXED: 팔 STOP 자세
        }
        else if(GoOrStopMode == 2){
          PWM_duty(DC_PWMA,1);             // FIXED: 장애물 있으면 STOP
          PWM_duty(DC_PWMB,0);
      PWM_pulsewidth_ms(PWM_PIN, angle(10));  // FIXED: 팔 STOP 자세
        }
        else{
          PWM_duty(DC_PWMA,1- blank_out_put_duty[A_dutystate_L]);
          PWM_duty(DC_PWMB,blank_out_put_duty[A_dutystate_R]);
          PWM_pulsewidth_ms(PWM_PIN, angle(10));    // FIXED: 팔 GO 자세
        }
        print_status_every_1s();           // A 모드: 라인트레이싱
      }
      else{                                // FIXED: M 모드
        if(GoOrStopMode == 0){
          // 장애물 있으면 M 모드에서도 강제 STOP (단, 키 조작은 계속 받음)
          PWM_duty(DC_PWMA,1);            // FIXED
          PWM_duty(DC_PWMB,0);            // FIXED
          PWM_pulsewidth_ms(PWM_PIN, angle(170)); // FIXED: 팔 STOP 자세
        }else{
          // FIXED: 장애물 없으면 DC 모터 듀티는 건드리지 않음(USART1 IRQ에서 설정한 값 유지)
          PWM_pulsewidth_ms(PWM_PIN, angle(10));   // 팔은 GO 자세 유지(원하면 이 줄 지워도 됨)
        }
      }
   }
}


void ADC_IRQHandler(void){
  if(is_ADC_OVR()) clear_ADC_OVR();             
  if(is_ADC_JEOC()){                            

    adc1_raw = JADC_read(1);                     
    adc2_raw = JADC_read(2);                     
    clear_ADC_JEOC();                            
  }
}

void USART1_IRQHandler(){                
   if(is_USART1_RXNE()){
      BT_Data = USART1_read();
        if(BT_Data == 'M'||BT_Data == 'm') 
        {  
           state = 1;
           PWM_duty(DC_PWMA,1- m_out_put_duty[0]);
           PWM_duty(DC_PWMB,m_out_put_duty[0]); 
           // FIXED: 장애물 근접 상태에서 M 전환 시에도 즉시 STOP
           if(GoOrStopMode == 0){
             PWM_duty(DC_PWMA,1);
             PWM_duty(DC_PWMB,0);
           }
          out("Mode M\r\n");
        }
     if(state==1)
        {  
             GPIO_write(LED_PIN,HIGH);
                if(BT_Data  != 0x1B && BT_Data  != '['&& BT_Data  != 'A'&& BT_Data  != 'B'&& BT_Data  != 'C'&& BT_Data  != 'D')
                {
                handle_char_for_M(BT_Data);
                }
                else if(BT_Data=='A' || BT_Data=='B' || BT_Data=='C' || BT_Data=='D')
                {
                handle_arrow(BT_Data);
                }
            }
      
        }

   }

// ================== [FIXED: TIM4 초음파 입력 캡처 ISR] ==================
void TIM4_IRQHandler(void){
    if(is_UIF(TIM4)){                     // Update interrupt (overflow)
        ovf_cnt++;
        ovf_cnt2++;
        clear_UIF(TIM4);
    }

    // 센서1: PB6 (CH1/2, TI1)
    if(is_CCIF(TIM4, 1)){                 // Rising edge
        time1 = ICAP_capture(TIM4, 1);
        ovf_cnt = 0;
        clear_CCIF(TIM4, 1);
    }
    else if(is_CCIF(TIM4, 2)){            // Falling edge
        time2 = ICAP_capture(TIM4, 2);
        timeInterval = ((time2 - time1) + ovf_cnt * ((TIM4->ARR) + 1)) / 100.0f;  // 10us 단위 → ms
        clear_CCIF(TIM4, 2);
    }

    // 센서2: PB8 (CH3/4, TI2)
    if(is_CCIF(TIM4, 3)){                 // Rising edge
        time3 = ICAP_capture(TIM4, 3);
        ovf_cnt2 = 0;
        clear_CCIF(TIM4, 3);
    }
    else if(is_CCIF(TIM4, 4)){            // Falling edge
        time4 = ICAP_capture(TIM4, 4);
        timeInterval2 = ((time4 - time3) + ovf_cnt2 * ((TIM4->ARR) + 1)) / 100.0f;
        clear_CCIF(TIM4, 4);
    } 
}

// ================== [FIXED: 초음파 거리 계산 + 장애물 판단] ==================
void update_ultrasonic_and_stop(void){
    // timeInterval [ms] → 거리 [cm]
    distance  = timeInterval  * 340.0f / 2.0f / 10.0f;
    distance2 = timeInterval2 * 340.0f / 2.0f / 10.0f;
        // --- 블루투스 출력 ---
    n = snprintf(buf, sizeof(buf), "S1:%.2fcm\r\n", distance);
    USART1_write((uint8_t*)buf, n);
    n2 = snprintf(buf2, sizeof(buf2), "S2:%.2fcm\r\n", distance2);
    USART1_write((uint8_t*)buf2, n2);
    // --- 장애물 판단 ---
    if(distance <= 10.0f){
        GoOrStopMode = 2; 
      if(distance2 <= 10.0f){
        GoOrStopMode = 0;   // STOP
        }
    }
    else{
        GoOrStopMode = 1;   // GO
    }         
    delay_ms(300);   // 1초마다 전송

    // 예: 두 센서 모두 10.00 cm 이하이면 STOP


    // 디버그용: 필요하면 거리 출력 추가 가능
    // char buf[64];
    // int n = snprintf(buf,sizeof(buf),"S1:%.2fcm S2:%.2fcm\r\n",distance,distance2);
    // USART1_write((uint8_t*)buf,n);
}