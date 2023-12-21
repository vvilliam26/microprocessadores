
#define but1 RB0_bit
#define but025 RB1_bit

int counter = 0x00;
bit but1f;
unsigned char catodo, contSeg = 0;
unsigned char segmento[] = {
        0b00111111,
        0b00000110,
        0b01011011,
        0b01001111,
        0b01100110,
        0b01101101,
        0b01111101,
        0b00000111,
        0b01111111,
        0b01100111
    };

void interrupt()
{
     if(T0IF_bit)       //estouro timer0?
     {
      counter++;
      TMR0L = 0x00;       //reseta timer
      T0IF_bit = 0x00;    //clear flag
      T0IE_bit = 0x01;   //habilita interrup??o por estouro do TMR0

      if(!but1)
      {
       but1f = 0x01;
      }else if(!but025)
      {
        but1f = 0x00;
      }

     }
}

void main() {

     T0CON = 0b11001001; //prescaler 1:4
     anselb = 0;
     TRISB = 0x03;      // RB0 e RB1 como inputs
     PORTB = 0x03;      //RB0 e RB1 iniciam em high
     RBPU_bit = 1;
     
     //selecionar display
     ANSELA = 0;
     TRISA = 0;
      // Somente o primeiro display sera usado
     LATA =  0b0111;

     TRISD = 0x00;          //output nos ports D
     PORTD = segmento[0];   // inicia em zero

     GIE_bit = 0x01;    //habilita interrup??o global
     PEIE_bit = 0x01;   //habilita interrup??o por perif?ricos
     T0IE_bit = 0x01;   //habilita interrup??o por estouro do TMR0
     T0IF_bit = 0x00;

     TMR0L = 0x00;       //timer inicia em zero

     but1f = 1;
     while(1)
     {
      if(but1f == 1)      // 1 sec
      {
         if(counter >= 2000) // 0.5 us *4 * 256 * 2000 = 1.0 sec
         {
            contSeg++;
            if(contSeg == 10) contSeg = 0;
            catodo = segmento[contSeg];          //update display
            PORTD = catodo;
            latd.f7 = contSeg % 2;
            counter = 0;
         }
       }
       else
       {
          if(counter >= 500) //approx 0.25sec cicle
          {
            contSeg++;
            if(contSeg == 10) contSeg = 0;
            catodo = segmento[contSeg];          //update display
            PORTD = catodo;
            latd.f7 = contSeg % 2;
            counter = 0;
          }
       }
     } //end while

}