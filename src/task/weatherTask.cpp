#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include "weatherTask.h"
#include "displayTask/display.h"

/**
 * This task handles the weather icon and the temperature, updated every 15
 * minutes, it uses ArduinoJson to extract the weather data
 * 
 * RISK OF MEMORY LEAK AND MEMORY FRAGMENTATION NEED TO BE CHECKED
 */

extern SemaphoreHandle_t lvglmutex;

void weather_task(void *pvParameter) {

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;

    for (;;) {
        if (WiFi.status() == WL_CONNECTED) {
            http.useHTTP10(true);
            http.begin(client, "https://api.open-meteo.com/v1/forecast?latitude=42.86&longitude=12.36&current=temperature_2m,is_day,weather_code&timezone=auto");
            int code = http.GET();

            /* parsing */
            if (code == HTTP_CODE_OK) {
                DynamicJsonDocument doc(1024);
                DeserializationError error = deserializeJson(doc, http.getStream());
                
                if (!error) {
                    float temperature = doc["current"]["temperature_2m"];
                    int weatherCode = doc["current"]["weather_code"];
                    bool isDay = doc["current"]["is_day"];
                    
                    xSemaphoreTake(lvglmutex, portMAX_DELAY);
                    display_weather(weatherCode, temperature, isDay);
                    xSemaphoreGive(lvglmutex);
                } else {
                    Serial.print("JSON parse error: ");
                    Serial.println(error.c_str());
                }
            } else {
                Serial.print("HTTP error: ");
                Serial.println(code);
            }
            http.end();
        }
        vTaskDelay(900000 / portTICK_PERIOD_MS);
    }
}
