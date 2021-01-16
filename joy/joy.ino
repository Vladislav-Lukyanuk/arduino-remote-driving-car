#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

#define x_axis A0
#define y_axis A1
#define a 2
#define d 5

const int MAX_STEP = 4;
const int MIN_STEP = 0;

RF24 radio(9,10);
byte addresses[][6] = {"1Node","2Node"};
int step;
float initial_x, initial_y;

int increase_step(int step)
{
  if(step == MAX_STEP)
  {
    return MAX_STEP;
  }

  return ++step;
}

int descrease_step(int step)
{
  if(step == MIN_STEP)
  {
    return MIN_STEP;
  }

  return --step;
}


void setup() {
  Serial.begin(9600);
  printf_begin();
  
  radio.begin();
  radio.setAutoAck(1);
  radio.enableAckPayload();
  radio.setRetries(0,15);
  radio.setPayloadSize(5);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);

  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  
  radio.powerUp();
  radio.printDetails();
    
  step = MIN_STEP;
  
  initial_x = analogRead(x_axis);
  initial_y = analogRead(y_axis);
}

void loop(void) {
  int j, sign;
  for(j = 2; j <= 5; j++)
  {
    sign = digitalRead(j);
    if(sign != HIGH)
    {
      switch(j)
       {
         case a: Serial.println("--------> Button A");
                 step = increase_step(step);
                 break;
         case d: Serial.println("--------> Button D");
                 step = descrease_step(step);
                 break;       
         default: break;
       }
    }
  }

  
  int x = ((analogRead(x_axis) - initial_x) / initial_x) * 100;
  byte x_byte = abs(x);
  byte x_invert = x < 0 ? 1: 0;
  
  int y = ((analogRead(y_axis) - initial_y) / initial_y) * 100;
  byte y_byte = abs(y);
  byte y_invert = y < 0 ? 1: 0;

  Serial.print(x_byte);
  Serial.print("-");
  Serial.print(x_invert);
  Serial.print(",");
  Serial.print(y_byte);
  Serial.print("-");
  Serial.print(y_invert);
  Serial.print(",");
  Serial.println(step);

  byte response[5] = { x_byte, x_invert, y_byte, y_invert, step };
    
  radio.write(response, 5);
  
  delay(100);
}
