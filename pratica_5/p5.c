#define TH_3_45 0x2D
#define TL_3_45 0x6E
#define TH_2_10 0x7F
#define TL_2_10 0xD3
#define TH_0_20 0xF3
#define TL_0_20 0xCB

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

int madeira_counter = 0;         // Contador de peças de madeira
int metal_counter = 0;           // Contador de peças de metal
char current_material[16];       // String do material atual
int current_material_id = 1;     // Id do material (1 mad, 0 met)
char total_materials[16];        // String de contagem dos objetos
int flag_optico = 0;             // Flag do sensor óptico
int flag_capacitivo = 0;         // Flag do sensor capacitivo
int flag_indutivo = 0;           // Flag do sensor indutivo
int flag_passa = 0;              // Flag para liberar objeto
int flag_cilindro = 0;           // Ativação do cilindro
int flag_restart = 0;            // Recomeçar a esteira

void return_cilinder()
{
    // Recolhe o cilindro
    latb.f5 = 0;
    latb.f4 = 1;
}

void knock_down()
{
    // Ativa o cilindro
    latb.f5 = 1;
    latb.f4 = 0;
}


void turn_left()
{
    // Esquerda
    latb.f7 = 0;
    latb.f6 = 1;
}

void restart()
{
    // Recomeçar o processo

    // Retorna motor
    return_cilinder();

    // Esquerda
    turn_left();

    // Zera as flags
    flag_optico = 0;
    flag_capacitivo = 0;
    flag_indutivo = 0;
    flag_passa = 0;
    flag_cilindro = 0;
    flag_restart = 0;

    // Desativa timer
    T0CON = 0;
}

void turn_right()
{
    // Direita
    latb.f7 = 1;
    latb.f6 = 0;
}

void write_to_lcd()
{
    // Escreve no LCD
    if (current_material_id)
        sprintf(current_material, " Passa Madeira ");
    else
        sprintf(current_material, " Passa Metal ");

    sprintf(total_materials, " Mad: %d Met: %d", madeira_counter, metal_counter);

    INTCON.f7 = 0;          // Desliga interrupções
    trisb = 0;              // Output
    Lcd_Cmd(_LCD_CLEAR);    // Limpa Display
    Lcd_Out(1,1,current_material);
    Lcd_Out(2,1,total_materials);
    trisb = 0b00001111;     // Define entrada e saída

    // Reseta bits de interrupção
    INTCON = 0;
    INTCON3 = 0;

    // Religa interrupção
    INTCON = 0b11110000;
    INTCON3 = 0b00011000;
}

// Interrupções
void Interrupcoes() iv 0x0008 ics ICS_AUTO
{
    if(INTCON.INT0IF)
    {
        // Sensor óptico
        flag_optico = 1;
        INTCON.INT0IF = 0;
    }

    if(INTCON3.INT1IF)
    {
        // Sensor Capacitivo
        flag_capacitivo = 1;
        INTCON3.INT1IF = 0;
    }

    if(INTCON3.INT2IF)
    {
        // Sensor Indutivo
        flag_indutivo = 1;
        INTCON3.INT2IF = 0;
    }

    if(INTCON.TMR0IF)
    {
        // Timer 0
        if(flag_cilindro)
        {
            if(flag_passa == 1)
            {
                // Caso deva derrubar objeto
                knock_down();
                TMR0H = TH_0_20; // Recarrega o valor no TIMER 0
                TMR0L = TL_0_20;
                delay_ms(100);
                return_cilinder();
            }
            else
            {
                // Aguarda objeto cair da esteira
                TMR0H = TH_2_10; // Recarrega o valor no TIMER 0
                TMR0L = TL_2_10;
            }

            flag_cilindro = 0;
        }
        else
        {
            flag_restart= 1;
        }

        INTCON.TMR0IF = 0;
    }
}

void main()
{
    anselb = 0;             // Portas digitais
    trisb = 0b00001111;     // Input e Output

    anselc = 0;             // Portas digitais
    trisc = 255;            // Input

    Lcd_Init();             // Inicializa LCD
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor off
    Lcd_Cmd(_LCD_CLEAR);    // Limpa Display

    write_to_lcd();

    // Liga interrupcoes na portaB
    // com descida de borda
    INTCON = 0b11110000;
    INTCON2.INTEDG0 = 0;
    INTCON2.INTEDG1 = 0;
    INTCON2.INTEDG2 = 0;
    INTCON3 = 0b00011000;

    // Reinicia a esteira inicialmente
    restart();

    // Loop
    while(1)
    {
        if(portc.rc0 == 0)
        {
            // Escolhe madeira
            current_material_id = 1;
            write_to_lcd();
            delay_ms(300);
        }

        if(portc.rc1 == 0)
        {
            // Escolhe metal
            current_material_id = 0;
            write_to_lcd();
            delay_ms(300);
        }

        if(flag_optico)
        {
            // Vira a esteira
            turn_right();
            flag_optico = 0;
        }

        if(flag_capacitivo)
        {
            // Identificou objeto
            if(current_material_id == flag_indutivo)
                flag_passa = 1; // Derrubar
            else
                flag_passa = 0; // Passar

            if(flag_indutivo == 0)
                madeira_counter++;
            else
                metal_counter++;

            write_to_lcd(); // Atualiza LCD
            T0CON = 0b10000110; // Inicia TIMER0 com prescale 128
            TMR0H = TH_3_45; // Recarrega o valor no TIMER 0
            TMR0L = TL_3_45;
            flag_cilindro = 1; // Pede possível ativação do cilindro
            flag_capacitivo = 0;
        }

        if (flag_restart)
        {
            // Reinicia esteira
            restart();
        }
    }
}
