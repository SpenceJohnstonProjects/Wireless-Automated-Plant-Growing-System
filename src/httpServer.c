#include "define.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include <esp_http_server.h>
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_check.h"
#include <esp_wifi.h>
#include <esp_system.h>
#include "softAP.h"
#include "httpServer.h"
#include "html.h"
#include "structs.h"
#include "LedDriver.h"
#include "clock.h"
#include "comHubTasks.h"

#define EXAMPLE_HTTP_QUERY_KEY_MAX_LEN  (64)

static const char *TAG = "http server";

extern struct tm * timeInfo;

STATIC httpd_handle_t server = NULL;

extern SensorDevice connectedSensorDevice;
extern PowerDevice connectedPowerDevice;
extern DeviceProfile profile;
extern PowerType p_portInfo[MAX_POWER_PORTS];
extern bool hasProfileChanged;
extern bool hasPowerPortChanged;

/*
    function: set soil moisture max value based on calibration. this is needed for percentage based pump trigger based on soil moisture %

    format: /soilMax/x
            x = g/s = get / set
    note: soil moisture value for set is the last polled value from soil moisture sensor
*/
static esp_err_t soilMaxHandler(httpd_req_t *req)
{
    switch(req->uri[SOIL_MAX_CH1]) //get/set
    {
        case 's': //set
            //get value from uri
            profile.sMoist.maxSoilMoistCalibration = profile.device_s->soilMoist;

            //error handling if calibration value is completely dry
            if(profile.sMoist.maxSoilMoistCalibration == 0)
            {
                ESP_LOGI(TAG, "soil moisture max calibration value error");
                httpd_resp_send(req, "error", strlen("error"));
                return ESP_FAIL;
            }

            updateSMoistActual();
            ESP_LOGI(TAG, "soil moisture max calibration value: %u", profile.sMoist.maxSoilMoistCalibration);
            httpd_resp_send(req, "T", 1);
        break;

        case 'g': //get
            char buffer[STR_LEN] = { '\0' };
            sprintf(buffer, "%u", profile.sMoist.maxSoilMoistCalibration);
            httpd_resp_send(req, buffer, strlen(buffer));
        break;
        default:
        ESP_LOGI(TAG, "soil max request not valid");
        httpd_resp_send(req, "error", strlen("error"));
        return ESP_FAIL;
    }
    return ESP_OK;
}
static const httpd_uri_t SoilMax = {
    .uri = "/soilMax/*",
    .method = HTTP_GET,
    .handler = soilMaxHandler,
    .user_ctx = NULL
};

/*
    function: handles system time with the following URI format:
        /xn
        x = g/s = get or set
        n = new system time = HH:MM 

    URI: /time/xn
*/
static esp_err_t timeHandler(httpd_req_t *req)
{
    uint8_t HH;
    uint8_t MM;

    switch(req->uri[TIME_CH1])
    {
    case 'g'://get
        char buffer[8];
        memset(buffer, '\0', 6); //init buffer
        getLocalTime(NULL, &HH, &MM); //refresh timeInfo
        sprintf(buffer, "%02u:%02u", HH, MM);//fill buffer with HH:MM
        ESP_LOGI(TAG, "current local time: %u:%u", HH, MM);
        httpd_resp_send(req, buffer, strlen(buffer));
    break;
    case 's'://set
        HH = atoi(&req->uri[TIME_CH1 + 1]);
        MM = atoi(&req->uri[TIME_CH1 + 4]);
        ESP_LOGI(TAG, "new local time: %u:%u", HH, MM);
        setLocalTime(HH, MM);
        httpd_resp_send(req, "T", 1);  
    break;
    default:
        ESP_LOGI(TAG, "uknown URI. TimeHandler");
        return ESP_FAIL;
    }
    return ESP_OK;
}
static const httpd_uri_t Time = {
    .uri = "/time/*",
    .method = HTTP_GET,
    .handler = timeHandler,
    .user_ctx = NULL
};

