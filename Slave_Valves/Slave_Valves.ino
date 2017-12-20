#include "Valves_Def.h"
#include "Common_Def.h"
#include <Wire.h>
#include "Valve.h"

#define SIGNAL_LED 13
//инициализация кранов
Valve v1_in(V1_IN, V1_IN_O, V1_IN_C); //IN кран первой бочки
Valve v1_out(V1_OUT, V1_OUT_C, V1_OUT_C); //OUT кран первой бочки
Valve v2_in(V2_IN, V2_IN_O, V2_IN_C); //IN кран второй бочки 
Valve v2_out(V2_OUT, V2_OUT_O, V2_OUT_C); //OUT кран второй бочки

byte swst[4][2]; //switches states

void setup() {
	Serial.begin(9600);
  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(valvesAction);
  
  pinMode(SIGNAL_LED, OUTPUT);
  readEeprom(); 
 // v2_in.restoreState();

  byte wasPressed = EEPROM.read(19);
  byte hi = EEPROM.read(20);
  byte lo = EEPROM.read(21);
  unsigned int counter = word(hi, lo);

  Serial.print("PRESSED ");
  Serial.println(wasPressed);
  Serial.println(counter);
  delay(3000);
}

void loop() {
      // v1_in.selfTest();
    //  v1_out.selfTest();
      // v2_in.selfTest();
    //    v2_out.selfTest();
 // errorCheck();

 delay(100);
 
}


/*
 * на основании состояния поплавков ведущим формируется команда - число от 1 до 8
 * оно умещается в один байт, принимается ведомым, т.е тут, и в зависимости от числа,
 * выбирается действие: открыть или закрыть нужный кран
 */
void valvesAction(int bytesToRead){
	bool result = false;	
  byte action = 0;
  action = Wire.read();

  //v2_in.openValve();
  //TODO сохранение в EEPROM
  Serial.println(action);
  
  switch(action){
      case V1_IN_OPEN:
          digitalWrite(SIGNAL_LED, LOW);
          result = v1_in.openValve();
    //     swst[V1_IN - 1][OPENED] = result;
    //     EEPROM.write(3 * (V1_IN - 1) + 1, result);
		      break;
      case V1_IN_CLOSE:
          digitalWrite(SIGNAL_LED, HIGH);
      		result = v1_in.closeValve();
      		break;
      case V1_OUT_OPEN:
          digitalWrite(SIGNAL_LED, LOW);
      		result = v1_out.openValve();         
      		break;
      case V1_OUT_CLOSE:
          digitalWrite(SIGNAL_LED, HIGH);
      		result = v1_out.closeValve();
      		break;
      case V2_IN_OPEN:  
          digitalWrite(SIGNAL_LED, LOW);
      		result = v2_in.openValve();          
      		break;
      case V2_IN_CLOSE:   
          digitalWrite(SIGNAL_LED, HIGH);
      		result = v2_in.closeValve();            
      		break;
      case V2_OUT_OPEN:
          digitalWrite(SIGNAL_LED, LOW);
      		result = v2_out.openValve();
    	  	break;
      case V2_OUT_CLOSE:
          digitalWrite(SIGNAL_LED, HIGH);
      		result = v2_out.closeValve();
    		  break; 
  }
}



void readEeprom(){
  for(int i = 0; i < 4; i++){ //i - номер крана - 1
    swst[i][OPENED] = EEPROM.read(3 * i + 1); //был нажат концевик OPENED?
    swst[i][CLOSED] = EEPROM.read(3 * i + 2); //был нажат концевик CLOSED?
  }
}

void signalLedBlink(){
  delay(200);
  digitalWrite(SIGNAL_LED, HIGH);
  delay(200);
  digitalWrite(SIGNAL_LED, LOW);
}
/* 
 * Для каждого крана проверяем его состояние 
 * Если что-то не так, диод на плате говорит: номер крана - неисправный концевик
 * см документацию
 */
void errorCheck(){
  for(int vCounter = 0; vCounter < 4; vCounter++){  
    if(!swst[vCounter][OPENED] || !swst[vCounter][CLOSED]){
      //сообщаем номер крана
      for(int i = vCounter + 1; i != 0; i--){
        signalLedBlink();      
      }    
      //сообщаем, какой концевик не сработал
      if(!swst[vCounter][OPENED]) {
        delay(700);
        signalLedBlink();
      }    
      if(!swst[vCounter][CLOSED]) {
        delay(700);
        signalLedBlink();
        signalLedBlink();
      }
    }
    delay(3000);
  }
}

