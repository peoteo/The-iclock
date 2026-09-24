#include <Arduino.h>
#include <WebServer.h>

#include "webServer_esp32.h"
#include "alarmDataParsing.h"
#include "webpage.h"
#include "image1.h"
#include "image2.h"

WebServer server(80);

void sendWebpage() {
    server.send(200, "text/html", PAGE_MAIN);
}

void sendAlarmsData() {
    char json[300];
    serializeAllData(json, sizeof(json));
    server.send(200, "application/json", json);
}

void sendImage1() {
    server.send_P(200, "image/png", (PGM_P)image1, image1_len);
}

void sendImage2() {
    server.send_P(200, "image/png", (PGM_P)image2, image2_len);
}

void handleSaveAlarm () {
    String jsonString = server.arg("plain");     
    const char* jsonData = jsonString.c_str();

    deserializeAlarmJson(jsonData);

    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void webserver_task(void *pvParameter) {

    loadFromPreference();
    
    server.on("/", sendWebpage);

    server.on("/loadData", sendAlarmsData);

    server.on("/imageOn.png", sendImage1);

    server.on("/imageOff.png", sendImage2);

    server.on("/saveAlarm", handleSaveAlarm);

    server.begin();

    for(;;) {
        
        server.handleClient();

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
