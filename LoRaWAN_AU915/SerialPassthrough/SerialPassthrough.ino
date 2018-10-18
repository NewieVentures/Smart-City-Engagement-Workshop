/*
 * Compatible with:
 * SODAQ MBILI
 * SODAQ Autonomo
 * SODAQ ONE
 * SODAQ ONE BETA
 * SODAQ EXPLORER
 */

#include "Arduino.h"

#if defined(ARDUINO_AVR_SODAQ_MBILI)
#define debugSerial Serial 
#define loraSerial Serial1

#elif defined(ARDUINO_SODAQ_AUTONOMO) || defined(ARDUINO_SODAQ_ONE) || defined(ARDUINO_SODAQ_ONE_BETA)
#define debugSerial SerialUSB 
#define loraSerial Serial1

#elif defined(ARDUINO_SODAQ_EXPLORER)
#define debugSerial SerialUSB
#define loraSerial Serial2

#else
// please select a sodaq board
#endif

void setup() {
  // Enable LoRa module
  #if defined(ARDUINO_SODAQ_AUTONOMO)
  pinMode(BEE_VCC, OUTPUT);
  digitalWrite(BEE_VCC, HIGH); //set input power BEE high
  #elif defined(ARDUINO_AVR_SODAQ_MBILI)
  pinMode(20, OUTPUT);
  digitalWrite(20, HIGH); //set input power BEE high
  #endif

  // Hard reset the RN module
  #if defined(LORA_RESET)
  pinMode(LORA_RESET, OUTPUT);
  digitalWrite(LORA_RESET, LOW);
  delay(100);
  digitalWrite(LORA_RESET, HIGH);
  delay(100);
  #endif

  while ((!debugSerial) && (millis() < 10000)){
    // wait 10 seconds for serial monitor
  }
  
  debugSerial.begin(57600);
  loraSerial.begin(57600); 
  
  debugSerial.println("Please send command");
}

void loop() {  

    //send and receive data with serial
     if (debugSerial.available()){
      //debugSerial.print("SEND:    ");
      while (debugSerial.available()) {
        uint8_t inChar = debugSerial.read();
        //debugSerial.write(inChar);
        loraSerial.write(inChar);
      }
     }

     if (loraSerial.available()){
      //debugSerial.print("RECEIVE: ");
      while (loraSerial.available()) {
        uint8_t inChar = loraSerial.read();
        debugSerial.write(inChar);
      }
     }
}

