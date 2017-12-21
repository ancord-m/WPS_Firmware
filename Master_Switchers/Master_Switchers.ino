#include "Switchers_Def.h"
#include "Common_Def.h"
#include <Wire.h>
#include <Servo.h>

Servo servo;

void setup() {
  pinMode(INT_LED, OUTPUT);
  initSwitchers();
  servo.attach(SERVO_PIN);
  Wire.begin();  
}

void loop() {
  //вспышка помогает понять, что есть 10 секунд, чтобы поднять в ручном режиме поплавки
  //и перейти в нужное состояние 1, 4 или 5 для ручной дозации или самодиагностики.
  digitalWrite(INT_LED, HIGH);
  delay(SHORT_DELAY);
  digitalWrite(INT_LED, LOW);
  
  delay(VLONG_DELAY);

  switch(getSystemState()) {
        case 0: //бочки пусты, заполняем и открываем
            //открываем первую бочку
            sendCommandToValvesController(V1_OUT_OPEN);
            delay(USHORT_DELAY);
            sendCommandToValvesController(V2_OUT_CLOSE);
            delay(USHORT_DELAY);
            //TODO
            break;
        case 3: //первая бочка закончилась, ВКЛЮЧИТЬ ВТОРУЮ
            sendCommandToValvesController(V2_OUT_OPEN);
            delay(USHORT_DELAY);
            sendCommandToValvesController(V1_OUT_CLOSE);
            delay(USHORT_DELAY);
            servo.write(TO_FIRST);
            //TODO dose
            sendCommandToValvesController(V1_IN_OPEN);
            break;
        case 12: //вторая бочка закончилась, ВКЛЮЧИТЬ ПЕРВУЮ
            sendCommandToValvesController(V1_OUT_OPEN);
            delay(USHORT_DELAY);
            sendCommandToValvesController(V2_OUT_CLOSE);
            delay(USHORT_DELAY);
            servo.write(TO_SECOND);
            //TODO dose
            sendCommandToValvesController(V2_IN_OPEN);
            break;
        case 15: //первая или вторая бочка заполнена, закрыть все входные краны
            sendCommandToValvesController(V1_IN_CLOSE);
            delay(SHORT_DELAY);
            sendCommandToValvesController(V2_IN_CLOSE);
            break;    
        case 1: //штатно невозможно, в ручном режиме дозируем во вторую бочку
            servo.write(TO_SECOND);
            //TODO dose    
            break;
        case 4: //штатно невозможно, в ручном режиме дозируем в первую бочку
            servo.write(TO_FIRST);
            //TODO dose
            break;
        case 5: //ручной запуск самодиагностики
            break;
        default:
            //TODO выводить код состояния куда-то
            break;
  }
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
unsigned int getSystemState() {
  return (digitalRead(B1_BS) << 3) |
         (digitalRead(B1_TS) << 2) |
         (digitalRead(B2_BS) << 1) |
         (digitalRead(B2_TS) << 0);
}

