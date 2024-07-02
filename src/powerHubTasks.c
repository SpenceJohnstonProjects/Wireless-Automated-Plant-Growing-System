
#include "define.h"
//C includes
#include <stdio.h>
//FreeRTOS includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
//ESP includes
#include "driver/gpio.h"
#include "esp_log.h"
//my includes
#include "enums.h"
#include "powerHubTasks.h"
#include "structs.h"
#include "time.h"
#include "clock.h"
#include "httpClient.h"

static const char *TAG = "Power Hub Tasks";

//in clock.c
extern struct tm * timeInfo;

//globals
STATIC TaskHandle_t powerHubTaskHandle;

//states
PowerDeviceStates states;

/*
    Name: relayTask
    Description: updates system from com hub. drives relays 
        
    input(s): data from http server
    output(s): light state to http server
*/
void relayTask(void *pvParameter)
{
    while(1)
    {
        requestTime(); //gets system time from com hub
        requestProfileTimers();//check if timers have changed, if so update
        #ifdef DEBUG_TIMER
        ESP_LOGE(TAG, "timer primary start hr: %d", states.powerDeviceTimers.lightStartHr[0]);
        ESP_LOGE(TAG, "timer primary start min: %d", states.powerDeviceTimers.lightStartMin[0]);
        ESP_LOGE(TAG, "timer primary day: %d", states.powerDeviceTimers.lightTimer_d[0]);
        ESP_LOGE(TAG, "timer primary duration: %d", states.powerDeviceTimers.lightTimer_m[0]);
        ESP_LOGE(TAG, "timer secondary start hr: %d", states.powerDeviceTimers.lightStartHr[1]);
        ESP_LOGE(TAG, "timer secondary start min: %d", states.powerDeviceTimers.lightStartMin[1]);
        ESP_LOGE(TAG, "timer secondary day: %d", states.powerDeviceTimers.lightTimer_d[1]);
        ESP_LOGE(TAG, "timer secondary duration: %d", states.powerDeviceTimers.lightTimer_m[1]);
        #endif
        requestPortTypes();//check if port types have changed, if so update
        states.powerPeripheralStates.pumpIs = requestTypeState(PUMP);//check pump state
        states.powerPeripheralStates.fanIs = requestTypeState(FAN);//check fan state
        ESP_LOGE(TAG, "pump state: %d", states.powerPeripheralStates.pumpIs);
        ESP_LOGE(TAG, "light state: %d", states.powerPeripheralStates.lightIs);
        ESP_LOGE(TAG, "fan state: %d", states.powerPeripheralStates.fanIs);
        ESP_LOGE(TAG, "port0: %d = %d", states.ports[0].type, states.ports[0].OF);
        ESP_LOGE(TAG, "port1: %d = %d", states.ports[1].type, states.ports[1].OF);
        ESP_LOGE(TAG, "port2: %d = %d", states.ports[2].type, states.ports[2].OF);
        #ifdef DEBUG
        
        #endif
        drive();
        #ifdef DEBUG
        
        #endif
        updateComHub();
        vTaskDelay(xDelayPower);
    }
}

/*
    Name: drive
    Description: drives ports, cycles, and timers  
        
    input(s):  None
    output(s): None
*/
void drive()
{
    driveCycle();
    checkLightTimer();
    drivePowerPorts();
}

/*
    Name: setPowerLevelByType 
    Description: set ports of a given type to on/off
        
    input(s): 
        type: type of power port to change
        OF: on/off
    output(s): none
*/
void setPowerLevelByType(PowerType type, PowerLevel OF)
{
    for(uint8_t it = 0; it < MAX_POWER_PORTS; it++)
    {
        if(states.ports[it].type == type)
            states.ports[it].OF = OF;
    }
}

/*
    Name:lightsetEndTimer
    Description: set light end timer

    inputs: 
            hr:         start hr of cycle wanting to be changed
            min:        start min of cycle wanting to be changed
            add:        number of minutes to add to start time
            rollover:   roll over bool in case a timer goes past 24hr mark (aka rolls over)
    output: None
*/
 void lightsetEndTimer(uint8_t *hr, uint8_t *min, uint16_t * add, bool * rollover)
{
    uint16_t totalMinutes = *min + *add;

    *hr += totalMinutes / 60;
    *min = totalMinutes % 60;
    
    if( *hr >= 24)
    {
        *rollover = true;
        *hr = *hr % 24;
    }
    else
        *rollover = false;

    ESP_LOGE(TAG, "light end time: %02d:%02d. rollover: %d", *hr, *min, *rollover);
}