/*
    Purpose: get/set the name of a profile device.
    format:
        /name/txnnewName
            t = g/s = get or set name
            x = p/s = power or sensor device
            n = index of device
            newName = the new name of the device
*/
static esp_err_t nameHandler(httpd_req_t *req)
{
    //determine if request is for power or sensor type
    switch(req->uri[NAME_CHANGE_CH1])
    {
        case 's': //set
            switch(req->uri[NAME_CHANGE_CH1 + 1])
            {
                case 'p': //power device
                    memset(profile.device_p_info.name, '\0', STR_LEN);
                    memcpy(profile.device_p_info.name, &req->uri[NAME_CHANGE_CH1 + 3], strlen(&req->uri[NAME_CHANGE_CH1 + 3]));
            ESP_LOGI(TAG, "Value of power device name: %s", profile.device_p_info.name);
                break;
                case 's': //sensor device
                    memset(profile.device_s_info.name, '\0', STR_LEN);
                    memcpy(profile.device_s_info.name, &req->uri[NAME_CHANGE_CH1 + 3], strlen(&req->uri[NAME_CHANGE_CH1 + 3]));
                    ESP_LOGI(TAG, "Value of sensor device name: %s", profile.device_p_info.name);
                break;
                default:
                    ESP_LOGI(TAG, "device type not recognized");
                    httpd_resp_send(req, "error", strlen("error"));
                    return ESP_FAIL;
            }            
            httpd_resp_send(req, "T", strlen("T"));
        break;
        case 'g':
            switch(req->uri[NAME_CHANGE_CH1 + 1])
            {
                case 'p':
                    httpd_resp_send(req, profile.device_p_info.name, strlen(profile.device_p_info.name));
                break;
                case 's':
                    httpd_resp_send(req, profile.device_s_info.name, strlen(profile.device_s_info.name));
                break;
                default:
                    ESP_LOGI(TAG, "device type not recognized");
                    httpd_resp_send(req, "error", strlen("error"));
                    return ESP_FAIL;
            }
        break;
        default:
            ESP_LOGI(TAG, "first character not recognized");
            httpd_resp_send(req, "error", strlen("error"));
            return ESP_FAIL;
    }  
    return ESP_OK;
}
static const httpd_uri_t Name = {
    .uri = "/name/*",
    .method = HTTP_GET,
    .handler = nameHandler,
    .user_ctx = NULL
};

