/*
*    Project:    Growing Buddy
*    Objective:  Modular plant growing system
*    Author:     Spence Johnston
*/
#include "define.h"
#ifndef TEST
//C includes
#include <stdio.h>
//FreeRTOS includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
//ESP includes
#include "nvs_flash.h"

//task includes
#ifdef COMM
#include "comHubTasks.h"
#endif
#ifdef POWER
#include "powerHubTasks.h"
#endif
#ifdef SENSOR
#include "sensorHubTasks.h"
#endif
#ifdef LIGHT_OFF
#include "LedDriver.h"
extern QueueHandle_t ledQueue;
#endif

//prototypes
void init_NVS_flash();

//MAIN
void app_main() {
    init_NVS_flash();  

    #ifdef COMM
    initComHub();
    #endif
    #ifdef POWER
    initPowerHub();
    #endif
    #ifdef SENSOR
    initSensorHub();
    #endif
    #ifdef LIGHT_OFF
    LED_init();
    LED_msg led;
    led.isOn = false;
    xQueueSend(ledQueue, &led, 0);
    #endif
}

/*
    Name: init_NVS_flash
    Description: Initialize non volatile storage in flash

    input(s): none
    output(s): none
*/
void init_NVS_flash()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
}
//TODO add nvs flash options (non volalite storage) and separate file
//TODO add way to add profile settings in nvs
//see nvs_set_u8, nvs_get_u8, and nvs_commit
#endif