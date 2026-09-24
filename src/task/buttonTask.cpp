#include <Arduino.h>

#include "buttonTask.h"
#include "settings.h"
#include "webserver/alarmDataParsing.h"

bool AlarmTriggered = false;

enum {IDLE, SCREEN_ON, ALARM_ON} SystemState;

/**
 * This task reads the button input, changes the brightness
 * when it's pressed and if there's an alarm activeted
 * then the button stops the bell, moreover it checks if
 * any alarm must ring. (Finite State Machine logic)
 */

void checkAlarms() {
    static int lastMinute = -1;

    struct tm timeinfo;  
    time_t now;

    time(&now);
    localtime_r(&now, &timeinfo);
    
    if (timeinfo.tm_min == lastMinute) return;
    lastMinute = timeinfo.tm_min;

    for (int i = 0; i < 3; i++) {
        if (!alarms_data[i].enabled) continue;
        if (!(alarms_data[i].days & (1 << timeinfo.tm_wday))) continue;
        if (alarms_data[i].hour   != timeinfo.tm_hour) continue;
        if (alarms_data[i].minute != timeinfo.tm_min) continue;

        AlarmTriggered = true;
        return; 
    }
}

void button_task(void *pvParameter) {
    /* screen var */
    int brightness = 1;
    int lastBrightness = 0;

    /* timer var */
    unsigned long lastTimeAlarmChecked = 0;   /* Alarm Checked */
    unsigned long lastTimeAlarmStarted = 0;   /* Alarm Started */
    unsigned long lastTimeScreenOn = 0;       /* Screen ON */

    /* button reading debounce var */
    static int stableCount = 0;
    static int lastState = HIGH;
    static bool handled = false;

    pinMode(BUTTON, INPUT_PULLUP);

    SystemState = IDLE; 

    for (;;) {
        unsigned long now = millis();
        bool buttonPressed = false;

        /* button reading logic with debounce */
        int reading = digitalRead(BUTTON);

        if (reading == lastState) {
            if (stableCount < 3)
                stableCount++;
        } else {
            stableCount = 0;
            handled = false;
            lastState = reading;
        }

        if (stableCount == 3 && reading == LOW && !handled) {
            buttonPressed = true;
            handled = true;
        }

        /* check the alarms every 1 sec */
        if (now - lastTimeAlarmChecked > 1000) {
            checkAlarms();
            lastTimeAlarmChecked = now;
        }

        /* finite state machine */
        switch (SystemState) {

        case IDLE:
            brightness = 1;
            if (buttonPressed) {
                SystemState = SCREEN_ON;
                lastTimeScreenOn = now;
            } else if (AlarmTriggered) {
                SystemState = ALARM_ON;
                lastTimeAlarmStarted = now;
            }
            break;

        case SCREEN_ON:
            brightness = 60;    
            if (now - lastTimeScreenOn > 5000) {
                SystemState = IDLE;
            } else if (AlarmTriggered) {
                SystemState = ALARM_ON;
                lastTimeAlarmStarted = now;
            }
            break;

        case ALARM_ON:
            brightness = 60;
            if (now - lastTimeAlarmStarted > 90000) {
                SystemState = IDLE;
                AlarmTriggered = false;
            } else if (buttonPressed) {
                SystemState = IDLE;
                AlarmTriggered = false;
            }
            break;
        }

        if (brightness != lastBrightness) {
            analogWrite(GFX_BL, brightness);
            lastBrightness = brightness;
        }

        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