/*
    Name: driveCycle 
    Description: handles state of light cycle. determines if in 
                 primary or secondary light cycle.
        
    input(s):  None
    output(s): None
*/
void driveCycle()
{
    if(states.isSecondCycle == false)
    {
        uint8_t days;
        getLocalTime(&days, NULL, NULL);
        if(days >= states.powerDeviceTimers.lightTimer_d[states.isSecondCycle])
            states.isSecondCycle = true;
    }
}

/*
    Name: checkLightTimer
    Description: turn off or on light depending on time and timer bounds
        
    input(s):  None
    output(s): None
*/
void checkLightTimer()
{
    uint8_t currentHr;
    uint8_t currentMin;
    getLocalTime(NULL, &currentHr, &currentMin);
    //get start and end time of light timer to make code easier to read
    uint8_t startHr = states.powerDeviceTimers.lightStartHr[states.isSecondCycle];
    uint8_t startMin = states.powerDeviceTimers.lightStartMin[states.isSecondCycle];
    uint8_t endHr = states.lightEndHr[states.isSecondCycle];
    uint8_t endMin = states.lightEndMin[states.isSecondCycle];

    //bool to check if time is out of bounds of light timer
    bool isOutsideBounds;

    if(!states.lightRollOver[states.isSecondCycle])//if timer not rolled over
    {
        if(
            //above start time
            ((currentHr > startHr) ||
            (currentHr == startHr && currentMin > startMin)) &&
            //below end time
            ((currentHr < endHr) ||
            (currentHr == endHr && currentMin < endMin))
          )
            isOutsideBounds = false;
        else
            isOutsideBounds = true;
    }
    else //if timer rolled over
    {
        if(
            //below start time
            ((currentHr < startHr) ||
            (currentHr == startHr && currentMin < startMin)) &&
            //above end time
            ((currentHr > endHr) || 
            (currentHr == endHr && currentMin > endMin))
          )
            isOutsideBounds = true;
        else
            isOutsideBounds = false;
    }

    if(isOutsideBounds)
    {
        states.powerPeripheralStates.lightIs = OFF;
        #ifdef DEBUG_LIGHT_TIMER
        ESP_LOGE(TAG, "Light turned off");
        #endif
    }
    else
    {
        states.powerPeripheralStates.lightIs = ON;
        #ifdef DEBUG_LIGHT_TIMER
        ESP_LOGE(TAG, "Light turned on");
        #endif
    }

    #ifdef DEBUG_LIGHT_TIMER
    ESP_LOGE(TAG, "Current time: %02d:%02d", currentHr, currentMin);
    ESP_LOGE(TAG, "Light start time: %02d:%02d", states.powerDeviceTimers.lightStartHr[0], states.powerDeviceTimers.lightStartMin[0]);
    ESP_LOGE(TAG, "Light end time: %02d:%02d", states.lightEndHr[0], states.lightEndMin[0]);
    #endif
}

/*
    Name: drivePowerPorts
    Description: set state of ports, turn off or on ports.
        
    input(s):  None
    output(s): error state
*/
esp_err_t drivePowerPorts()
{
    //sets port variables values based on peripheral states
    setPowerLevelByType(FAN, states.powerPeripheralStates.fanIs);
    setPowerLevelByType(PUMP, states.powerPeripheralStates.pumpIs);
    setPowerLevelByType(LIGHT, states.powerPeripheralStates.lightIs);
    setPowerLevelByType(NONE, OFF);

    //sets actual ports based on port variable
    if(gpio_set_level(POWER_PERIPHERAL0, states.ports[0].OF) != ESP_OK)
        return ESP_FAIL;
    

    if(gpio_set_level(POWER_PERIPHERAL1, states.ports[1].OF) != ESP_OK)
        return ESP_FAIL;

    if(gpio_set_level(POWER_PERIPHERAL2, states.ports[2].OF) != ESP_OK)
        return ESP_FAIL;

    return ESP_OK;
}

/*
    Name: setPortType 
    Description: set the peripheral type of a specified port
        
    input(s): 
        type: peripheral type
        index: port number
    output(s): error state
*/
esp_err_t setPortType(PowerType type, uint8_t index)
{
    if(index >= MAX_POWER_PORTS  || type > NONE)
    {
        ESP_LOGI(TAG, "setPortType: Index out of bounds");
        return ESP_FAIL;
    }
    else
    {
        states.ports[index].type = type;
        return ESP_OK;
    } 
}

