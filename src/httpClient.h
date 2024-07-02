#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "stdint.h"
#include "stdbool.h"
#include "structs.h"
#include "enums.h"

#define xDelayClient (500 / portTICK_PERIOD_MS)
#define PHUB_CHANGED_CH1 24 //http://192.168.4.1/phub/nt
#define INFO_REQ_CH1 24     //http://192.168.4.1/info/pnt
#define TIME_REQ_CH1 27     //http://192.168.4.1/profile/nx0
#define S_POST_CH1 24       //http://192.168.4.1/shub/xtv
#define P_TYPE_CH1 28       //http://192.168.4.1/portType/nxt
#define UPDATE_CH1 26       //http://192.168.4.1/update/x

void http_client_init(void);

//powerhub functions
PowerLevel powerhub_client_type_level_request(PowerType requested);
bool powerhub_profile_changed_request();
bool powerhub_port_changed_request();
bool powerhub_time_changed_request();
void powerhub_time_request(uint8_t * hr, uint8_t * min);
void powerhub_time_day_request(uint8_t * day);
uint16_t timerRequest(selectTimer timer);
PowerType reqPortType(uint8_t index);
//sensorhub functions
void sensorHub_client_data_post(SensorType type, uint16_t data);
void powerhub_client_update_light_status(PowerLevel lightLvl);
#endif