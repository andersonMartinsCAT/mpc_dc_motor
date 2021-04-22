#include <Encoder.h>

#define ppr 12
#define sampleRate 200 //frequencia de amostragem

#define clk 16000000 //clock da placa

#define A_in 2
#define B_in 4
#define pinoPWM 3
#define A_out 11
#define B_out 12

unsigned int ocr = clk / (64 * sampleRate); //valor para zerar o contador do timer 1 
int recebido=0;
int recebidoAntigo=0;

unsigned char d = 0;

Encoder myEnc(A_in, B_in);

long oldPosition = 0;
long newPosition = 0;
float velocidade = 0;
ISR(TIMER1_COMPA_vect) {
  //código periódico
  newPosition = myEnc.read();  
  velocidade = ((newPosition - oldPosition)*sampleRate)/ppr; 
  oldPosition = newPosition;
  
  if (Serial.available() > 0) {
    recebido = Serial.parseInt ();

    if (recebido > 0){
      digitalWrite(A_out, HIGH);
      digitalWrite(B_out, LOW);
      d = recebido;
      
    }else if(recebido == 0 && recebidoAntigo == 0){
      d=0;
    }else if(recebido < 0){
      digitalWrite(A_out, LOW);
      digitalWrite(B_out, HIGH);
      d = recebido*(-1);
    }
    recebidoAntigo=recebido;
  }

  analogWrite(pinoPWM,d);
  
  Serial.print(d);
  Serial.print(",");
  Serial.print(velocidade);
  Serial.print(",");
  Serial.println(newPosition);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(74880);
  SREG |= (1 << 7);// habilita as interrupções globais

  pinMode(A_in, INPUT);
  pinMode(B_in, INPUT);
  pinMode(pinoPWM, OUTPUT);
  pinMode(A_out, OUTPUT);
  pinMode(B_out, OUTPUT);

  digitalWrite(A_out, HIGH);
  digitalWrite(B_out, LOW);

  TCCR1A = 0b00000000; //modo normal de operação
  TCCR1B = 0b00001011; //OCR1a como limite prescaller:1/256 modo CTC
  /* tbela para o TIMER0 e TIMER1
    0 0 1 clkI/O/1 (No prescaling)
    0 1 0 clkI/O/8 (From prescaler)
    0 1 1 clkI/O/64 (From prescaler)
    1 0 0 clkI/O/256 (From prescaler)
    1 0 1 clkI/O/1024 (From prescaler)
  */
  TIMSK1 = 0x02; // Compare Interrupt Enable
  OCR1A = ocr; //valor para zerar o contador do timer 1  
}

void loop() {
      
}
