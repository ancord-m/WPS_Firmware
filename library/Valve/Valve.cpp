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
	
	//восстановка состояния и если желаемое не достигнуто - достигнуть
	//desiredState может быть либо 0 (OPENED), либо 1 (CLOSED); см enum ValveState
	desiredState = readDesiredState();
	if(desiredState != getState())
	{
		desiredState ? closeValve() : openValve();
	}
	
}

bool Valve::openValve()
{		
	desiredState = OPENED;
	writeDesiredState();
	return action(desiredState);
}

bool Valve::closeValve()
{
	desiredState = CLOSED;
	writeDesiredState();
	return action(desiredState);		
}

bool Valve::action(ValveState state)
{
	int cycles = 0;
	bool switchWasPressed = getState() != UNDEFINED ? true : false;
	Serial.print("Inside action. Desired state: ");
	state ? Serial.print("CLOSED\n") : Serial.print("OPENED\n");
	if(getState() != state){
		AF_DCMotor valveMotor(valveNumber);
		valveMotor.setSpeed(MOTOR_SPEED);
		&valveMotor ? Serial.print("Speed was set\n") : Serial.print("Null pointer\n");
		while(getState() != state){
			if(state == OPENED) {
				Serial.print("Opening valve. State: ");
				Serial.print(getState());
				Serial.print('\n');
				valveMotor.run(OPEN);	
			} else {
				Serial.print("Closing valve. State: ");
				Serial.print(getState());
				Serial.print('\n');
				valveMotor.run(CLOSE);	
			}
			//число циклов * delay ниже, превосходит время на откр/закр крана
			//и сюда войдем только, если концевик не сработал, либо упало напряжение (кран поедет медленее) 
			if(++cycles > CYCLES_TO_STOP) { 
				switchWasPressed = false; //если концевик не сработает, то об этом будет сообщено
				break;
			} else {
				switchWasPressed = true;
			}
			delay(SHORT_DELAY);			
		}
		Serial.print("Stopping valve\n");
		valveMotor.run(STOP);	
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
		opened = analogRead(openedSwitch);
		closed = analogRead(closedSwitch);
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
}

int Valve::readDesiredState(){
	int startAddr = 3 * (valveNumber - 1);
	return EEPROM.read(startAddr);
}

int Valve::getValveNumber(){
	return valveNumber;
}

bool Valve::selfTest(){
	bool oswp = false; //opened switch was pressed
	bool cswp = false; //closed ...
	
	oswp = openValve();
	delay(500);
	cswp = closeValve();
	
	return oswp && cswp;
}
