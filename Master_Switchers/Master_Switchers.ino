#include "Switchers_Def.h"
#include "Common_Def.h"
#include <Wire.h>
#include <Servo.h>
#include <EEPROM.h>
#include <Avr/wdt.h>

//управляет коромыслом, которое направляет гипохлорид в нужную бочку
Servo servo; 
//при выкл электричества надо помнить, что надо было отдать (или нет) команду
//на закрытие входного крана, также надо следить отдельно за этим и закрывать
//в рабочем цикле
bool needToClose_V1_IN;
bool needToClose_V2_IN;

void setup() {  
  pinMode(SIGNAL_LED, OUTPUT);
  initSwitchers();
  Wire.begin();   
   
  needToClose_V1_IN = EEPROM.read(V1_IN_CELL);
  needToClose_V2_IN = EEPROM.read(V2_IN_CELL);  

  wdt_enable(WDTO_8S);
}

void loop() {
  wdt_reset();
  //вспышка помогает понять, что есть 10 секунд, чтобы поднять в ручном режиме поплавки
  //и перейти в нужное состояние 1, 4 или 5 для ручной дозации или самодиагностики.

  digitalWrite(SIGNAL_LED, HIGH);
  delay(SHORT_DELAY);
  digitalWrite(SIGNAL_LED, LOW);
  
  delay(LONG_DELAY);

  switch(getSystemState()) {
        case 0: //бочки пусты, закрыть все краны, заполнять "вручную"            
            sendCommandToValvesController(V1_IN_CLOSE);
            delay(USHORT_DELAY);
            sendCommandToValvesController(V1_OUT_CLOSE);
            delay(USHORT_DELAY);
            sendCommandToValvesController(V2_IN_CLOSE);
            delay(USHORT_DELAY);
            sendCommandToValvesController(V2_OUT_CLOSE);           
            break;
        case 3: //0011 первая бочка закончилась, ВКЛЮЧИТЬ ВТОРУЮ            
            sendCommandToValvesController(V2_OUT_OPEN);
            delay(USHORT_DELAY);
            sendCommandToValvesController(V1_OUT_CLOSE);
            delay(USHORT_DELAY);
            doseTo(TO_FIRST);
            doseHClO();
            sendCommandToValvesController(V1_IN_OPEN);
            needToClose_V1_IN = true; 
            EEPROM.write(V1_IN_CELL, 1);
            break;
        case 12: //1100 вторая бочка закончилась, ВКЛЮЧИТЬ ПЕРВУЮ            
            sendCommandToValvesController(V1_OUT_OPEN);
            delay(USHORT_DELAY);
            sendCommandToValvesController(V2_OUT_CLOSE);
            delay(USHORT_DELAY);
            doseTo(TO_SECOND);
            doseHClO();
            sendCommandToValvesController(V2_IN_OPEN);
            needToClose_V2_IN = true;
            EEPROM.write(V2_IN_CELL, 1);
            break;    
        case 1: //0001 штатно невозможно, в ручном режиме дозируем во вторую бочку            
            doseTo(TO_SECOND);
            doseHClO();    
            break;
        case 4: //0100 штатно невозможно, в ручном режиме дозируем в первую бочку            
            doseTo(TO_FIRST);
            doseHClO();
            break;
        case 5: //0101 ручной запуск самодиагностики            
            systemSelfTest();
            break;
        default:
            //TODO выводить код состояния куда-то
            break;
  }
  
  if(needToClose_V1_IN) {    
    while(!listenPin(B1_TS)) {  
      wdt_reset();    
      delay(SHORT_DELAY);
      continue;
    }
    sendCommandToValvesController(V1_IN_CLOSE);
    needToClose_V1_IN = false;
    EEPROM.write(V1_IN_CELL, 0);    
  }
  
  if(needToClose_V2_IN) {    
    while(!listenPin(B2_TS)) {  
      wdt_reset();    
      delay(SHORT_DELAY);
      continue;
    }
    sendCommandToValvesController(V2_IN_CLOSE);
    needToClose_V2_IN = false;
    EEPROM.write(V2_IN_CELL, 0);    
  }  
}

void sendCommandToValvesController(byte command){ 
  wdt_reset();   
  Wire.beginTransmission(VALVES_CONTROLLER);
  Wire.write(command);
  Wire.endTransmission();
  wdt_reset();
}

void initSwitchers() {
  pinMode(DOSE_PUMP, OUTPUT); digitalWrite(DOSE_PUMP, LOW);
  pinMode(B1_BS, INPUT); digitalWrite(B1_BS, HIGH);
  pinMode(B1_TS, INPUT); digitalWrite(B1_TS, HIGH);
  pinMode(B2_BS, INPUT); digitalWrite(B2_BS, HIGH);
  pinMode(B2_TS, INPUT); digitalWrite(B2_TS, HIGH);
}

//позиционный код состояния системы
//НП1_ВП1_НП2_ВП2
unsigned int getSystemState() {
  return (listenPin(B1_BS) << 3) |
         (listenPin(B1_TS) << 2) |
         (listenPin(B2_BS) << 1) |
         (listenPin(B2_TS) << 0);
}

void doseHClO(){
  digitalWrite(DOSE_PUMP, HIGH);
  delay(1000);
  digitalWrite(DOSE_PUMP, LOW);
}

void doseTo(unsigned int barrel){
  wdt_reset();
  servo.attach(SERVO_PIN);
  servo.write(barrel);
  delay(SHORT_DELAY);
  servo.detach();
}

//примитивная защита от дребезга
bool listenPin(unsigned int pin) {
  bool finish = false;
  bool result;
  
  while(!finish) {
      result = digitalRead(pin);
      for(int i = 0; i < 2000; ++i) {
          wdt_reset();
          continue;
      }
      finish = digitalRead(pin) == result ? true : false;
  }
  
  return result;
}

void systemSelfTest() {
  doseTo(TO_FIRST);
  delay(LONG_DELAY);
  doseTo(TO_SECOND);
  delay(LONG_DELAY);
  sendCommandToValvesController(V1_IN_OPEN);
  delay(VLONG_DELAY);
  sendCommandToValvesController(V1_IN_CLOSE);
  delay(VLONG_DELAY);
  sendCommandToValvesController(V1_OUT_OPEN);
  delay(VLONG_DELAY);
  sendCommandToValvesController(V1_OUT_CLOSE);
  delay(VLONG_DELAY);
  sendCommandToValvesController(V2_IN_OPEN);
  delay(VLONG_DELAY);
  sendCommandToValvesController(V2_IN_CLOSE);
  delay(VLONG_DELAY);
  sendCommandToValvesController(V2_OUT_OPEN);
  delay(VLONG_DELAY);
  sendCommandToValvesController(V2_OUT_CLOSE);
  delay(VLONG_DELAY);
}

