#include <Arduino.h>

#include "clockTask.h"
#include "displayTask/display.h"

extern SemaphoreHandle_t lvglmutex;

static const char* weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

/** 
 * This task initializes and synchronizes the internal rtc then 
 * it displays the time and the date
 */

void clock_task(void *pvParameter)
{

  Serial.println("Clock init");
  static int lastSecond = -1;
  char timeStr[16];
  char dateStr[16];
  struct tm timeinfo;  
  time_t now;

  for (;;) {

    time(&now);
    localtime_r(&now, &timeinfo);

    int second  = timeinfo.tm_sec;
    int minute  = timeinfo.tm_min;
    int hour    = timeinfo.tm_hour;
    int day     = timeinfo.tm_mday;
    int month   = timeinfo.tm_mon + 1;
    int year    = timeinfo.tm_year + 1900;
    int weekday = timeinfo.tm_wday;

    if (second != lastSecond) {

      lastSecond = second;

      if (second % 2 == 0) {
        snprintf(timeStr, sizeof(timeStr), "%02d:%02d", hour, minute);
      } else {
        snprintf(timeStr, sizeof(timeStr), "%02d %02d", hour, minute);
      }

      snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d", day, month, year);

      xSemaphoreTake(lvglmutex, portMAX_DELAY);
      display_time(timeStr, dateStr, weekdays[weekday]);
      xSemaphoreGive(lvglmutex);
    }

    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}