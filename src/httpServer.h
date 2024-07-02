#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include "esp_http_server.h"

#define PROFILE_CHANGE_CH1 9    // /profile/nxy*
#define NAME_CHANGE_CH1 6       // /name/txnnewName
#define TIME_CH1 6              // /time/xn
#define INFO_CH1 6              // /info/xnt
#define PORT_TYPE_CH1 10        // /portType/nxt
#define SOIL_MAX_CH1 9          // /soilMax/xn

#define PHUB_CH1 6              // /phub/xn
#define SHUB_CH1 6              // /shub/xtv
#define DAY_CH1 5               // /day/x
#define UPDATE_CH1 8            // /update/x

void init_http_server();

#endif