#ifndef WEBSERVER_ESP32_H
#define WEBSERVER_ESP32_H

void sendWebpage();
void sendAlarmsData();
void sendImage1();
void sendImage2();

void webserver_task(void *pvParameter); 

#endif