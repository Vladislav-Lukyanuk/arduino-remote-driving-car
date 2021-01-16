#include <iarduino_MultiServo.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

iarduino_MultiServo MSS;
RF24 radio(9,10);
byte addresses[][6] = {"1Node","2Node"};

struct Data {
  float x_percent;
  float y_percent;
  byte step;
};

void setup() {
  Serial.begin(9600);
  printf_begin();
  
  radio.begin();
  radio.setAutoAck(1);
  radio.enableAckPayload();
  radio.setRetries(0,15);
  radio.setPayloadSize(5);
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]); 

  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  
  radio.startListening();
  radio.powerUp();
  radio.printDetails();
  
  MSS.servoSet(0,SERVO_SG90);
  MSS.begin();
}

void loop() {
   byte pipeNo;
   byte response[5];
    while( radio.available(&pipeNo)){
      radio.read(&response, 5);                   
      radio.writeAckPayload(pipeNo,&response, sizeof(response));  // This can be commented out to send empty payloads.
      Serial.print(response[0]);
      Serial.print("-");
      Serial.print(response[1]);
      Serial.print(",");
      Serial.print(response[2]);
      Serial.print("-");
      Serial.print(response[3]);
      Serial.print(",");
      Serial.println(response[4]);  
   }

   int deg = response[0] * 0.6;

   if(response[1] == 1) {
      deg = deg * -1;
   }

   int rotate_to = 90 + deg;

  printf("Servo deg(%d)\n", rotate_to);
  MSS.servoWrite(0,rotate_to);

  delay(100);
}
