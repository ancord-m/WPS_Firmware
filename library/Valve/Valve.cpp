#include "Valve.h"

#if (ARDUINO >= 100)
	#include <Arduino.h> 
#else
	#include <WProgram.h> 
#endif

Valve::Valve(int valveNumber, int openedSwitch, int closedSwitch)
{
	this->valveNumber = valveNumber;
	this->openedSwitch = openedSwitch;
	this->closedSwitch = closedSwitch;
	
	pinMode(openedSwitch, INPUT); digitalWrite(openedSwitch, HIGH);
	pinMode(closedSwitch, INPUT); digitalWrite(closedSwitch, HIGH);	
}

bool Valve::openValve()
{		
	wdt_reset();
	desiredState = OPENED;
	writeDesiredState();
	return action(OPENED);
}

bool Valve::closeValve()
{
	wdt_reset();
	desiredState = CLOSED;
	writeDesiredState();
	return action(CLOSED);		
}

bool Valve::action(ValveState state)
{	
	bool switchWasPressed = getState() != UNDEFINED ? true : false;	
	if(getState() != state){
		unsigned long i = 0;
		bool finish = false;
		AF_DCMotor valveMotor(valveNumber);
		valveMotor.setSpeed(MOTOR_SPEED);		
		while(!finish){
			wdt_reset();			
			if(state == OPENED) {			
				valveMotor.run(OPEN);	
			} else {				
				valveMotor.run(CLOSE);	
			}	
			//примитивная защита от дребезга контактов и наводок
			if(getState() == state) {
				//Ждем 3000 тактов. ПРЕРЫВАНИЯ ЗАПРЕЩЕНЫ, DELAY НЕ РАБОТАЕТ!
				for(int i = 0; i < 3000; ++i) {
					wdt_reset();
					continue;
				}
				//еще раз спросим кран: он уверен, что приехал?
				finish = getState() == state ? true : false;
			}					
			//войдем только, если концевик не сработал, либо упало напряжение (кран поедет медленее) 
			if(++i >= STOP_COUNT) { 
				switchWasPressed = false; //если концевик не сработает, то об этом будет сообщено	
				break;
			} else {
				switchWasPressed = true;
			}		
		}		
		valveMotor.run(STOP);		
		return switchWasPressed;	
	}	
	return switchWasPressed;
}

int Valve::getState()
{
	//когда концевик нажат, digitalRead вернет 
	//analogRead ничего не вернет, АЦП отнимает много процессорного времени и дает кучу наводок.
	//отключать V1 OUT, который висит на A6 и A7, только по счетчику STOP_COUNT
	bool opened = false;
	bool closed = false;	
	
	//концевики крана подключены к портам 20 и 21, их надо через analogRead опрашивать
	if(openedSwitch != 21 && closedSwitch != 20) { 
		opened = digitalRead(openedSwitch);
		closed = digitalRead(closedSwitch);
	} 
		
	//если кран завис, отказал концевик, ни один не нажат, то будут одинаковые значения
	//XOR позволит это выявить и сообщить выше.
	if(!(opened ^ closed))
	{
		return UNDEFINED;
	}
	
	if(opened == false) 
	{
		return OPENED;
	} 
	else if(closed == false) 	
	{
		return CLOSED;
	}	
}

void Valve::writeDesiredState(){
	int startAddr = 3 * (valveNumber - 1); //см протокол хранения состояния
	EEPROM.write(startAddr, desiredState); //0 - OPENED, 1 - CLOSED
	delay(10);
}

int Valve::readDesiredState(){
	int startAddr = 3 * (valveNumber - 1);
	return EEPROM.read(startAddr);
}

int Valve::getValveNumber(){
	return valveNumber;
}

void Valve::restoreState(){
	//восстановка состояния и если желаемое не достигнуто - достигнуть
	//desiredState может быть либо 0 (OPENED), либо 1 (CLOSED); см enum ValveState
	readDesiredState() ? desiredState = CLOSED : desiredState = OPENED;
	if(desiredState != getState()){
		desiredState ? closeValve() : openValve();			
	}
}

bool Valve::selfTest(){
	bool oswp = false; //opened switch was pressed
	bool cswp = false; //closed ...
	
	oswp = openValve();
	delay(3000);
	cswp = closeValve();
	delay(3000);
	
	//EEPROM.write(3*(valveNumber - 1) + 1, oswp);
	//EEPROM.write(3*(valveNumber - 1) + 2, cswp);
	
	return oswp && cswp;
}