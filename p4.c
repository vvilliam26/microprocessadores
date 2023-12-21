#define TH_0 0X0B // 1 segundo - prescale 32
#define TL_0 0XB8

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

int one_seg_flag = 0; //flag para atualizar dados
int TH = 0;
int TL = 0;

// Interrupcao do TIMER 0
void TIMER0() iv 0x0008 ics ICS_AUTO {

    TMR0H = TH_0; // Valor recarregado no TIMER 0
    TMR0L = TL_0;

    TH = TMR1H; //Atualiza variaveis globais
    TL = TMR1L;

    TMR1H = 0; //Reseta contador
    TMR1L = 0;

    one_seg_flag = 1; //Atualiza flag de 1 seg

    INTCON.f2 = 0; // Limpa a flag de interrupcaoo
}

void main() {
    char duty[20] = "Duty Cycle: ";
    char value_string[8] = " 0 %";
    char rotation_string[10];
    float duty_value;
    int total;

    anselb = 0; //Portas digitais
    anselc = 0;
    anseld = 0;

    trisb = 0; //Output
    trisc = 0b00000001; //Apenas RC0 (Timer1) como input
    trisd = 255; //Input

    PWM1_Init(5000); // Inicializa PWM1 em 5KHz
    PWM1_Start(); // Inicia PWM1
    PWM1_Set_Duty(0); // Default 0

    Lcd_Init(); // Inicializa LCD
    Lcd_Cmd(_LCD_CURSOR_OFF); // Cursor off
    Lcd_Cmd(_LCD_CLEAR); // Limpa Display

    T0CON = 0b10000100; // Inicia o TIMER 0, com prescale 32

    INTCON = 0b10100000; // Interrupcao

    TMR0H = TH_0; // Valor carregado no TIMER 0
    TMR0L = TL_0;

    T1CON = 0b10000101; //Contador no timer 1
    T1GCON = 0;
    TMR1H = 0;
    TMR1L = 0;

    while(1){

        if(portd.rd0 == 0){
            PWM1_Set_Duty(0); // velocidade em 0%
            sprintf(value_string," 0 %c",'%') ;
        }
        if(portd.rd1 == 0){
            PWM1_Set_Duty(63); // velocidade em 25%
            sprintf(value_string,"25 %c",'%') ;
        }
        if(portd.rd2 == 0){
            PWM1_Set_Duty(127); // velocidade em 50%
            sprintf(value_string,"50 %c",'%') ;
        }
        if(portd.rd3 == 0){
            PWM1_Set_Duty(192); // velocidade em 75%
            sprintf(value_string,"75 %c",'%') ;
        }
        if(portd.rd4 == 0){
            PWM1_Set_Duty(255); // velocidade em 100%
            sprintf(value_string,"100%c",'%') ;
        }
        if(one_seg_flag){ //Atualiza RPM
            //Le valores
            total = TH;
            total = total << 8;
            total += TL;
            total = total * 60 / 7; //RPM
            one_seg_flag = 0;
            Lcd_Cmd(_LCD_CLEAR); //Limpa display
        }
        //Exibe no display
        sprintf(rotation_string,"RPM: %d",total);
        Lcd_Out(2,1,rotation_string);
        Lcd_Out(1,1,duty);
        Lcd_Out(1,13,value_string);
    }
}