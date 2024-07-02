#ifndef ENUMS_H
#define ENUMS_H

#define SENSOR_TYPE_MAX 3
#define POWER_TYPE_MAX 4
#define TIMER_COUNT 8 //amount of timers, for loops
#include <stdint.h>

//ENUMs
typedef enum {
    COM,
    SENS,
    POW
}DeviceType;

typedef enum {
    Temp,
    Hum,
    SoilMoist
    //add more later. ex water level sensor etc.
}SensorType;

typedef enum {
    FAN,
    LIGHT,
    PUMP,
    NONE
    //add more later. ex humidifier, heater,  etc.
}PowerType;

typedef enum {
    OFF,
    ON,
    ALWAYS_ON,
    ALWAYS_OFF,
    ERROR
}PowerLevel;

//used to select timer of powerhub
typedef enum {
    LIGHT_START_HR_C1,
    LIGHT_START_HR_C2,
    LIGHT_START_MIN_C1,
    LIGHT_START_MIN_C2,
    LIGHT_TIMER_D_C1,
    LIGHT_TIMER_D_C2,
    LIGHT_TIMER_MIN_C1,
    LIGHT_TIMER_MIN_C2
}selectTimer;

#endif