/*
    name: info handler

    GET request format: /info/xnt
        first character: x: S = get # connected Sensor modules
                            P = get # connected Power module
                            s = get sensor data of device n
                            p = get power level of a given type
        
        second character:   n = Device # based on previous character
                            
        third character:    t = type
    
    Output: the desired value as a string.
*/
static esp_err_t infoHandler(httpd_req_t *req)
{
    char buffer[STR_LEN] = { '\0' };
    switch(req->uri[INFO_CH1]) //first character of wildcard
    {  
        case 'S': httpd_resp_send(req, "1", strlen("1")); break;
        case 'P': httpd_resp_send(req, "1", strlen("1")); break;
        
        case 's': //sensor data of type, device N
            switch(req->uri[INFO_CH1 + 2] - 48)
            {
            case Temp:
                sprintf(buffer, "%u", profile.device_s->temp);
                ESP_LOGI(TAG, "temp data sent: %s", buffer);
            break;
            case Hum:
                sprintf(buffer, "%u", profile.device_s->hum);
                ESP_LOGI(TAG, "humidity data sent: %s", buffer);
            break;
            case SoilMoist:
                sprintf(buffer, "%u", ((profile.device_s->soilMoist * 100) / profile.sMoist.maxSoilMoistCalibration));
                ESP_LOGI(TAG, "soil moisture data sent: %s", buffer);
            break;
            default:
                ESP_LOGI(TAG, "invalid command character 3");
                httpd_resp_send(req, "error", strlen("error")); 
                return ESP_FAIL;
            }
            httpd_resp_send(req, buffer, strlen(buffer));
        break;
        
        case 'p'://power data of type, device N
            switch(req->uri[INFO_CH1 + 2])
            {
            case 'f':
                buffer[0] = profile.device_p->fanIs + 48;
                ESP_LOGI(TAG, "fan power state sent: %s", buffer);
            break;
            case 'l':
                buffer[0] = profile.device_p->lightIs + 48;
                ESP_LOGI(TAG, "light power state sent: %s", buffer);
            break;
            case 'p':
                buffer[0] = profile.device_p->pumpIs + 48;
                ESP_LOGI(TAG, "pump power state sent: %s", buffer);
            break;
            case 'n':
                buffer[0] = OFF + 48;
                ESP_LOGI(TAG, "none power state sent: %s", buffer);
            break;
            default:
                ESP_LOGI(TAG, "invalid command character 3");
                httpd_resp_send(req, "error", strlen("error")); 
                return ESP_FAIL;
            }
            httpd_resp_send(req, buffer, strlen(buffer));
        break;
        
        default:
            ESP_LOGI(TAG, "invalid command character 1");
            httpd_resp_send(req, "error", strlen("error"));
            return ESP_FAIL;
    }
    return ESP_OK;
}
static const httpd_uri_t Info = {
    .uri = "/info/*",
    .method = HTTP_GET,
    .handler = infoHandler,
    .user_ctx = NULL
};

