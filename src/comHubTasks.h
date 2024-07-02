#ifndef COMMHUBTASKS_H
#define COMMHUBTASKS_H

#include "structs.h"

#define xDelayCom (500 / portTICK_PERIOD_MS)

//TASKS
void comHub_task(void *pvParameter);

//FUNCTIONS
void updateSMoistActual();
esp_err_t initComHub();

#endif