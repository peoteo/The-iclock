#include <Arduino.h>
#include <Wire.h>
#include <lvgl.h>
#include "settings.h"
#include "displayTask/display.h"

SemaphoreHandle_t lvglmutex = xSemaphoreCreateMutex();
SemaphoreHandle_t i2cmutex = xSemaphoreCreateMutex();

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("--------------------------");
  Serial.println("Alarm clock initialization");

  log_i("Total heap: %d", ESP.getHeapSize());      //d for debug, e for error or i for information log messages
  log_i("Free heap: %d", ESP.getFreeHeap());
  log_i("Total PSRAM: %d", ESP.getPsramSize());
  log_i("Free PSRAM: %d", ESP.getFreePsram());

  Wire.begin(I2C_SDA, I2C_SCL);

  gui_start();

}

void loop() {

  xSemaphoreTake(lvglmutex, portMAX_DELAY);
  lv_timer_handler();
  xSemaphoreGive(lvglmutex);

  vTaskDelay(10 / portTICK_PERIOD_MS);
}