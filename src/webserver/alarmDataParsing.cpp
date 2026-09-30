#include <Arduino.h>
#include <Preferences.h>

#include "alarmDataParsing.h"
#include "displayTask/display.h"

extern SemaphoreHandle_t lvglmutex;

alarmData alarms_data[3] = {
    {7, 30, 0b01111111, false},
    {10, 0, 0b00010000, true},
    {12, 56, 0b00010011, false}
}; // alarms data

Preferences preference;


void serializeAllData(char *jsonData, size_t size) {
    int length = 0;

    length += snprintf(jsonData + length, size - length, "[");

    for (int i = 0; i < 3; i++) {

        length += snprintf(jsonData + length, size - length, 
            "{ \"hour\": %d, \"minute\": %d, \"days\": [", alarms_data[i].hour, alarms_data[i].minute);

        for (int digit = 0; digit < 7; digit++) {
            if (alarms_data[i].days & (1 << digit)) {
                length += snprintf(jsonData + length, size - length, "%d,", digit);
            }
        }

        if (jsonData[length - 1] == ',') {
            jsonData[length - 1] = '\0';
            length--;
        }

        length += snprintf(jsonData + length, size - length, "], \"enabled\": %s }", alarms_data[i].enabled ? "true" : "false");


        if (i < 2) {
            length += snprintf(jsonData + length, size - length, ",");
        } 
    }

    length += snprintf(jsonData + length, size - length, "]");
}

void deserializeAlarmJson(const char* alarmJson) {
    int alarm_id = extract_int(alarmJson, "id");
    if (alarm_id < 0 || alarm_id > 2) return;

    alarmData& current_alarm = alarms_data[alarm_id];

    current_alarm.hour = extract_int(alarmJson, "hour");
    current_alarm.minute = extract_int(alarmJson, "minute");
    current_alarm.days = extract_arrayDays(alarmJson, "days");
    current_alarm.enabled = extract_bool(alarmJson, "enabled");

    saveToPreference(alarm_id);

    xSemaphoreTake(lvglmutex, portMAX_DELAY);
    display_alarmIcon(alarm_id, current_alarm.enabled);
    xSemaphoreGive(lvglmutex);

    //Serial.printf("id: %d hour: %d minute: %d days: %d enabled: %d \n", alarm_id, current_alarm.hour, current_alarm.minute, current_alarm.days, current_alarm.enabled);
}

void saveToPreference(int id) {
    preference.begin("alarms", false);
    
    char buff[25];

    snprintf(buff, 25, "alarm%d_hour", id);
    preference.putUChar(buff, alarms_data[id].hour);
    snprintf(buff, 25, "alarm%d_minute", id);
    preference.putUChar(buff, alarms_data[id].minute);
    snprintf(buff, 25, "alarm%d_days", id);
    preference.putUChar(buff, alarms_data[id].days);
    snprintf(buff, 25, "alarm%d_enabled", id);
    preference.putBool(buff, alarms_data[id].enabled);
    
    preference.end();
}

void loadFromPreference(){
    preference.begin("alarms", true);

    bool prefInit = preference.isKey("NVSinit");
    char buff[25];

    /* the following statement is esecuted only when there's no "alarms" space */
    if (prefInit == false) {
        preference.end();
        preference.begin("alarms", false);
        
        for (int i = 0; i < 3; i++) {
            snprintf(buff, 25, "alarm%d_hour", i);
            preference.putUChar(buff, alarms_data[i].hour);
            snprintf(buff, 25, "alarm%d_minute", i);
            preference.putUChar(buff, alarms_data[i].minute);
            snprintf(buff, 25, "alarm%d_days", i);
            preference.putUChar(buff, alarms_data[i].days);
            snprintf(buff, 25, "alarm%d_enabled", i);
            preference.putBool(buff, alarms_data[i].enabled);
        }

        preference.putBool("NVSinit", true);

        preference.end();
        preference.begin("alarms", true);
    }

    /* load the information stored into the alarms_data struct */
    for (int i = 0; i < 3; i++) {
        snprintf(buff, 25, "alarm%d_hour", i);
        alarms_data[i].hour = preference.getUChar(buff, alarms_data[i].hour);
        snprintf(buff, 25, "alarm%d_minute", i);
        alarms_data[i].minute = preference.getUChar(buff, alarms_data[i].minute);
        snprintf(buff, 25, "alarm%d_days", i);
        alarms_data[i].days = preference.getUChar(buff, alarms_data[i].days);
        snprintf(buff, 25, "alarm%d_enabled", i);
        alarms_data[i].enabled = preference.getBool(buff, alarms_data[i].enabled);
    }

    preference.end();

    /* update alarms icon */
    xSemaphoreTake(lvglmutex, portMAX_DELAY);
    for (int i = 0; i < 3; i++) {
        display_alarmIcon(i, alarms_data[i].enabled);
    }
    xSemaphoreGive(lvglmutex);
}


int extract_int(const char* json, const char* key) {
    const char* pos = strstr(json, key);
    if (!pos) return -1;

    pos = strchr(pos, ':');
    if (!pos) return -1;

    pos++;
    return atoi(pos);
}

bool extract_bool(const char* json, const char* key) {
    const char* pos = strstr(json, key);
    if (!pos) return false;

    pos = strchr(pos, ':');
    if (!pos) return false;

    pos++;
    while (*pos == ' ' || *pos == '\t'|| *pos == '\n') pos++;

    if (strncmp(pos, "true", 4) == 0) return true;
    if (strncmp(pos, "false", 5) == 0) return false;
    return false;
}

uint8_t extract_arrayDays(const char* json, const char* key) {
    const char* pos = strstr(json, key);
    if (!pos) return 0;

    pos = strchr(pos,'[');
    if (!pos) return 0;

    const char* end = strchr(pos, ']');
    if (!end || end <= pos) return 0;

    int len = end - pos -1;
    if (len >= 50) len = 49;

    char buffer[50];
    snprintf(buffer, len + 1, "%s", pos + 1);

    uint8_t days = 0b00000000;
    char* element = strtok(buffer, ",");
    while (element) {
        int digit = atoi(element);

        if (digit < 0 || digit > 6) return 0;
        if (days & (1 << digit)) return 0;

        days |= (1 << digit);
        element = strtok(NULL, ",");
    }

    return days;
}