/*
    name: profile handler

    GET request format:
        first character     n = profile number

        second character    0 = primary light duration(hr:min)
                            1 = primary light start time(hr:min)
                            2 = primary light duration(days)
                            3 = secondary light duration(hr:min)
                            4 = secondary light start time(hr:min)
                            5 = secondary light duration(days)
                            6 = fan humidy threshold(%)
                            7 = fan temperature threshold(%)
                            8 = pump water moisture threshold(%)
        
        third character     g = get
                            s = set
        
        four - n character:    n = value to set if previous character is set

        Expected URI: /profile/nxy*
    
    Output: the desired value as a string.
*/
static esp_err_t profileHandler(httpd_req_t *req)
{
    char buffer[10] = { '\0' };

    ESP_LOGI(TAG, "URI INPUT: %s", req->uri);
    
    if('g' == req->uri[PROFILE_CHANGE_CH1 + 2]) // get
    {
        switch(req->uri[PROFILE_CHANGE_CH1 + 1]) //second character of wildcard
        {  
            case '0': //primary light duration(hr:min)
                sprintf(buffer, "%02u:%02u", (profile.timer.lightTimer_m[0] / 60), (profile.timer.lightTimer_m[0] % 60));
                ESP_LOGI(TAG, "Primary Light Duration(m): %s", buffer);
                break;
            case '1': //primary light start time(hr:min)
                sprintf(buffer, "%02u:%02u", profile.timer.lightStartHr[0], profile.timer.lightStartMin[0]);
                ESP_LOGI(TAG, "Primary Light start time: %s", buffer);
                break;
            case '2': //primary light duration(days)
                sprintf(buffer, "%u", profile.timer.lightTimer_d[0]);
                ESP_LOGI(TAG, "Primary Light Duration(d): %s", buffer);
                break;
            case '3': //secondary light duration(hr:min)
                sprintf(buffer, "%02u:%02u", (profile.timer.lightTimer_m[1] / 60), (profile.timer.lightTimer_m[1] % 60));
                ESP_LOGI(TAG, "Secondary Light Duration(s): %s", buffer);
                break;
            case '4': //secondary light start time(hr:min)
                sprintf(buffer, "%02u:%02u", profile.timer.lightStartHr[1], profile.timer.lightStartMin[1]);
                ESP_LOGI(TAG, "Secondary Light start time: %s", buffer);
                break;
            case '5': //secondary light duration(days)
                sprintf(buffer, "%u", profile.timer.lightTimer_d[1]);
                ESP_LOGI(TAG, "Secondary Light Duration(d): %s", buffer);
                break;
            case '6': //fan humidy threshold(%)
                sprintf(buffer, "%u", profile.hum.maxHum);
                ESP_LOGI(TAG, "Fan Humidity max threshold: %s", buffer);
                break;
            case '7': //fan temperature threshold(%)
                sprintf(buffer, "%u", profile.temp.maxTemp);
                ESP_LOGI(TAG, "Fan temperature max threshold: %s", buffer);
                break;
            case '8': //pump water moisture threshold(%)
                sprintf(buffer, "%u", profile.sMoist.minSoilMoistPercent);
                updateSMoistActual();
                ESP_LOGI(TAG, "water pump soil moisture min threshold: %s", buffer);
                break;
            default:
                sprintf(buffer, "Error");
                ESP_LOGI(TAG, "Request ERROR");
                httpd_resp_send(req, buffer, strlen(buffer));
                return ESP_FAIL;
        }
        httpd_resp_send(req, buffer, strlen(buffer));
        return ESP_OK;
    }
    else if ('s' == req->uri[PROFILE_CHANGE_CH1 + 2])    //set
    {
        switch(req->uri[PROFILE_CHANGE_CH1 + 1]) //second character of wildcard
        {  
            case '0': //primary light duration(hr:min)
                profile.timer.lightTimer_m[0] = (atoi(&req->uri[PROFILE_CHANGE_CH1 + 3]) * 60) + (atoi(&req->uri[PROFILE_CHANGE_CH1 + 6]));
                ESP_LOGI(TAG, "Primary light duration: %u minutes", profile.timer.lightTimer_m[0]);
                break;
            case '1': //primary light start time(hr:min)
                profile.timer.lightStartHr[0] = atoi(&req->uri[PROFILE_CHANGE_CH1 + 3]);
                profile.timer.lightStartMin[0] = atoi(&req->uri[PROFILE_CHANGE_CH1 + 6]);
                ESP_LOGI(TAG, "Primary light start time: %u:%u", profile.timer.lightStartHr[0], profile.timer.lightStartMin[0]);
                break;
            case '2': //primary light duration(days)
                profile.timer.lightTimer_d[0] = atoi(&req->uri[PROFILE_CHANGE_CH1 + 3]);
                ESP_LOGI(TAG, "Primary light cycle in days: %u", profile.timer.lightTimer_d[0]);
                break;
            case '3': //secondary light duration(hr:min)
                profile.timer.lightTimer_m[1] = (atoi(&req->uri[PROFILE_CHANGE_CH1 + 3]) * 60 + (atoi(&req->uri[PROFILE_CHANGE_CH1 + 6])));
                ESP_LOGI(TAG, "Secondary light duration: %u minutes", profile.timer.lightTimer_m[1]);
                break;
            case '4': //secondary light start time(hr:min)
                profile.timer.lightStartHr[1] = atoi(&req->uri[PROFILE_CHANGE_CH1 + 3]);
                profile.timer.lightStartMin[1] = atoi(&req->uri[PROFILE_CHANGE_CH1 + 6]);
                ESP_LOGI(TAG, "Secondary light start time: %u:%u", profile.timer.lightStartHr[1], profile.timer.lightStartMin[1]);
                break;
            case '5': //secondary light duration(days)
                profile.timer.lightTimer_d[1] = atoi(&req->uri[PROFILE_CHANGE_CH1 + 3]);
                ESP_LOGI(TAG, "Secondary light cycle in days: %u", profile.timer.lightTimer_d[1]);
                break;
            case '6': //fan humidy threshold(%)
                profile.hum.maxHum = atoi(&req->uri[PROFILE_CHANGE_CH1 + 3]);
                ESP_LOGI(TAG, "Humidity max threshold: %u", profile.hum.maxHum);
                break;
            case '7': //fan temperature threshold(%)
                profile.temp.maxTemp = atoi(&req->uri[PROFILE_CHANGE_CH1 + 3]);
                ESP_LOGI(TAG, "Temperature max threshold: %u", profile.temp.maxTemp);
                break;
            case '8': //pump water moisture threshold(%)
                profile.sMoist.minSoilMoistPercent = atoi(&req->uri[PROFILE_CHANGE_CH1 + 3]);
                ESP_LOGI(TAG, "Soil moisture min thresholdL %u", profile.sMoist.minSoilMoistPercent);
                updateSMoistActual();
                break;
            default:
                sprintf(buffer, "error");
                httpd_resp_send(req, buffer, strlen(buffer));
                return ESP_FAIL;
        }
        buffer[0] = 'T'; //successful set msg
        hasProfileChanged = true;
        httpd_resp_send(req, buffer, strlen(buffer));
        return ESP_OK;
    }
    else
    {
        httpd_resp_send(req, "Error", strlen("Error"));
        ESP_LOGI(TAG, "not a get or set URI");
        return ESP_FAIL;
    }
}
static const httpd_uri_t Profile = {
    .uri = "/profile/*",
    .method = HTTP_GET,
    .handler = profileHandler,
    .user_ctx = NULL
};

