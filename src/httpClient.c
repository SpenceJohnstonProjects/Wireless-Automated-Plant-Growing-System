#include "define.h"
#include "httpClient.h"
#include "enums.h"
#include "structs.h" 

#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "station.h"

#include "esp_http_client.h"

#define CONFIG_EXAMPLE_HTTP_ENDPOINT "192.168.4.1"

#define MAX_HTTP_RECV_BUFFER 100
#define MAX_HTTP_OUTPUT_BUFFER 100
static const char *TAG = "HTTP_CLIENT";

#ifdef POWER
#include "powerHubTasks.h"
extern PowerDeviceStates states;
#endif
#ifdef SENSOR
#include "sensorHubTasks.h"
#endif

#ifdef POWER
/*
    Purpose: get powerlevel of a given type
    param: requested = power type of wanted power level
    output: power level of requested power type
*/
PowerLevel powerhub_client_type_level_request(PowerType requested)
{
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = { '\0' };   // Buffer to store response of http request
    int content_length = 0;
    int data_read;
    PowerLevel returnLevel = ERROR;
    
    char URL_buff[] = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/info/pnt";
    switch(requested)
    {
    case FAN:
        URL_buff[INFO_REQ_CH1 + 2] = 'f';
    break;
    case PUMP:
        URL_buff[INFO_REQ_CH1 + 2] = 'p';
    break;
    case LIGHT:
        URL_buff[INFO_REQ_CH1 + 2] = 'l';
    break;
    case NONE:
        URL_buff[INFO_REQ_CH1 + 2] = 'n';
    break;
    }
    

    ESP_LOGI(TAG, "URI request: %s", URL_buff);

    esp_http_client_config_t config = {
        .url = URL_buff ,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_GET);
    esp_err_t err; 

    // GET Request
    err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
    } else {
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
            esp_http_client_cleanup(client);
        } 
        else {
                data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                //handle response here
                returnLevel = output_buffer[0] - 48;
                ESP_LOGI(TAG, "Response of port type: %d = %d", requested, returnLevel);
            } else {
                ESP_LOGE(TAG, "Failed to read response");
                esp_http_client_cleanup(client);
            }
        }
    }
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    return returnLevel;
}

//check if Power profile have changed
bool powerhub_profile_changed_request()
{
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = { '\0' };   // Buffer to store response of http request
    int content_length = 0;
    int data_read;
    bool hasChanged = false;

    esp_http_client_config_t config = {
        .url = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/phub/xp" ,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_GET);

    // GET Request
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return false;
    } else {
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
            esp_http_client_cleanup(client);
            return false;
        } 
        else {
                data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                //handle response here
                if(output_buffer[0] == 'T')
                    hasChanged = true;
                else
                    hasChanged = false;
                ESP_LOGI(TAG, "Response of profile change: %d", hasChanged);
            } else {
                ESP_LOGE(TAG, "Failed to read response");
                esp_http_client_cleanup(client);
            }
        }
    }
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    return hasChanged;
}
//check if power ports have changed
bool powerhub_port_changed_request()
{
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = { '\0' };   // Buffer to store response of http request
    int content_length = 0;
    int data_read;
    bool hasChanged = false;

    esp_http_client_config_t config = {
        .url = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/phub/xP" ,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_GET);

    // GET Request
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return false;
    } else {
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
            esp_http_client_cleanup(client);
            return false;
        } 
        else {
                data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                //handle response here
                if(output_buffer[0] == 'T')
                    hasChanged = true;
                else
                    hasChanged = false;
                ESP_LOGI(TAG, "Response of port change: %d", hasChanged);
            } else {
                ESP_LOGE(TAG, "Failed to read response");
                esp_http_client_cleanup(client);
            }
        }
    }
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    return hasChanged;
}
//timer profile info requests. singular based on enum index. call from powerhubTask multiple times instead of from here.
uint16_t timerRequest(selectTimer timer)
{
    uint16_t returnData = 0;
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = { '\0' };   // Buffer to store response of http request
    int content_length = 0;
    int data_read;
    char url_buffer[] = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/profile/nxg";
    switch(timer)
    {
        case LIGHT_START_HR_C1:
        case LIGHT_START_MIN_C1: 
        url_buffer[TIME_REQ_CH1 + 1] = '1';
        break;
        case LIGHT_START_HR_C2:
        case LIGHT_START_MIN_C2:
        url_buffer[TIME_REQ_CH1 + 1] = '4';
        break;
        case LIGHT_TIMER_D_C1:
        url_buffer[TIME_REQ_CH1 + 1] = '2';
        break;
        case LIGHT_TIMER_D_C2:
        url_buffer[TIME_REQ_CH1 + 1] = '5';
        break;
        case LIGHT_TIMER_MIN_C1:
        url_buffer[TIME_REQ_CH1 + 1] = '0';
        break;
        case LIGHT_TIMER_MIN_C2:
        url_buffer[TIME_REQ_CH1 + 1] = '3';
        break;
    }
    esp_http_client_config_t config = {
        .url = url_buffer ,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_GET);
    // GET Request
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return 0;
    } else {
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
            esp_http_client_cleanup(client);
            return 0;
        } 
        else {
                data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                //handle response here
                switch(timer)
                {
                    case LIGHT_START_HR_C1://first set of #s
                    case LIGHT_START_HR_C2://first set of #s
                    case LIGHT_TIMER_D_C1:
                    case LIGHT_TIMER_D_C2:
                    returnData =(uint16_t) strtol(output_buffer, NULL, 10);
                    break;
                    case LIGHT_START_MIN_C1://second set of #s
                    case LIGHT_START_MIN_C2://second set of #s
                    returnData =(uint16_t) strtol(&output_buffer[3], NULL, 10);
                    break;
                    case LIGHT_TIMER_MIN_C1:
                    case LIGHT_TIMER_MIN_C2:
                        returnData =((uint16_t) strtol(output_buffer, NULL, 10) *60) + ((uint16_t) strtol(&output_buffer[3], NULL, 10));
                    break;
                }
                ESP_LOGI(TAG, "Response of timer get: %d", returnData);
            } else {
                ESP_LOGE(TAG, "Failed to read response");
                esp_http_client_cleanup(client);
            }
        }
    }
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    return returnData;
}
//power port info requests. singular based on index. call from powerhubTask multiple times instead of from here
PowerType reqPortType(uint8_t index)
{
    PowerType returnData = NONE;
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = { '\0' };   // Buffer to store response of http request
    int content_length = 0;
    int data_read;
    char url_buffer[] = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/portType/nxg";
    url_buffer[P_TYPE_CH1 + 1] = index + 48;
    esp_http_client_config_t config = {
        .url = url_buffer ,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_GET);
    // GET Request
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return 0;
    } else {
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
            esp_http_client_cleanup(client);
            return 0;
        } 
        else {
                data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                //handle response here
                switch(output_buffer[0])
                {
                case 'f':
                    returnData = FAN;
                break;
                case 'l':
                    returnData = LIGHT;
                break;
                case 'p':
                    returnData = PUMP;
                break;
                case 'n':
                    returnData = NONE;
                break;
                default: //error msg
                    returnData = NONE;
                }
                
                ESP_LOGI(TAG, "Response of timer get: %d", returnData);
            } else {
                ESP_LOGE(TAG, "Failed to read response");
                esp_http_client_cleanup(client);
            }
        }
    }
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    return returnData;
}

