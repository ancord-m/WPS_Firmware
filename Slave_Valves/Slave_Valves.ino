#include "Valves_Def.h"
#include "Common_Def.h"
#include <Wire.h>
#include "Valve.h"

Valve valve(V2_IN, V2_IN_O, V2_IN_C);
AF_DCMotor m(3);


void setup() {
  Serial.begin(9600);
 // Wire.begin(SLAVE_ADDR);
 // Wire.onReceive(valvesAction);
}

void loop() {
  int result = valve.getState();
  //valve.selfTest();
/*
  switch(result)
  {
    case 0: Serial.print("Opened\n"); break;
    case 1: Serial.print("Closed\n"); break;
    case 2: Serial.print("Undefined\n"); break;
  }

  Serial.print('\n');
  valve.openValve();
  delay(2000);
  valve.closeValve();
  delay(2000);*/
/*  m.setSpeed(200);
  m.run(BACKWARD);
  delay(1000);
  m.run(FORWARD);
  delay(1000);*/
  
}


/*
 * на основании состояния поплавков ведущим формируется команда - число от 1 до 8
 * оно умещается в один байт, принимается ведомым, т.е тут, и в зависимости от числа,
 * выбирается действие: открыть или закрыть нужный кран
 */
void valvesAction(){
  byte action = 0;
  action = Wire.read();

  switch(action){
    case V1_IN_OPEN:
                  Serial.print("Valve 1 is opened");                 
                  break;
    case V1_IN_CLOSE:
                  Serial.print("Valve 1 is closed");    
                  break;
    case V1_OUT_OPEN:
                  Serial.print("Valve 2 is opened");
                  break;
    case V1_OUT_CLOSE:
                  Serial.print("Valve 2 is closed");
                  break;
    case V2_IN_OPEN:
                  Serial.print("Valve 3 is opened");
                  break;
    case V2_IN_CLOSE:
                  Serial.print("Valve 3 is closed");
                  break;
    case V2_OUT_OPEN:
                  Serial.print("Valve 4 is opened");
                  break;
    case V2_OUT_CLOSE:
                  Serial.print("Valve 4 is closed");
                  break; 
  }
  Serial.print('\n');
}