/*
    get or set port power type. sets changed flag if setting

    format: /nxyT
            n = connected powerdevice #
            x = index (0 - 2)
            y = get/ set
                g = get
                s = set 
            T = type = f/l/p
                f = fan
                l = light
                p = pump
                n = none

    expected URI: /portType/nxt
*/
static esp_err_t portTypeHandler(httpd_req_t *req)
{
    if((req->uri[PORT_TYPE_CH1 + 1] - 48) > MAX_POWER_PORTS)
    {
        httpd_resp_send(req, "Error", strlen("Error"));
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Port type cmd sent: %s", req->uri);
    if(req->uri[PORT_TYPE_CH1 + 2] == 's')//set
    {
        switch(req->uri[PORT_TYPE_CH1 + 3]) //type
        {
        case 'f': //fan
            p_portInfo[req->uri[PORT_TYPE_CH1 + 1] - 48] = FAN;
        break;
        case 'l': //light
            p_portInfo[req->uri[PORT_TYPE_CH1 + 1] - 48] = LIGHT;
        break;
        case 'p': //pump
            p_portInfo[req->uri[PORT_TYPE_CH1 + 1] - 48] = PUMP;
        break;
        case 'n': //none
            p_portInfo[req->uri[PORT_TYPE_CH1 + 1] - 48] = NONE;
        break;
        default:
            httpd_resp_send(req, "Error", strlen("Error"));
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "new port value: %d", p_portInfo[req->uri[PORT_TYPE_CH1 + 1] - 48]);
        httpd_resp_send(req, "T", strlen("T"));
        hasPowerPortChanged = true;
        return ESP_OK;
    }
    else //get
    {
        switch( p_portInfo[req->uri[PORT_TYPE_CH1 + 1] - 48])
        {
        case FAN:
            httpd_resp_send(req, "f", strlen("f"));
        break;
        case LIGHT:
            httpd_resp_send(req, "l", strlen("l"));
        break;
        case PUMP:
            httpd_resp_send(req, "p", strlen("p"));
        break;
        case NONE:
            httpd_resp_send(req, "n", strlen("n"));
        break;
        default:
            httpd_resp_send(req, "Error", strlen("Error"));
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "port %d type: %d", req->uri[PORT_TYPE_CH1 + 1] - 48, p_portInfo[req->uri[PORT_TYPE_CH1 + 1] - 48]);
        return ESP_OK;
    }
}
static const httpd_uri_t PortType = {
    .uri = "/portType/*",
    .method = HTTP_GET,
    .handler = portTypeHandler,
    .user_ctx = NULL
};

/*
    updates light status from powerhub.
    x = updated power level from com hub
    /update/x
*/
static esp_err_t updatePeripheralStatusHandle(httpd_req_t *req)
{
    if(req->uri[UPDATE_CH1] - 48 >= POWER_TYPE_MAX)
    {
        httpd_resp_send(req, "ERROR", strlen("error"));
        return ESP_FAIL;
    }
    profile.device_p->lightIs = req->uri[UPDATE_CH1] - 48;
    httpd_resp_send(req, "T", strlen("T"));
    return ESP_OK;
}
static const httpd_uri_t UpdatePeripheralStatus = {
    .uri = "/update/*",
    .method = HTTP_GET,
    .handler = updatePeripheralStatusHandle,
    .user_ctx = NULL
};

//PAGE URI STUFF
#ifdef COMM
//html and css page handler
static esp_err_t page_handler(httpd_req_t *req)
{
    esp_err_t error;
    ESP_LOGI(TAG, "page URI handler");
    const char * response = (const char *) req->user_ctx;
    error = httpd_resp_send(req, response, strlen(response));
    if(error != ESP_OK)
        ESP_LOGI(TAG, "ERROR %d while sending response", error);
    else
        ESP_LOGI(TAG, "Response sent successfully!");
    return error;
}

static esp_err_t css_handler(httpd_req_t *req)
{
    esp_err_t error;
    ESP_LOGI(TAG, "css URI handler");
    const char * response = (const char *) req->user_ctx;
    httpd_resp_set_type(req, "text/css");
    error = httpd_resp_send(req, response, strlen(response));
    
    if(error != ESP_OK)
        ESP_LOGI(TAG, "ERROR %d while sending response", error);
    else
        ESP_LOGI(TAG, "Response sent successfully!");
    httpd_resp_set_type(req, "text/html");
    return error;
}
static const httpd_uri_t index_html = {
    .uri = "/index.html",
    .method = HTTP_GET,
    .handler = page_handler,
    .user_ctx = INDEX_HTML
};
static const httpd_uri_t group_html = {
    .uri = "/group.html",
    .method = HTTP_GET,
    .handler = page_handler,
    .user_ctx = GROUP_HTML
};
static const httpd_uri_t profile_html = {
    .uri = "/profile.html",
    .method = HTTP_GET,
    .handler = page_handler,
    .user_ctx = PROFILE_HTML
};
static const httpd_uri_t setting_html = {
    .uri = "/setting.html",
    .method = HTTP_GET,
    .handler = page_handler,
    .user_ctx = SETTINGS_HTML
};
static const httpd_uri_t device_html = {
    .uri = "/device.html",
    .method = HTTP_GET,
    .handler = page_handler,
    .user_ctx = DEVICES_HTML
};
static const httpd_uri_t style_css = {
    .uri = "/style.css",
    .method = HTTP_GET,
    .handler = css_handler,
    .user_ctx = STYLE_CSS
};
#endif
//power hub uris

/*
    see if profile values or port types have changed
    uri: /phub/xn
    format: /xn
            x = profile number
            n = profile or port
                p = profile
                P = port
                
*/
static esp_err_t powerHubChangedHandler(httpd_req_t *req)
{
    switch(req->uri[PHUB_CH1 + 1])
    {
    case 'p':
        if(hasProfileChanged == true)
        {
            httpd_resp_send(req, "T", strlen("T"));
            hasProfileChanged = false;
            ESP_LOGI(TAG, "Power profile has changed");
        }
        else
        {
            httpd_resp_send(req, "F", strlen("F"));
            ESP_LOGI(TAG, "Power profile has not changed");
        }
    break;
    case 'P':
        if(hasPowerPortChanged == true)
        {
            httpd_resp_send(req, "T", strlen("T"));
            hasPowerPortChanged = false;
            ESP_LOGI(TAG, "Power port has changed");
        }
        else
        {
            httpd_resp_send(req, "F", strlen("F"));
            ESP_LOGI(TAG, "Power port has not changed");
        }
    break;
    default:
        httpd_resp_send(req, "error", strlen("error"));
        return ESP_FAIL;
    }
    return ESP_OK;
}
static const httpd_uri_t PowerHubChanged = {
    .uri = "/phub/*",
    .method = HTTP_GET,
    .handler = powerHubChangedHandler,
    .user_ctx = NULL
};

/*
    gets the day or resets the day
    expected uri: /day/x
        /x = g/s
        g = get
        s = reset
*/
static esp_err_t dayHandler(httpd_req_t *req)
{
    switch(req->uri[DAY_CH1])
    {
    case 'g':
        char buffer[10];
        uint8_t days;
        getLocalTime(&days, NULL, NULL);
        sprintf(buffer, "%d", days);
        httpd_resp_send(req, buffer, strlen(buffer));
    break;
    case 's':
        resetDays();
        httpd_resp_send(req, "T", strlen("T"));
    break;
    default:
        httpd_resp_send(req, "ERROR", strlen("error"));
        return ESP_FAIL;
    }
    return ESP_OK;
}
static const httpd_uri_t Day = {
    .uri = "/day/*",
    .method = HTTP_GET,
    .handler = dayHandler,
    .user_ctx = NULL
};

//sensor hub uris

/*
    updates value of sensors
    uri: /shub/xtv
    format: /xtv
        x = profile/device number
        t = type (see enum)
            Hum = humidity
            Temp = temperature
            SoilMoist = soil moisture
        v = value of new data

*/
static esp_err_t sensorHubHandler(httpd_req_t *req)
{
    ESP_LOGE(TAG, "Sensor data URI send: %s", req->uri);
    switch(req->uri[SHUB_CH1 + 1] - 48) //type
    {
    case Hum:
        profile.device_s->hum = atoi(&req->uri[SHUB_CH1 + 2]);
    break;
    case Temp:
        profile.device_s->temp = atoi(&req->uri[SHUB_CH1 + 2]);
    break;
    case SoilMoist:
        profile.device_s->soilMoist = atoi(&req->uri[SHUB_CH1 + 2]);
    break;    
    default:
        httpd_resp_send(req, "error", strlen("error"));
        return ESP_FAIL;
    }
    httpd_resp_send(req, "T", strlen("T"));
    return ESP_OK;
}
static const httpd_uri_t SensorHub = {
    .uri = "/shub/*",
    .method = HTTP_GET,
    .handler = sensorHubHandler,
    .user_ctx = NULL
};

/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Error 404: not found");
    return ESP_FAIL;
}

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 20;
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        //general/UI
        httpd_register_uri_handler(server, &SoilMax);
        httpd_register_uri_handler(server, &Time);
        httpd_register_uri_handler(server, &Name);
        httpd_register_uri_handler(server, &Info);
        httpd_register_uri_handler(server, &Profile);
        httpd_register_uri_handler(server, &PortType);
        //html pages
        //need ifdef for compilation
        #ifdef COMM
        httpd_register_uri_handler(server, &index_html);
        httpd_register_uri_handler(server, &group_html);
        httpd_register_uri_handler(server, &profile_html);
        httpd_register_uri_handler(server, &setting_html);
        httpd_register_uri_handler(server, &device_html);
        httpd_register_uri_handler(server, &style_css);
        #endif
        //power hub specifics
        httpd_register_uri_handler(server, &PowerHubChanged);
        httpd_register_uri_handler(server, &Day);
        httpd_register_uri_handler(server, &UpdatePeripheralStatus);
        //sensor hub specifics
        httpd_register_uri_handler(server, &SensorHub);

        return server;
    }
    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

#if !CONFIG_IDF_TARGET_LINUX
static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop http server");
        }
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}
#endif // !CONFIG_IDF_TARGET_LINUX

void init_http_server()
{
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
}