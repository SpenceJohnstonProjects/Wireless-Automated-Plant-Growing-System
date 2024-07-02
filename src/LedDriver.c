#include "define.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <esp_log.h>
#include "led_strip.h"
#include "enums.h"
#include "LedDriver.h"

static const char *TAG = "LED driver";

#define xDelayBlink (100 / portTICK_PERIOD_MS)
#define LED_GPIO GPIO_NUM_8

//Globals
STATIC led_strip_handle_t       led_strip;
STATIC led_strip_config_t       strip_config;
STATIC led_strip_rmt_config_t   rmt_config;

static LED_data                 LED;

QueueHandle_t ledQueue;

/*
    Name:
    Description: Sets up LED. Receives queue msg, then outputs the desired effect to the LED
*
*           options:
*               set on/off: can either be on or off, if off. no other options matter
*               set color: change color between red, yellow, green
*               set blinking state: can either be solid or blinking
*
                cNULL option for color indicates that the LED color stays the same.
*   input:    queue message
*   output:   LED 
*/
void led_task(void *pvParameter)
{
    LED_msg ledMsg; //declare variable to hold cmds

    ledMsg.color = GREEN; //initialize default values for LED
    ledMsg.isBlink = false;
    ledMsg.isOn = true;
    LED_set_color(ledMsg.color);
    LED_SET();

    while(1)
    {
        if(xQueueReceive(ledQueue, &ledMsg, 0) == pdTRUE) //if a msg was received
        {
            ESP_LOGI(TAG, "LED msg received.");
            if(ledMsg.color != cNULL)
                LED_set_color(ledMsg.color);
        }
        if(!ledMsg.isOn) //if turn off signal
        {
            LED_OFF();
        }
        else if(ledMsg.isBlink) //if blinking set
        {
            blinkLED();
            vTaskDelay(xDelayBlink);
        }
        else
        {
            LED_SET();
        }
        vTaskDelay(100);
    }
}

/*
    Name: blinkLED
    Description: 
        blinks LED
    input(s):  none
    output(s): none
*/
void blinkLED()
{
    static bool blink_ff = 1; //blink flip flop

    if(blink_ff)
        LED_OFF();
    else
        LED_SET();
    blink_ff = !blink_ff;
}

#ifdef DEBUG
/*
    Name: cycle_color_task
    Description: cycles through LED colors defined in LED_data
        
    input(s):  none
    output(s): none
*/
void cycle_color_task(void *pvParameter)
{
    uint8_t color_it = 0;
    LED_init();
    while(1)
    {
        if(color_it >= RGB_COLOR_NUM)
            color_it = 0;
        else
            color_it++;

        LED_set_color(color_it);
        LED_SET();
        vTaskDelay(xDelayBlink);
    }
}

/*
    Name: blink_task
    Description: blinks LED using task
        
    input(s):  none
    output(s): none
*/
void blink_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Blink task started");
    //LED_init();
    //LED_set_color(YELLOW);
    bool LED_OF = false;

    while(1)
    {
        if(LED_OF == false)
        {
            LED_OF = true;
            LED_OFF();
        }
        else
        {
            LED_OF = false;
            LED_SET();
        }
        vTaskDelay(xDelayBlink);
    }
}
#endif

/*
    Name: LED_OFF
    Description: turn off LED
        
    input(s):  none
    output(s): none
*/
void LED_OFF()
{
    led_strip_clear(led_strip);
}

/*
    Name: LED_SET
    Description: set LED to a given color 
        
    input(s):  none
    output(s): none
*/
void LED_SET()
{
    led_strip_set_pixel(led_strip, 0, LED.R, LED.G, LED.B);
    led_strip_refresh(led_strip);
}

/*
    Name: LED_set_color
    Description: sets the LED to a given color 
        
    input(s):  none
    output(s): none
*/
void LED_set_color(LED_COLOR color)
{
    if(color >= RGB_COLOR_NUM ) //if past enum bounds, set to white
    {
        ESP_LOGI(TAG, "Color not recognized. Setting to white.");
        LED.R = 3;
        LED.G = 3;
        LED.B = 3;
    }
    else
    {
        LED.R = LED.palette.R[color];
        LED.G = LED.palette.G[color];
        LED.B = LED.palette.B[color];
    }
}

/*
    Name: LED_init
    Description: initialize I2C LED, start LED task 
        
    input(s):  none
    output(s): none
*/
void LED_init()
{
    ledQueue = xQueueCreate(QUEUE_LEN, sizeof(LED_msg));//create queue for msgs
    ESP_LOGI(TAG, "initializing LED driver");
    /* LED strip initialization with the GPIO and pixels number*/
    strip_config.strip_gpio_num = LED_GPIO; // The GPIO that connected to the LED strip's data line
    strip_config.max_leds = 1; // The number of LEDs in the strip,
    strip_config.led_pixel_format = LED_PIXEL_FORMAT_GRB; // Pixel format of your LED strip
    strip_config.led_model = LED_MODEL_WS2812; // LED strip model
    strip_config.flags.invert_out = false; // whether to invert the output signal (useful when your hardware has a level inverter)

    rmt_config.clk_src = RMT_CLK_SRC_DEFAULT; // different clock source can lead to different power consumption
    rmt_config.resolution_hz = 10 * 1000 * 1000; // 10MHz
    rmt_config.flags.with_dma = false; // whether to enable the DMA feature

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    //init color pallete to be used (red, green, yellow)
    init_RGB_color_t();
    ESP_LOGI(TAG, "initialize LED driver complete");
    
    //start task
   xTaskCreate(&led_task,          "led task",                 2048, NULL, 5, NULL);
}

/*
    Name: init_RGB_color_t
    Description: initialize LED color palette 
        
    input(s):  none
    output(s): none
*/
void init_RGB_color_t()
{
    //Color 1: Red
    LED.palette.R[0] = 10;
    LED.palette.G[0] = 0;
    LED.palette.B[0] = 0;
    //Color 2: Green
    LED.palette.R[1] = 0;
    LED.palette.G[1] = 10;
    LED.palette.B[1] = 0;
    //Color 3: Yellow
    LED.palette.R[2] = 5;
    LED.palette.G[2] = 5;
    LED.palette.B[2] = 0;
}