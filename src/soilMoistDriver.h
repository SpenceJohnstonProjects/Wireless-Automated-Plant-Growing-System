#ifndef SOIL_MOIST_DRIVER_H
#define SOIL_MOIST_DRIVER_H

#define SENSOR_PERIPHERAL_SOILMOIST_EN GPIO_NUM_10
#define SOIL_ADC_PORT ADC1_CHANNEL_3

void initSoilSensor();
void getMoistureLvl(uint8_t * voltage);

#endif