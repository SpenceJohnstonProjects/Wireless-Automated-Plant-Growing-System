#include "define.h"
#include <stdio.h>
#include "esp_log.h"
//FreeRTOS includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
//ADC includes
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "soilMoistDriver.h"

static const char *TAG = "Soil moisture sensor";

static esp_adc_cal_characteristics_t adc1_chars;

/*
    Name: getMoistureLvl
    Description: 
        polls soil moisture peripheral using the following steps:
        1) Turn on device through GPIO
        2) poll device 256 times
        3) average results
        4) Turn off device

    input(s): voltage variable from outside of scope.
    output(s): averaged soil moisture level (mV)z
*/
void getMoistureLvl(uint8_t * voltage)
{
    uint8_t voltage_it = 0;
    uint32_t temp_voltage = 0;
    //turn on device with GPIO
    gpio_set_level(SENSOR_PERIPHERAL_SOILMOIST_EN,1);
    vTaskDelay(1); //setup time
    
    do //get avg of voltage readings
    {   //sum voltage levels 
        temp_voltage = temp_voltage + (esp_adc_cal_raw_to_voltage(adc1_get_raw(SOIL_ADC_PORT), &adc1_chars)); //12 bit ADC value
        #ifdef DEBUG_SENSOR_DRIVER
            ESP_LOGI(TAG, "AVG voltage accumulator: %lu at index %u", temp_voltage, voltage_it);
        #endif
        voltage_it = voltage_it + 1;
        vTaskDelay(1);
    } while (voltage_it != 0);
    
    //turn off device with GPIO
    gpio_set_level(SENSOR_PERIPHERAL_SOILMOIST_EN,0);

    
    //avg voltage readings aka divide by iterations (256)
    temp_voltage = temp_voltage >> 8;
    //quantize 12 bit sensor value to 8 bit value, aka divide by 16. 12bit max value / 8bits max value = 16
    temp_voltage = temp_voltage >> 4; 
    //update voltage value
    *voltage = (uint8_t) temp_voltage; 
}

/*
    Name: initSoilSensor
    Description: 
        Initialize Vin and ADC pins for SENS-13637 soil moisture peripheral

    input(s): none
    output(s): none
*/
void initSoilSensor()
{
    ESP_LOGI(TAG, "initializing soil moisture sensor");
    //Vin for soil moisture sensor
    ESP_ERROR_CHECK(gpio_set_direction(SENSOR_PERIPHERAL_SOILMOIST_EN, GPIO_MODE_OUTPUT));
    gpio_set_level(SENSOR_PERIPHERAL_SOILMOIST_EN,0);

    //ADC pin
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 0, &adc1_chars);
    
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    ESP_ERROR_CHECK(adc1_config_channel_atten(SOIL_ADC_PORT, ADC_ATTEN_DB_11));
}