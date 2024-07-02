#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#define RGB_COLOR_NUM 3

//enums
//Colors supported by RGB_color_t
typedef enum {
    RED = 0,
    GREEN,
    YELLOW,
    cNULL
}LED_COLOR;

//structs
//stores all color values defined by LED_COLOR enum 
typedef struct {
    uint8_t R[RGB_COLOR_NUM];
    uint8_t G[RGB_COLOR_NUM];
    uint8_t B[RGB_COLOR_NUM];
}RGB_color_t;

//stores current values of LED and a RGB_color_t for color palette
typedef struct {
    uint8_t R;
    uint8_t G;
    uint8_t B;
    RGB_color_t palette;
}LED_data;

typedef struct {
    LED_COLOR color;
    bool isBlink;
    bool isOn;
}LED_msg;

//prototypes
void LED_init();
void init_RGB_color_t();

void blinkLED();

void LED_OFF();
void LED_SET();

void LED_set_color(LED_COLOR color);

void led_task(void *pvParameter);
#endif