#ifndef Valve_h
#define Valve_h

#define OPEN BACKWARD
#define CLOSE FORWARD
#define STOP RELEASE
#define MOTOR_SPEED 200
#define CYCLES_TO_STOP 50
#define SHORT_DELAY 100

#include "AFMotor.h"
#include <Wire.h>


//valveNumber - число от 1 до 4 для обращения с Motor Shield L293D
//openedSwitch и closedSwitch - номера ножек, к которым подключены концевики крана

enum ValveState //текущие они же и желаемые состояния
{
	OPENED,
	CLOSED,
	UNDEFINED //если оба концевика показывают одинаковые значения
};

class Valve
{
	public:
		Valve(int valveNumber, int openedSwitch, int closedSwitch);
		bool openValve();
		bool closeValve();
		int getState();
		bool selfTest();
	private:
		int desiredState; //желаемое состояние из ValveState; если вырубят свет в работе, то надо помнить, куда ехал
		int openedSwitch;  //номер ножки МК
		int closedSwitch; //к которой подключен концевик
		int valveNumber; //номер для обращения с платы MS L293D		
		bool action(ValveState state); //передаю желаемое состояние
};

#endif
