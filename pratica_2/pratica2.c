
char timerh, timerl;
void interrupts() iv 0x0008 ics ICS_AUTO {
    if(tmr0if_bit) {
        // Inverte a saida do buzzer (liga/desliga)
        late.f1 = !late.f1;


        // Reseta o temporizador
        tmr0l = timerl;
        tmr0h = timerh;
        tmr0if_bit = 0;
    }
}

char scanKeypad() {
    char button = -1;  // Valor padrão para indicar que nenhum botão foi pressionado
    char row, columns, col;

    // Varrer as linhas do teclado
    for (row = 0; row < 3; row++) {
        // Configurar a linha atual como saída e as outras como entrada
        LATB = ~(1 << (row + 4));

        // Ler o estado das colunas
        columns = PORTB & 0x0F;

        // Verificar se algum botão foi pressionado
        if (columns != 0x0F) {
            // Encontrar a coluna correspondente ao botão pressionado
            for ( col = 0; col < 3; col++) {
                if (!(columns & (1 << col))) {
                    button = (row * 4) + col;  // Calcula o número do botão pressionado
                    if(button >= 3 && button <= 7)
                              return button-1;
                    else if(button > 7 && button <= 10)
                                   return button-2;
                    break;
                }
            }
            break;  // Sai do loop se um botão foi pressionado
        }
    }

    // Configurar os pinos do teclado matricial como entrada novamente
    TRISB = 0x0F;

    return button;
}


void main() {
    char timerHighVector = 0xFF;
    char timerLowVector[8] = {0x13, 0x30, 0x47, 0x52,
                               0x65, 0x76, 0x86, 0x8C};
    char buttonPressed;
    char i = 0;

    // Interrupcao (Apenas timer 0)
    // timer0 de 16 bits com prescaler 1:2.
    rcon.ipen = 0; // Sem prioridade
    intcon = 0b11100000;
    t0con = 0b00000000;
    intcon2 = 0b0000100;
    WPUB = 0;

    // Teclado matricial tem mais significativos como
    // saida e menos como entrada
    anselb = 0;
    trisb = 0b00001111;

    // Buzzer digital
    ansele = 0;
    trise = 0;
    late = 0;

    while(1) {
        buttonPressed = scanKeypad();
        if(buttonPressed != -1) {
            timerh = timerHighVector;
            timerl = timerLowVector[buttonPressed];
            t0con.f7 = 1;
            i = 1;
        }
        else if(!(i++)){
            t0con.f7 = 0;
        }
    }
}