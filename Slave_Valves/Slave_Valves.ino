#include "Valves_Def.h"
#include "Common_Def.h"
#include <Wire.h>
#include "Valve.h"

Valve v1_in(V1_IN, V1_IN_O, V1_IN_C); //IN кран первой бочки
Valve v1_out(V1_OUT, V1_OUT_C, V1_OUT_C); //OUT кран первой бочки
Valve v2_in(V2_IN, V2_IN_O, V2_IN_C); //IN кран второй бочки 
Valve v2_out(V2_OUT, V2_OUT_O, V2_OUT_C); //OUT кран второй бочки

byte switchesStates[4][2];

void setup() {
	Serial.begin(9600);
 // Wire.begin(SLAVE_ADDR);
 // Wire.onReceive(valvesAction);
 pinMode(13, OUTPUT);

//TODO инициализация кранов
//чтение из EEPROM состояния кранов: желаемое состояние и состояния концевиков
//если какой-либо кран не достиг желаемого состояния, повторить нужное действие
}

void loop() {
  int result = valve.getState();
  result = valve.selfTest();
  result ? digitalWrite(13, HIGH) : digitalWrite(13, LOW);
  delay(3000);
	
}


/*
 * на основании состояния поплавков ведущим формируется команда - число от 1 до 8
 * оно умещается в один байт, принимается ведомым, т.е тут, и в зависимости от числа,
 * выбирается действие: открыть или закрыть нужный кран
 */
void valvesAction(){
	bool result = false;	
  byte action = 0;
  action = Wire.read();
  
  //TODO сохранение в EEPROM

  switch(action){
    case V1_IN_OPEN:
		result = v1_in.openValve();
		break;
    case V1_IN_CLOSE:
		result = v1_in.closeValve();
		break;
    case V1_OUT_OPEN:
		result = v1_out.openValve();
		break;
    case V1_OUT_CLOSE:
		result = v1_out.closeValve();
		break;
    case V2_IN_OPEN:
		result = v2_in.openValve();
		break;
    case V2_IN_CLOSE:
		result = v2_in.closeValve();
		break;
    case V2_OUT_OPEN:
		result = v2_out.openValve();
		break;
    case V2_OUT_CLOSE:
		result = v2_out.closeValve();
		break; 
  }
}
