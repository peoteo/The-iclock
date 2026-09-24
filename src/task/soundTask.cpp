#include <Arduino.h>
#include "es8311.h"
#include <esp_check.h>
#include <SD_MMC.h>
#include <AudioFileSourceFS.h>
#include <AudioGeneratorWAV.h>
#include <AudioOutputI2S.h>

#include "soundTask.h"
#include "settings.h"
#include "buttonTask.h"

extern SemaphoreHandle_t i2cmutex;

AudioGeneratorWAV *wav = nullptr;
AudioFileSourceFS *file = nullptr;
AudioOutputI2S *out = nullptr;
es8311_handle_t es_handle = nullptr;

static const char* TAG = "sound";  // Define logging tag

/**
 * This task initializes es8311 and if the global var AlarmTriggered is
 * true plays repeatly the wav file from the sd
 */

static esp_err_t es8311_codec_init(void) {
  es_handle = es8311_create(I2C_NUM_0, ES8311_ADDRRES_0);
  ESP_RETURN_ON_FALSE(es_handle, ESP_FAIL, TAG, "es8311 create failed");
  const es8311_clock_config_t es_clk = {
    .mclk_inverted = false,
    .sclk_inverted = false,
    .mclk_from_mclk_pin = true,
    .mclk_frequency = EXAMPLE_MCLK_FREQ_HZ,
    .sample_frequency = EXAMPLE_SAMPLE_RATE
  };

  ESP_ERROR_CHECK(es8311_init(es_handle, &es_clk, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16));
  ESP_RETURN_ON_ERROR(es8311_voice_mute(es_handle, true), TAG, "es8311 muting failed");
  ESP_RETURN_ON_ERROR(es8311_voice_fade(es_handle, ES8311_FADE_16LRCK), TAG, "set es8311 softramp rate failed");
  ESP_RETURN_ON_ERROR(es8311_voice_volume_set(es_handle, VOICE_VOLUME, NULL), TAG, "set es8311 volume failed");
  ESP_RETURN_ON_ERROR(es8311_microphone_config(es_handle, false), TAG, "set es8311 microphone failed");

  //Serial.println("ES8311 init");
  return ESP_OK;
}

/**
 * @brief Clear the DAC ram and mute the es8311
 *
 * @param mute True to mute, false to unmute
 */
void mute_es8311(bool mute) {
  xSemaphoreTake(i2cmutex, portMAX_DELAY);
  es8311_voice_mute(es_handle, mute);
  xSemaphoreGive(i2cmutex); 
}

void sound_task(void *pvParameter) {

  /* ES8311 initialization */
  xSemaphoreTake(i2cmutex, portMAX_DELAY);
  es8311_codec_init();
  xSemaphoreGive(i2cmutex);  

  /* SD card configuration */
  if(!SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0)){
    Serial.println("Pin change failed!");
  }
  if (!SD_MMC.begin( "/sdcard", true)) {
    Serial.println("Card Mount Failed");
  }
  //uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  //Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

  /* I2S setup */
  out = new AudioOutputI2S();
  out->SetPinout(I2S_BCK_PIN, I2S_LRCK_PIN, I2S_DOUT_PIN, I2S_MCK_PIN);  // BCLK, LRCK, DOUT, MCLK

  for (;;) {

    if (AlarmTriggered) {
      /* Start the playback */
      if (!wav) {
        file = new AudioFileSourceFS(SD_MMC, "/alarm.wav");
        wav = new AudioGeneratorWAV();

        wav->begin(file, out); 
        mute_es8311(false);

        log_i("Free heap: %d", ESP.getFreeHeap());
      
      }

      /* Loop the wav file */
      if (wav->isRunning()) {
        if (!wav->loop()) {
          wav->stop();
          file->open("/alarm.wav");
          wav->begin(file, out);
        }
      }

      vTaskDelay(1 / portTICK_PERIOD_MS);

    } else {
      /* If the wav file is not closed close it */
      if (wav) {
        mute_es8311(true);
        Serial.println("Playback has been stopped");
  
        wav->stop();
        delete wav;
        delete file;
        wav = nullptr;
        file = nullptr;
  
        log_i("Free heap: %d", ESP.getFreeHeap());
      }

      vTaskDelay(200 / portTICK_PERIOD_MS);
    }
  }
}
