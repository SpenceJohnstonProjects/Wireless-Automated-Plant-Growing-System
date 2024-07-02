#include "define.h"

#include <stdio.h>
//FreeRTOS includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
//esp includes
#include "esp_log.h"
//temp/hum sensor includes
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "dht11.h"
//my includes
#include "enums.h"
#include "soilMoistDriver.h"
#include "sensorHubTasks.h"
#include "clock.h"
#include "httpClient.h"

static const char *TAG = "Sensor hub";

/*
    Name: tempHumTask
    Description: 
          1) initializes the DHT11 temperature and humidity sensor.
          2) poll sensor data 256 times
          3) avg data
          4) sends that data to a queue.

     NOTES: The DHT11 can be polled once every 2 seconds 
     needs 1 sec setup time for power
    input(s): none
    output(s): Sensor message to http server
*/
void tempHumTask(void *pvParameter)
{
    struct dht11_reading sensorValue;
    DHT11_init(TEMP_HUM_GPIO);

   while(1)
   {    
          //get sensor value
          sensorValue = DHT11_read();
          //send msg(s)
          if(sensorValue.status == 0)
          {
               #ifndef DEBUG_SENSOR_DATA
               sensorHub_client_data_post(Hum, sensorValue.humidity);
               vTaskDelay(100 / portTICK_PERIOD_MS);
               sensorHub_client_data_post(Temp, sensorValue.temperature);
               #endif
          }
          
          #ifdef DEBUG_SENSOR_DATA
               ESP_LOGI( TAG, "Temperature is %d \n",  sensorValue.temperature);
               ESP_LOGI( TAG, "Humidity is %d\n",      sensorValue.humidity);
               ESP_LOGI( TAG, "Status code is %d\n",   sensorValue.status);
          #endif
          ESP_LOGI(TAG, "temperature and humidity sensor values read.");
          vTaskDelay(xDelaySensor);
   }
}

/*
    Name: moistureTask
    Description: 
          1) initialize SENSE-13637 soil moisture sensor peripheral.
          2) poll sensor data
          3) sends that data to a http server

    input(s): ADC value from sensor
    output(s): sensor data to http server
*/
void moistureTask(void *pvParameter)
{
    static uint8_t moistureLvl;

     initSoilSensor();
     while(1)
     {
          getMoistureLvl(&moistureLvl);
          #ifndef DEBUG_SENSOR_DATA
          sensorHub_client_data_post(SoilMoist, moistureLvl);
          #endif
          
          #ifdef DEBUG_SENSOR_DATA
               ESP_LOGI(TAG, "ADC value: %u mV\n", moistureLvl);
          #endif
          ESP_LOGI(TAG, "soil moisture sensor values read.");
          vTaskDelay(xDelaySensor);
     }
}

/*
    Name: init sensor hub
    Description: Start soil moisture, temperature, and humidity tasks

    input(s): none
    output(s): none
*/
void initSensorHub()
{
     //init http client using station
     http_client_init();

     xTaskCreate(&moistureTask,         "soil moisture task",                8192, NULL, 5, NULL);
     xTaskCreate(&tempHumTask,         "temperature and humidity task",      8192, NULL, 5, NULL);
}