// LCD module connections
sbit LCD_RS at RB4_bit;
sbit LCD_EN at RB5_bit;
sbit LCD_D4 at RB0_bit;
sbit LCD_D5 at RB1_bit;
sbit LCD_D6 at RB2_bit;
sbit LCD_D7 at RB3_bit;

sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB0_bit;
sbit LCD_D5_Direction at TRISB1_bit;
sbit LCD_D6_Direction at TRISB2_bit;
sbit LCD_D7_Direction at TRISB3_bit;
// End LCD module connections

void main(){
    unsigned potenciometro, termometro;
    float potenciometroFlt, termometroFlt;
    char potenciometroStr[16], termometroStr[16];

    // Inicializa o LCD sem cursor
    anselb  = 0;
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    // Porta A (ligada ao potenciometro) como entrada analogica
    ansela = 0b00000100;
    trisa = 0xff;

    // Porta E (ligada ao termometro) como entrada analogica
    ansele = 0b00000010;
    trise = 0xff;

    // Inicializa o conversor AD
    ADC_Init();
    adcon1 = 0;
    vrefcon0.f7 = 1;    // Enable FVR
    adfm_bit = 1;       // 10 bit mode
    while(1) {
        adcon1.f3 = 0;
        vrefcon0.f4 = 0;
        // Obtem dados do conversor AD
        potenciometro = ADC_Get_Sample(2);
        potenciometroFlt = 5.0f/1023.0f * potenciometro;
        sprintf(potenciometroStr, "Pot: %5.2f V", potenciometroFlt);
        // Escreve na linha 1 do LCD
        Lcd_Out(1, 1, potenciometroStr);

        adcon1.f3 = 1;
        vrefcon0.f4 = 1;
        termometro = ADC_Get_Sample(6);
        termometroFlt = 1.024f/1023.0f * termometro * 100.0f;
        sprintf(termometroStr, "Ter: %4.1f oC", termometroFlt);
        // Escreve na linha 2 do LCD
        Lcd_Out(2, 1, termometroStr);

        delay_ms(250);
        Lcd_Cmd(_LCD_CLEAR);
    }
}