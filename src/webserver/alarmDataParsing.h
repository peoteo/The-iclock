#ifndef ALARMDATAPARSING_H
#define ALARMDATAPARSING_H

#include <Arduino.h>

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t days; //0_0_sat_0_fri_0_thu 0_wed_0_tue_0_mon_0_sun 
    bool enabled;
} alarmData;

extern alarmData alarms_data[3]; 

void serializeAllData(char * jsonData, size_t size);
void deserializeAlarmJson(const char* alarmJson);

void saveToPreference(int id);
void loadFromPreference();

int extract_int(const char* json, const char* key);
bool extract_bool(const char* json, const char* key);
uint8_t extract_arrayDays(const char* json, const char* key);

#endif