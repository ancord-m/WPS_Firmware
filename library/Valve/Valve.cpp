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
	desiredState = OPENED;
	//writeDesiredState();
	return action(OPENED);
}

bool Valve::closeValve()
{
	desiredState = CLOSED;
	//writeDesiredState();
	return action(CLOSED);		
}

bool Valve::action(ValveState state)
{	
	bool switchWasPressed = getState() != UNDEFINED ? true : false;
	Serial.print("Inside action. Desired state: ");
	state ? Serial.print("CLOSED\n") : Serial.print("OPENED\n");	
	if(getState() != state){
		unsigned long i = 0;
		bool finish = false;
		AF_DCMotor valveMotor(valveNumber);
		valveMotor.setSpeed(MOTOR_SPEED);		
		while(!finish){	
			if(state == OPENED) {			
				valveMotor.run(OPEN);	
			} else {				
				valveMotor.run(CLOSE);	
			}	
			
			if(getState() == state) {
				for(int i = 0; i < 2000; ++i) {
					continue;
				}
				finish = getState() == state ? true : false;
			}
		//	Serial.println(i);		
					
			//войдем только, если концевик не сработал, либо упало напряжение (кран поедет медленее) 
			if(++i >= STOP_COUNT) { 
				switchWasPressed = false; //если концевик не сработает, то об этом будет сообщено	
				Serial.println("SWITCH ERROR");	
				break;
			} else {
				switchWasPressed = true;
			}		
		}
		
		Serial.print("Stopping valve\n");
		Serial.println(i);
		Serial.println(switchWasPressed);			
		
		valveMotor.run(STOP);
		
	//	EEPROM.write(19, switchWasPressed);
	//	EEPROM.write(20, highByte(i));
	//	EEPROM.write(21, lowByte(i));
		
		
		return switchWasPressed;	
	}
	
	return switchWasPressed;
}

int Valve::getState()
{
	//когда концевик нажат, digitalRead/analogRead вернет ноль
	bool opened = false;
	bool closed = false;	
	
	//концевики крана подключены к портам 20 и 21, их надо через analogRead опрашивать
	if(openedSwitch != 21 && closedSwitch != 20) { 
		opened = digitalRead(openedSwitch);
		closed = digitalRead(closedSwitch);
	} else 	{		
	//	opened = analogRead(openedSwitch);
	//	closed = analogRead(closedSwitch);
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
	desiredState ? Serial.print("Wanted to CLOSE\n") : Serial.print("Wanted to OPEN\n");
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

void Valve::limitSwitchesTest(){
	int opened;
	int closed;
	
	if(openedSwitch != 21 && closedSwitch != 20) { 
		opened = digitalRead(openedSwitch);
		closed = digitalRead(closedSwitch);
	} else 	{	
		opened = analogRead(openedSwitch);
		closed = analogRead(closedSwitch);
	}
	
	Serial.print("Valve "); Serial.print(valveNumber);
	Serial.print(": Open SW: ");
	Serial.print(opened);
	Serial.print("; Close SW: ");
	Serial.println(closed);
}
