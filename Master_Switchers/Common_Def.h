/*
	Здесь размещены общие для ведущего и ведомого
	определения символьных констант
*/
#ifndef COMMON_DEF_H
#define COMMON_DEF_H

//The address of the slave valve driver MCU
#define VALVES_CONTROLLER 8

#define INT_LED 13
#define SERVO_PIN 6
#define TO_FIRST 75   //angles
#define TO_SECOND 105

//List of commands to be received from Master
#define V1_IN_OPEN 1
#define V1_IN_CLOSE 2
#define V1_OUT_OPEN 3
#define V1_OUT_CLOSE 4
#define V2_IN_OPEN 5
#define V2_IN_CLOSE 6
#define V2_OUT_OPEN 7
#define V2_OUT_CLOSE 8

//Other
#define SLAVE_ADDR 8
#define OPEN BACKWARD 
#define CLOSE FORWARD

//delays in milliseconds
enum Delays {
  USHORT_DELAY = 500,
  SHORT_DELAY = 1000,
  LONG_DELAY = 5000,
  VLONG_DELAY = 10000
};

#endif