//request system time
void powerhub_time_day_request(uint8_t * day)
{
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = { '\0' };   // Buffer to store response of http request
    int content_length = 0;
    int data_read;
    esp_http_client_config_t config = {
        .url = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/day/g" ,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_GET);
     // GET Request
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return;
    } else {
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
            esp_http_client_cleanup(client);
            return;
        } 
        else {
                data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                //handle response here
                *day =(uint8_t) strtol(output_buffer, NULL, 10);
                ESP_LOGI(TAG, "Response of day get: %u", *day);
            } else {
                ESP_LOGE(TAG, "Failed to read response");
                esp_http_client_cleanup(client);
            }
        }
    }
    esp_http_client_close(client); //might not need
    esp_http_client_cleanup(client);
}
//TODO implimentation. just pulled code from function above. maybe refactor to include DD:HH:MM instead of just HH:MM
void powerhub_time_request(uint8_t * hr, uint8_t * min)
{
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = { '\0' };   // Buffer to store response of http request
    int content_length = 0;
    int data_read;
    esp_http_client_config_t config = {
        .url = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/time/g" ,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_GET);
     // GET Request
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return;
    } else {
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
            esp_http_client_cleanup(client);
            return;
        }
        else {
                data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                //handle response here
                *hr =(uint8_t) strtol(output_buffer, NULL, 10);
                *min = (uint8_t) strtol(&output_buffer[3], NULL, 10);
                ESP_LOGI(TAG, "Response of time get: %s", output_buffer);
                ESP_LOGI(TAG, "setting clock to: %02d : %02d", *hr, *min);
            } else {
                ESP_LOGE(TAG, "Failed to read response");
                esp_http_client_cleanup(client);
            }
        }
    }
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
}

void powerhub_client_update_light_status(PowerLevel lightLvl)
{
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = { '\0' };   // Buffer to store response of http request
    int content_length = 0;
    int data_read;
    char url_buffer[] = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/update/x";
    url_buffer[UPDATE_CH1] = lightLvl + 48;
    esp_http_client_config_t config = {
        .url = url_buffer ,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_GET);
    // GET Request
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return;
    } else {
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
            esp_http_client_cleanup(client);
            return;
        }
        else {
                data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
            } else {
                ESP_LOGE(TAG, "Failed to read response");
                esp_http_client_cleanup(client);
            }
        }
    }
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    return;
}

#endif

#ifdef SENSOR
void sensorHub_client_data_post(SensorType type, uint16_t data)
{
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = { '\0' };   // Buffer to store response of http request
    int content_length = 0;
    int data_read;
    
    char URL_buff[STR_LEN] = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/shub/xt";
    URL_buff[S_POST_CH1 + 1] = type + 48;
    sprintf(&URL_buff[S_POST_CH1 + 2], "%u", data);
    esp_http_client_config_t config = {
        .url = URL_buff ,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_GET);
    esp_err_t err; 

    // GET Request
    err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
    } else {
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
            esp_http_client_cleanup(client);
        } 
        else {
                data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
            } else {
                ESP_LOGE(TAG, "Failed to read response");
                esp_http_client_cleanup(client);
            }
        }
    }
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
}
#endif
/*
    Name: http_client_init
    Description: initialize http client
        
    input(s):  none
    output(s): none
*/
void http_client_init(void)
{
    wifi_init_sta();
    ESP_LOGE(TAG, "HTTP client initialized");
}