/*
    Name: setPowerTimers
    Description: set values of specified timers
        
    input(s): 
        select: timer type
        value: actual timer value
    output(s): error state
*/
esp_err_t setPowerTimers(selectTimer select, uint32_t value)
{
    switch(select)
    {
        case 
            LIGHT_START_HR_C1: 
            states.powerDeviceTimers.lightStartHr[0] = value; 
            states.lightEndHr[0] = value;
            states.lightEndMin[0] = states.powerDeviceTimers.lightStartMin[0];
            lightsetEndTimer(&states.lightEndHr[0], &states.lightEndMin[0], &states.powerDeviceTimers.lightTimer_m[0], &states.lightRollOver[0]);
            break;
        case 
            LIGHT_START_HR_C2: 
            states.powerDeviceTimers.lightStartHr[1] = value; 
            states.lightEndHr[1] = value;
            states.lightEndMin[1] = states.powerDeviceTimers.lightStartMin[1];
            lightsetEndTimer(&states.lightEndHr[1], &states.lightEndMin[1], &states.powerDeviceTimers.lightTimer_m[1], &states.lightRollOver[1]);
            break;
        case 
            LIGHT_START_MIN_C1: 
            states.powerDeviceTimers.lightStartMin[0] = value;
            states.lightEndMin[0] = value;
            states.lightEndHr[0] = states.powerDeviceTimers.lightStartHr[0]; 
            lightsetEndTimer(&states.lightEndHr[0], &states.lightEndMin[0], &states.powerDeviceTimers.lightTimer_m[0], &states.lightRollOver[0]);
            break;
        case 
            LIGHT_START_MIN_C2: 
            states.powerDeviceTimers.lightStartMin[1] = value; 
            states.lightEndMin[1] = value;
            states.lightEndHr[1] = states.powerDeviceTimers.lightStartHr[1];
            lightsetEndTimer(&states.lightEndHr[1], &states.lightEndMin[1], &states.powerDeviceTimers.lightTimer_m[1], &states.lightRollOver[1]);
            break;
        case 
            LIGHT_TIMER_D_C1: states.powerDeviceTimers.lightTimer_d[0] = value; break;
        case 
            LIGHT_TIMER_D_C2: states.powerDeviceTimers.lightTimer_d[1] = value; break;
        case 
            LIGHT_TIMER_MIN_C1: 
            states.powerDeviceTimers.lightTimer_m[0] = value; 
            states.lightEndHr[0] = states.powerDeviceTimers.lightStartHr[0];
            states.lightEndMin[0] = states.powerDeviceTimers.lightStartMin[0];
            lightsetEndTimer(&states.lightEndHr[0], &states.lightEndMin[0], &states.powerDeviceTimers.lightTimer_m[0], &states.lightRollOver[0]);
            break;
        case 
            LIGHT_TIMER_MIN_C2: 
            states.powerDeviceTimers.lightTimer_m[1] = value; 
            states.lightEndHr[1] = states.powerDeviceTimers.lightStartHr[1];
            states.lightEndMin[1] = states.powerDeviceTimers.lightStartMin[1];
            lightsetEndTimer(&states.lightEndHr[1], &states.lightEndMin[1], &states.powerDeviceTimers.lightTimer_m[1], &states.lightRollOver[1]);
            break;
        default:
            ESP_LOGI(TAG, "selection out of range");
            return ESP_FAIL;
    }
    return ESP_OK;
}

/*
    Name: startNewCycle
    Description: resets local days and sets cycle to primary
        
    input(s): None
    output(s): None
*/
void startNewCycle()
{
    resetDays();
    states.isSecondCycle = false;
}

/*
    Name: getPortType
    Description:  returns the peripheral type of a given port
        
    input(s): 
        index: port number
    output(s): peripheral type
*/
PowerType getPortType(uint8_t index)
{
    if(index >= MAX_POWER_PORTS)
    {
        ESP_LOGI(TAG, "index out of range");
        return NONE;
    }

    return states.ports[index].type;
}

/*
    Name: requestProfileTimers
    Description: requests timers from comhub profile
        
    input(s): None
    output(s): None
*/
void requestProfileTimers()
{
    //check if profile values have changed
    if(powerhub_profile_changed_request() == true)//if so, update
    {
        for(uint8_t it = 0; it < TIMER_COUNT; it++ )
        {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            setPowerTimers((PowerLevel) it, timerRequest(it));
        }
    }
}

