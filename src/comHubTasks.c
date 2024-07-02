#include "define.h"
//C includes
#include <stdio.h>
//FreeRTOS includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
//espidf includes
#include "esp_log.h"
//my includes
#include "softAP.h"
#include "httpServer.h"
#include "structs.h"
#include "enums.h"
#include "comHubTasks.h"
#include "clock.h"

static const char *TAG = "Communication Hub";

//globals
//TODO make arrays for multiple device.
SensorDevice connectedSensorDevice;
PowerDevice connectedPowerDevice;
DeviceProfile profile;
PowerType p_portInfo[MAX_POWER_PORTS];
bool hasProfileChanged;
bool hasPowerPortChanged;

TaskHandle_t commTaskHandle;

/*
    Name: comHub_task
    Description: get sensor data, check against profile values. turn peripheral on or off depending on values.
        
    input(s):  none
    output(s): none
*/
void comHub_task(void *pvParameter)
{  
   while(1)
   {
      //fan control
      //compare temperature and humidity to profile
      if((connectedSensorDevice.temp > profile.temp.maxTemp) || (connectedSensorDevice.hum > profile.hum.maxHum))
         profile.device_p->fanIs = ON;
      else
         profile.device_p->fanIs = OFF;
         
      //pump control
      //compare soil moisture to profile
      if(connectedSensorDevice.soilMoist <= profile.sMoist.actualminSoilMoist)
         profile.device_p->pumpIs = ON;
      else
         profile.device_p->pumpIs = OFF;

      vTaskDelay(xDelayCom);
   }
}

/*
    Name: updateSMoistActual
    Description: updates threshold value becaue of change in threshold or update in calibrated max value

   equation used: Relative Value = Maximum Value * (Percentage / 100)
      Relative value = threshold value
      Maximum value = soil moisture calibration max
      percentage = threshold %
        
    input(s):  none
    output(s): none
*/
void updateSMoistActual()
{
   profile.sMoist.actualminSoilMoist = (uint8_t) profile.sMoist.maxSoilMoistCalibration * (profile.sMoist.minSoilMoistPercent / 100.0); 
}

/*
    Name: initComHub
    Description: initialize profiles. start communication hub tasks
        
    input(s):  none
    output(s): none
*/
esp_err_t initComHub()
{
   ESP_LOGI(TAG, "initializing communication hub");
    //setup profile
   //make null pointers when array added.
   profile.device_p = &connectedPowerDevice;
   profile.device_s = &connectedSensorDevice;
   memset(profile.device_p_info.name, '\0', STR_LEN);
   memset(profile.device_s_info.name, '\0', STR_LEN);
   memcpy(profile.device_p_info.name, "power_hub", strlen("power_hub"));
   memcpy(profile.device_s_info.name, "sensor_hub", strlen("sensor_hub"));

   //init  default values of timers. send to powerhub
   hasProfileChanged = true;
   hasPowerPortChanged = false;
   
   profile.timer.lightStartHr[0] = 3;
   profile.timer.lightStartMin[0] = 30;
   profile.timer.lightTimer_d[0] = 14;
   profile.timer.lightTimer_m[0] = 30;
   profile.timer.lightStartHr[1] = 4;
   profile.timer.lightStartMin[1] = 31;
   profile.timer.lightTimer_d[1] = 14;
   profile.timer.lightTimer_m[1] = 40;

   //init default values for sensor profile
   profile.hum.maxHum = 50;
   profile.temp.maxTemp = 21;
   profile.sMoist.minSoilMoistPercent = 50;
   profile.sMoist.maxSoilMoistCalibration = 255;


   #ifdef DEBUG
   profile.device_s->hum = 60;
   profile.device_s->soilMoist = 50;
   profile.device_s->temp = 20;   
   #endif

   //init powerhub port types
   for(uint8_t it = 0; it < MAX_POWER_PORTS; it++)
   {
      p_portInfo[it] = NONE;
   }
   //setup wifit and http server
   wifi_init_softap();
   init_http_server();

   if(xTaskCreate(&comHub_task,         "comHub",                8192, NULL, 5, &commTaskHandle) == pdPASS )
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