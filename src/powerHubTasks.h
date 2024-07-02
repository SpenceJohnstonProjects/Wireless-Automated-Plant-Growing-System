#ifndef POWERHUBTASKS_H
#define POWERHUBTASKS_H

#include "structs.h"
#include "esp_err.h"
//my defines
#define xDelayPower (1000 / portTICK_PERIOD_MS)

//GPIO defines
#define POWER_PERIPHERAL0 GPIO_NUM_9
#define POWER_PERIPHERAL1 GPIO_NUM_18
#define POWER_PERIPHERAL2 GPIO_NUM_19

typedef struct
{
    PowerPeripheral ports[MAX_POWER_PORTS];
    PowerDevice powerPeripheralStates; //fan is, pumpis, lightis...
    timerProfile powerDeviceTimers;

    uint8_t lightEndHr[2];
    uint8_t lightEndMin[2];
    bool    lightRollOver[2];
    bool    isSecondCycle;
}PowerDeviceStates;

esp_err_t setPortType(PowerType type, uint8_t index);
esp_err_t setStateLevelByType(PowerType *type, PowerLevel *OF);
void setPowerLevelByType(PowerType type, PowerLevel OF);
esp_err_t setPowerTimers(selectTimer select, uint32_t value);
void startNewCycle();
void driveCycle();
void drive();
void checkLightTimer();
esp_err_t drivePowerPorts();
esp_err_t initPowerHub();
void initPowerIO();
void deinitPowerHub();
void deinitPowerIO();
void relayTask(void *pvParameter);
void lightsetEndTimer(uint8_t *hr, uint8_t *min, uint16_t * add, bool * rollover);
void updateComHub();
PowerType getPortType(uint8_t index);
void requestProfileTimers();
void requestPortTypes();
void requestTime();
PowerLevel requestTypeState(PowerType type);

#endif