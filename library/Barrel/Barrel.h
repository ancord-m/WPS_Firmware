#ifndef Barrel_h
#define Barrel_h

#include "WProgram.h"
#include "Arduino.h"

class Barrel
{
	public:
		Barrel(int number, Valve *inputValve, Valve *outputValve);
		Valve* getInputValve();
		Valve* getOutputValve();
	private:
		Valve *inputValve;
		Valve *outputValve;
		int barrelNumber;
}

#endif
