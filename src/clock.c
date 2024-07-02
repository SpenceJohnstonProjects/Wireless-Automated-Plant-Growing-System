#include "define.h"
#include "stdio.h"
#include "stdlib.h"
#include "esp_log.h"
#include <sys/time.h>
#include "time.h"
#include "clock.h"

static const char *TAG = "Clock";

struct tm * timeInfo;
STATIC time_t rawTime;

/*
    Name: getLocalTime
    Description: update timeInfo struct with local time and logs current time of system in following format:
    DD:HH:MM
        
    input(s):  
        DD: day pointer
        HH: hour pointer
        MM: minute pointer
    output(s): none
*/
void getLocalTime(uint8_t *DD, uint8_t *HH, uint8_t *MM)
{
    rawTime = time(NULL);
    timeInfo = localtime(&rawTime);
    if(DD != NULL)
        *DD = (uint8_t) timeInfo->tm_yday;
    if(HH != NULL)
        *HH = (uint8_t) timeInfo->tm_hour;
    if(MM != NULL)
        *MM = (uint8_t) timeInfo->tm_min;

    #ifdef DEBUG
    ESP_LOGI(TAG, "Local Time(dd:hh:mm): %02u:%02u:%02u", timeInfo->tm_yday, timeInfo->tm_hour, timeInfo->tm_min);
    #endif
}

/*
    Name: setLocalTime
    Description: sets the hours and minutes of the local time
        
    input(s):  
        HH: hour value
        MM: minute value
    output(s): none
*/
void setLocalTime(uint8_t HH, uint8_t MM)
{
    //struct to hold time info for set
    struct timeval tv;

    //update days
    rawTime = time(NULL);
    timeInfo = localtime(&rawTime);
    
    //set hour and min
    timeInfo->tm_hour = HH;
    timeInfo->tm_min = MM;
    
    //convert struct into seconds
    rawTime = mktime(timeInfo);
    
    //setup timeval
    tv.tv_sec = rawTime;
    tv.tv_usec = 0;
    
    //set local time
    settimeofday(&tv, NULL);
    ESP_LOGI(TAG, "Local Time(dd:hh:mm): %02u:%02u:%02u", timeInfo->tm_yday, timeInfo->tm_hour, timeInfo->tm_min);
}

/*
    Name: resetDays
    Description: resets the days of the local time. this is used to reset grow cycle 
        
    input(s):  none
    output(s): none
*/
void resetDays()
{
    //struct to hold time info for set
    struct timeval tv;
    struct timezone tz;

    //update time for hr and min
    rawTime = time(NULL);
    timeInfo = localtime(&rawTime);
    
    //reset the days. tm_wday and tm_yday ignored for mktime, so using mday and mon to reset the day
    timeInfo->tm_mday = 1;
    timeInfo->tm_mon = 0;
    timeInfo->tm_year = 70; //resets year to prevent overflow
    tz.tz_dsttime = DST_NONE;
    tz.tz_minuteswest = 0;
    
    //convert struct into seconds
    rawTime = mktime(timeInfo);
    
    //setup timeval
    tv.tv_sec = rawTime;
    tv.tv_usec = 0;
    
    //set local time
    settimeofday(&tv, &tz);
    ESP_LOGI(TAG, "Local Time(dd:hh:mm): %02u:%02u:%02u", timeInfo->tm_yday, timeInfo->tm_hour, timeInfo->tm_min);
}