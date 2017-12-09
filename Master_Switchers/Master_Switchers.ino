#include "Switchers_Def.h"
#include "Common_Def.h"
#include <Wire.h>

#define VALVES_CONTROLLER 8

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(13, OUTPUT);

  initSwitchers();

}

void loop() {
 Serial.println(getSystemState());  
  if(digitalRead(B1_BS)) {
    sendCommandToValvesController(V2_IN_OPEN);
  } else {
    sendCommandToValvesController(V2_IN_CLOSE);
  }
  delay(1000);
}

void sendCommandToValvesController(byte command){
  Wire.beginTransmission(VALVES_CONTROLLER);
  Wire.write(command);
  Wire.endTransmission();
}

void initSwitchers() {
  pinMode(B1_BS, INPUT); digitalWrite(B1_BS, HIGH);
  pinMode(B1_TS, INPUT); digitalWrite(B1_TS, HIGH);
  pinMode(B2_BS, INPUT); digitalWrite(B2_BS, HIGH);
  pinMode(B2_TS, INPUT); digitalWrite(B2_TS, HIGH);
}

//позиционный код состояния системы
//НП1_ВП1_НП2_ВП2
int getSystemState() {
  return (digitalRead(B1_BS) << 3) |
         (digitalRead(B1_TS) << 2) |
         (digitalRead(B2_BS) << 1) |
         (digitalRead(B2_TS) << 0);
}

