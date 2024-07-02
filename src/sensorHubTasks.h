#ifndef SENSORHUBTASKS_H
#define SENSORHUBTASKS_H

#define xDelaySensor (1000 / portTICK_PERIOD_MS)
#define TEMP_HUM_GPIO GPIO_NUM_9

void tempHumTask(void *pvParameter);
void moistureTask(void *pvParameter);
void initSensorHub();
#endif