/*
    Name: requestPortTypes
    Description: check if port type has changed from http server.
                 If so, update ports
        
    input(s): None
    output(s): None
*/
void requestPortTypes()
{
    //check if port types have changed
    if(powerhub_port_changed_request() == true)
    {//if so, update
        for(uint8_t it = 0; it < MAX_POWER_PORTS; it++ )
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
            setPortType(reqPortType(it), it);
        }
    }
}

/*
    Name: requestTypeState
    Description: request the power state of a given peripheral type
        
    input(s):  peripheral type to check
    output(s): power level of given type 
*/
PowerLevel requestTypeState(PowerType type)
{
    return powerhub_client_type_level_request(type);
}

/*
    Name: updateComHub
    Description: update http server
        
    input(s): None
    output(s): None
*/
void updateComHub()
{
    powerhub_client_update_light_status(states.powerPeripheralStates.lightIs);
}

/*
    Name: requestTime
    Description: heck if time has changed for comm hub. 
                 AKA if time has been set or days have been reset. 
                 If so, update
        
    input(s): None
    output(s): None
*/
void requestTime()
{
    uint8_t day;
    uint8_t hr;
    uint8_t min;
    powerhub_time_day_request(&day);
    powerhub_time_request(&hr, &min);
    if(day == 0)
        resetDays();
    setLocalTime(hr, min);
}

/*
    Name: initPowerHub
    Description: initialize power hub  
        
    input(s): None
    output(s): error state
*/
esp_err_t initPowerHub()
{
    ESP_LOGI(TAG, "initializing relay task");

    //init http client using station
    http_client_init();

    //initialize peripheral
    initPowerIO();
    
    //initialize Device information for port assignment
    //can use NONE for initial init of system
    states.powerPeripheralStates.fanIs = OFF;
    states.powerPeripheralStates.pumpIs = OFF;
    states.powerPeripheralStates.lightIs = OFF;
    states.isSecondCycle = false;
    states.lightRollOver[0] = false;
    states.lightRollOver[1] = false;

    for(uint8_t it = 0; it < MAX_POWER_PORTS; it++)
    {
        states.ports[it].OF = OFF;
        states.ports[it].type = NONE;
    }

    //grab timers from com hub
    for(uint8_t it = 0; it < TIMER_COUNT; it++ )
    {
        setPowerTimers((PowerLevel) it, timerRequest(it));
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    
    //grab port types from com hub
    for(uint8_t it = 0; it < MAX_POWER_PORTS; it++ )
    {
        setPortType(reqPortType(it), it);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    //grab time from com hub
    requestTime();

    //init light cycle
     startNewCycle();

    if(xTaskCreate(&relayTask,         "relay",                8192, NULL, 5, &powerHubTaskHandle) == pdPASS )
    {
        ESP_LOGI(TAG, "initialization complete!");
        return ESP_OK;
    }
    else
    {
        ESP_LOGI(TAG, "initialization failed.");
        return ESP_FAIL;
    }
}

/*
    Name: initPowerIO
    Description: initialize power hub IO
        
    input(s):  None
    output(s): None
*/
void initPowerIO()
{
    //reset pins, needed for config
    gpio_reset_pin(POWER_PERIPHERAL0);
    gpio_reset_pin(POWER_PERIPHERAL1);
    gpio_reset_pin(POWER_PERIPHERAL2);
    //set pins to output
    gpio_set_direction(POWER_PERIPHERAL0, GPIO_MODE_OUTPUT);
    gpio_set_direction(POWER_PERIPHERAL1, GPIO_MODE_OUTPUT);
    gpio_set_direction(POWER_PERIPHERAL2, GPIO_MODE_OUTPUT);
    //turn off initially
    gpio_set_level(POWER_PERIPHERAL0, OFF);
    gpio_set_level(POWER_PERIPHERAL1, OFF);
    gpio_set_level(POWER_PERIPHERAL2, OFF);
}

/*
    Name: deinitPowerHub
    Description: deinitialize power hub 
        
    input(s):  None
    output(s): None
*/
void deinitPowerHub()
{
    if(powerHubTaskHandle != NULL)
    {
        vTaskDelete(powerHubTaskHandle);
        powerHubTaskHandle = NULL;
    }
    
    deinitPowerIO();
}

/*
    Name: deinitPowerIO
    Description: deinitialize power hub IO
        
    input(s):  None
    output(s): None
*/
void deinitPowerIO()
{
    gpio_set_level(POWER_PERIPHERAL0, OFF);
    gpio_set_level(POWER_PERIPHERAL1, OFF);
    gpio_set_level(POWER_PERIPHERAL2, OFF);
}