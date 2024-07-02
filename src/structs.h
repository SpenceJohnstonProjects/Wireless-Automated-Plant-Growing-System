#ifndef STRUCTS_H
#define STRUCTS_H

#include "enums.h"

//DEFINES
#define STR_LEN 50
#define MAX_DEVICES 1
#define MAX_SENSOR_PORTS 3
#define MAX_POWER_PORTS 3

//generic device type
typedef struct {
    char                name[STR_LEN];  //human readable name
    uint8_t             id[6];          //MAC address
}deviceInfo;

//SENSOR structures
typedef struct {
    SensorType          type;
    uint8_t             data;
}SensorPeripheral;

typedef struct {
    uint8_t             hum;
    uint8_t             temp;
    uint8_t             soilMoist;
}SensorDevice;

//POWER structures
typedef struct {
    PowerType           type;
    PowerLevel          OF;
}PowerPeripheral;

typedef struct {
    PowerLevel          fanIs;
    PowerLevel          pumpIs;
    PowerLevel          lightIs;
}PowerDevice;

//profiles
typedef struct {
    uint8_t             maxTemp;
}TempProfile;

typedef struct {
    uint8_t             maxHum;
}HumProfile;

typedef struct {
    uint8_t             minSoilMoistPercent;     //minimum soil moisture in percentage. relative percentage based on max soil calibration
    uint8_t             actualminSoilMoist;      //actual sensor reading minimum value
    uint8_t             maxSoilMoistCalibration; //maximum sensor value based on calibration
}SoilMoistProfile;

typedef struct {        //primary[0] secondary[1]
    uint8_t             lightTimer_d[2];    //# of days to run cycle
    uint8_t             lightStartHr[2];    //start time hours
    uint8_t             lightStartMin[2];   //start time minutes
    uint16_t            lightTimer_m[2];    //duration of light timer in min
    //uint8_t             pumpTimer_m;  //duration of pump timer in min. add in future development
    //uint8_t             fanTimer_m; //duration of fan timer in min. add in future development
}timerProfile;

//complex structures
typedef struct {
    char                name[STR_LEN];  // name of profile
    deviceInfo          device_p_info;  //name of power device
    PowerDevice *       device_p;       //power device state
    deviceInfo          device_s_info;  //name of sensor device
    SensorDevice *      device_s;       //sensor device state
    TempProfile         temp;           //temperature profile
    HumProfile          hum;            //humidity profile
    SoilMoistProfile    sMoist;         //soil moisture profile
    timerProfile        timer;          //timers for power device switching
}DeviceProfile;

#endif