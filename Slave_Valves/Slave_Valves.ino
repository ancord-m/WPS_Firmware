#include "Valves_Def.h"
#include "Common_Def.h"
#include <Wire.h>
#include <Avr/wdt.h>
#include "Valve.h"

#define SIGNAL_LED 13
//инициализация кранов
Valve v1_in(V1_IN, V1_IN_O, V1_IN_C); //IN кран первой бочки
Valve v1_out(V1_OUT, V1_OUT_C, V1_OUT_C); //OUT кран первой бочки
Valve v2_in(V2_IN, V2_IN_O, V2_IN_C); //IN кран второй бочки 
Valve v2_out(V2_OUT, V2_OUT_O, V2_OUT_C); //OUT кран второй бочки

byte swst[4][2]; //switches states

void setup() {
  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(valvesAction);  
  pinMode(SIGNAL_LED, OUTPUT);
  
  wdt_enable(WDTO_8S);
  //доезжаем куда надо после внезапного отключения
  v1_in.restoreState();
  v1_out.restoreState();
  v2_in.restoreState();
  v2_out.restoreState();  
}

void loop() {
  wdt_reset();
  errorCheck();
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
  
  //TODO сохранение в EEPROM
  
  switch(action){
      case V1_IN_OPEN:          
          result = v1_in.openValve();
          swst[V1_IN - 1][OPENED] = result;
    //     EEPROM.write(3 * (V1_IN - 1) + 1, result);
		      break;
      case V1_IN_CLOSE:
      		result = v1_in.closeValve();
          swst[V1_IN - 1][CLOSED] = result;
      		break;
      case V1_OUT_OPEN:
      		result = v1_out.openValve(); 
          swst[V1_OUT - 1][OPENED] = result;        
      		break;
      case V1_OUT_CLOSE:
      		result = v1_out.closeValve();
          swst[V1_OUT - 1][CLOSED] = result;
      		break;
      case V2_IN_OPEN:  
      		result = v2_in.openValve();
          swst[V2_IN - 1][OPENED] = result;         
      		break;
      case V2_IN_CLOSE:   
      		result = v2_in.closeValve();     
          swst[V2_IN - 1][CLOSED] = result;       
      		break;
      case V2_OUT_OPEN:
      		result = v2_out.openValve();
          swst[V2_OUT - 1][OPENED] = result;
    	  	break;
      case V2_OUT_CLOSE:
      		result = v2_out.closeValve();
          swst[V2_OUT - 1][CLOSED] = result;
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
    wdt_reset();
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

