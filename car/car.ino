#include <iarduino_MultiServo.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

#define data_size 5

iarduino_MultiServo MSS;
RF24 radio(9,10);
byte addresses[][6] = {"1Node"};

void printAxis(char axis, byte value, byte negative) {
  Serial.print(" | ");
  Serial.print(axis);
  Serial.print(": ");
  Serial.print(value);
  Serial.print(" | ");
  Serial.print("isNegative: ");
  Serial.print(negative);
  Serial.print(" | ");
}

void setup() {
  Serial.begin(250000);
  printf_begin();
  
  radio.begin();
  radio.setAutoAck(1);
  radio.enableAckPayload();
  radio.setRetries(0,15);
  radio.setPayloadSize(data_size);
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
   byte response[data_size];
    while(radio.available(&pipeNo)){
      radio.read(&response, data_size);                   
      radio.writeAckPayload(pipeNo,&response, data_size);
      
      printAxis('X', response[0], response[1]);
      printAxis('Y', response[2], response[3]);
      Serial.print("step: ");
      Serial.println(response[4]);

      int deg = response[0] * 0.6;

      if(response[1] == 1) {
        deg = deg * -1;
      }

      int rotate_to = 90 + deg;

      printf("Servo deg(%d)\n", rotate_to);
      MSS.servoWrite(0,rotate_to);
   }

  delay(25);
}
