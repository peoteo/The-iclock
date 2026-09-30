#ifndef DISPLAY_H
#define DISPLAY_H

void display_time(char* timeStr, char* dateStr, const char* wDay);
void display_alarmIcon(int alarm_id, bool state);
void display_weather(int weatherCode, float temperature, bool isDay);

void gui_start();

void lcd_reset(void);

void wifi_start();

void start_tasks();

#endif