#ifndef DEFINE_H
#define DEFINE_H
//////////////////////////////
//DEFINE HOW TO PROGRAM BOARD
//WARNING: ONLY SELECT ONE
//#define COMM 1
//#define POWER 1
//#define SENSOR 1
//#define TEST 1
//#define LIGHT_OFF //used to turn off light on dev board.
//////////////////////////////

//////////////////////////////
//REAL-TIME DEBUG DEFINES
//#define DEBUG
//#define DEBUG_TIMER
//#define DEBUG_SENSOR_DATA
//#define DEBUG_SENSOR_DRIVER
//#define DEBUG_LIGHT_TIMER
//#define DEBUG_POWER_IO
//////////////////////////////

#define QUEUE_LEN 50

#define SSID_NAME "GrowingBuddy"
#define SSID_PWORD "ThisIsAPassword"

#ifdef TEST
#define STATIC 
#else
#define STATIC static
#endif
#endif