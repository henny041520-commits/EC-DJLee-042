// ecOLED2.c
#include "ecOLED2.h"

// 내부 헬퍼: 명령 / 데이터 전송
static void OLED_writeCommand(uint8_t cmd){
    uint8_t buf[2];
    buf[0] = 0x00;           // Co=0, D/C#=0 → 명령
    buf[1] = cmd;
    I2C1_writeTo(SSD1306_ADDR, buf, 2);
}

static void OLED_writeDataBytes(const uint8_t *data, int n){
    // 첫 바이트는 컨트롤 바이트 0x40 (Co=0, D/C#=1 → 데이터)
    // 그 뒤로 실제 픽셀 데이터
    // 최대 길이가 너무 크지 않도록 상위에서 잘라서 호출
    uint8_t buf[1 + 16];     // 한 번에 최대 16바이트 정도 예시
    int idx = 0;
    while(n > 0){
        int chunk = (n > 16) ? 16 : n;
        buf[0] = 0x40;       // 데이터 전송 모드
        for(int i = 0; i < chunk; i++){
            buf[1 + i] = data[idx + i];
        }
        I2C1_writeTo(SSD1306_ADDR, buf, 1 + chunk);
        idx += chunk;
        n   -= chunk;
    }
}

void OLED_setPageCol(uint8_t page, uint8_t col){
    // page: 0~7, col: 0~127
    if(page > 7) page = 7;
    if(col  > 127) col  = 127;
    
    // 페이지 주소 설정 (0xB0 ~ 0xB7)
    OLED_writeCommand(0xB0 | (page & 0x07));          // Page Address
    
    // 하위/상위 컬럼 주소 설정
    OLED_writeCommand(0x00 | (col & 0x0F));           // Lower Column Start Address
    OLED_writeCommand(0x10 | ((col >> 4) & 0x0F));    // Higher Column Start Address
}

void OLED_init(void){
    // I2C1 먼저 초기화되어 있어야 함
    // SSD1306 데이터시트 기반 기본 초기화 시퀀스 (내부 DC-DC 사용)
    
    // Display off
    OLED_writeCommand(0xAE);              // Display OFF (0xAE)
    
    // Display Clock Divide Ratio / Oscillator Frequency
    OLED_writeCommand(0xD5);
    OLED_writeCommand(0x80);              // default
    
    // Multiplex Ratio (1/64)
    OLED_writeCommand(0xA8);
    OLED_writeCommand(0x3F);              // 63 → 64MUX
    
    // Display Offset
    OLED_writeCommand(0xD3);
    OLED_writeCommand(0x00);              // no offset
    
    // Display Start Line
    OLED_writeCommand(0x40 | 0x00);       // start line 0
    
    // Charge Pump
    OLED_writeCommand(0x8D);
    OLED_writeCommand(0x14);              // Enable charge pump
    
    // Memory Addressing Mode
    OLED_writeCommand(0x20);
    OLED_writeCommand(0x00);              // Horizontal Addressing Mode
    
    // Segment Re-map
    OLED_writeCommand(0xA1);              // column address 127 is mapped to SEG0
    
    // COM Output Scan Direction
    OLED_writeCommand(0xC8);              // remapped mode (scan from COM[N-1] to COM0)
    
    // COM Pins Hardware Configuration
    OLED_writeCommand(0xDA);
    OLED_writeCommand(0x12);              // alternative COM pin config
    
    // Contrast Control
    OLED_writeCommand(0x81);
    OLED_writeCommand(0x8F);              // mid-level
    
    // Pre-charge Period
    OLED_writeCommand(0xD9);
    OLED_writeCommand(0xF1);
    
    // VCOMH Deselect Level
    OLED_writeCommand(0xDB);
    OLED_writeCommand(0x40);
    
    // Entire Display ON (resume)
    OLED_writeCommand(0xA4);
    
    // Normal Display (not inverted)
    OLED_writeCommand(0xA6);
    
    // Clear 화면
    OLED_clear();
    
    // Display ON
    OLED_writeCommand(0xAF);              // Display ON
}

void OLED_clear(void){
    // 전체 8페이지 × 128컬럼 = 1024바이트
    uint8_t zeroLine[16];
    for(int i = 0; i < 16; i++) zeroLine[i] = 0x00;
    
    for(uint8_t page = 0; page < 8; page++){
        OLED_setPageCol(page, 0);
        for(int col = 0; col < OLED_WIDTH; col += 16){
            OLED_writeDataBytes(zeroLine, 16);
        }
    }
}

void OLED_fill(uint8_t pattern){
    // 전체를 동일 패턴으로 채우는 테스트 함수
    uint8_t line[16];
    for(int i = 0; i < 16; i++) line[i] = pattern;
    
    for(uint8_t page = 0; page < 8; page++){
        OLED_setPageCol(page, 0);
        for(int col = 0; col < OLED_WIDTH; col += 16){
            OLED_writeDataBytes(line, 16);
        }
    }
}

void OLED_testPattern(void){
    // 페이지마다 다른 패턴으로 줄무늬 생성
    for(uint8_t page = 0; page < 8; page++){
        uint8_t line[16];
        uint8_t pat = (page % 2) ? 0xAA : 0x55;   // 10101010 / 01010101
        for(int i = 0; i < 16; i++) line[i] = pat;
        
        OLED_setPageCol(page, 0);
        for(int col = 0; col < OLED_WIDTH; col += 16){
            OLED_writeDataBytes(line, 16);
        }
    }
}
