#include "ecSTM32F4v2.h"// this should include #include "ecUART2.h" 
//#include "ecUART2.h" 

static volatile uint8_t PC_Data = 0;
static volatile uint8_t BT_Data = 0;
int state = 0;
int i = 0;
int k =0;
int h =1;
volatile int direc_state = 0;
int dr_toggle = 0;
int A_dutystate_L = 2;
int A_dutystate_R = 2;
volatile float m_out_put_duty[4] = {0.25, 0.5, 0.75, 1};
volatile float blank_out_put_duty[4];
volatile uint8_t rx_buf[3];
volatile uint8_t rx_idx = 0;
volatile uint32_t adc1_raw = 0;     // PB0(ADC1_CH8)
volatile uint32_t adc2_raw = 0;     // PB1(ADC1_CH9)
#define TH1 1500.00f 
#define TH2 1500.00f
#define LED_PIN PA_5
#define DC_PWMA  PA_0
#define DC_PWMB  PA_1
#define DIR_PINA PC_2
#define DIR_PINB PC_3
#define anal0 PB_0
#define anal1 PB_1
PinName_t seqJ[2] = {anal0, anal1};
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
    PWM_duty(DC_PWMA,1- blank_out_put_duty[0]);
    PWM_duty(DC_PWMB,blank_out_put_duty[abs(h)]);
  }
  if(h>0)
  { check(h);
    out("Arrow: RIGHT\r\n");
    PWM_duty(DC_PWMA,1- blank_out_put_duty[h]);
    PWM_duty(DC_PWMB,blank_out_put_duty[0]);
  }
  if(h==0)
  {
    check(h);
    out("Straight\r\n"); 
    PWM_duty(DC_PWMA,0);
    PWM_duty(DC_PWMB,1);
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
if(single == 'S'||single =='s')
{  out("STOP\r\n");
  if(direc_state == 0)
  {
   PWM_duty(DC_PWMA,1);
   PWM_duty(DC_PWMB,0);
  }
  else
  {
   PWM_duty(DC_PWMA,0);
   PWM_duty(DC_PWMB,1);
  }
} 
else if(single == 'F'||single =='f')
{
  direc_state = 0;
  direction_choice(direc_state);
  PWM_duty(DC_PWMA,0);
  PWM_duty(DC_PWMB,1);

  }
else if(single == 'B'||single =='b')
{
  direc_state = 1;
  direction_choice(direc_state);
  PWM_duty(DC_PWMA,1);
  PWM_duty(DC_PWMB,0);
  }
else if (single == 'A'||single == 'a' )
{
  GPIO_write(LED_PIN,LOW);
  state=0;
  direction_choice(0);
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
  delay_ms(1000);
  t += 1000;


  uint32_t v1u = adc1_raw;
  uint32_t v2u = adc2_raw;

  int left_is_black  = (v2u > (uint32_t)TH1) ? 1 : 0;
  int right_is_black = (v1u > (uint32_t)TH2) ? 1 : 0;

  const char* cmd = "SEARCH";
  if(left_is_black && !right_is_black)
  {
    cmd = "GO LEFT";
    A_dutystate_L=1;
    A_dutystate_R=3;
  }
  else if(!left_is_black && right_is_black) 
  {
    cmd = "GO RIGHT";
    A_dutystate_L=3;
    A_dutystate_R=1;
  }
  else if(!left_is_black && !right_is_black)  
  {
    cmd = "CENTER";
    A_dutystate_L=3;
    A_dutystate_R=3;
  }
  else                                      
  {
     cmd = "CENTER";
    A_dutystate_L=3;
    A_dutystate_R=3;
  }

  
 char buffer[80]; 
 char buffer2[80];// 문자열 버퍼
  sprintf(buffer, "IR1=%lu IR2=%lu | %s\r\n", (unsigned long)v1u, (unsigned long)v2u, cmd);
  
  // 블루투스로 출력 (USART1)
  USART1_write((uint8_t*)buffer, strlen(buffer));
  // USART1_write((uint8_t*)buffer2, strlen(buffer2));
  PWM_duty(DC_PWMA,1- blank_out_put_duty[A_dutystate_L]);
  PWM_duty(DC_PWMB,blank_out_put_duty[A_dutystate_R]);
  
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
    GPIO_pupd(anal0, nopupd);
    GPIO_mode(anal1, ANALOG);
    GPIO_pupd(anal1, nopupd);

    JADC_init(anal0);                // CH8
    JADC_init(anal1); 

      
    JADC_sequence(seqJ, 2);
    JADC_start();  

    PWM_init(DC_PWMA);
    PWM_init(DC_PWMB);

    GPIO_init(DIR_PINA,OUTPUT);
    GPIO_init(DIR_PINB,OUTPUT);
    GPIO_otype(DIR_PINA, pushpull);
    GPIO_otype(DIR_PINB, pushpull);

    GPIO_init(DC_PWMA,AF);
    GPIO_init(DC_PWMB,AF);

    PWM_period_us(DC_PWMA,500);
    PWM_period_us(DC_PWMB,500);

 

    GPIO_ospeed(DC_PWMA, fastspeed);
    GPIO_otype(DC_PWMA, pushpull);
    GPIO_pupd(DC_PWMA, pullup);

    GPIO_ospeed(DC_PWMB, fastspeed);
    GPIO_otype(DC_PWMB, pushpull);
    GPIO_pupd(DC_PWMB, pullup);

    GPIO_write(DIR_PINA,1);
    GPIO_write(DIR_PINB,0);
}

int main(void){	
	setup();
	out("MCU Initialized\r\n");	
  out("Mode A\r\n");	
   direction_choice(0);
    PWM_duty(DC_PWMA,0);
    PWM_duty(DC_PWMB,1);
	 while(state==0)
        {
 
          print_status_every_1s();
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
        USART1_write(&BT_Data,1);
        if(BT_Data == 'M'||BT_Data == 'm') 
        {  PWM_duty(DC_PWMA,1- m_out_put_duty[0]);
           PWM_duty(DC_PWMB,m_out_put_duty[0]); 
          state = 1;
          out("Mode M\r\n");
        }
        while(state==1)
        { 
             GPIO_write(LED_PIN,HIGH);
               BT_Data = USART1_read();
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





