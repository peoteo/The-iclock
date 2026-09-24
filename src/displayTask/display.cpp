#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include "TCA9554.h"
#include "WiFi.h"
#include "display.h"
#include "settings.h"
#include "ui/ui.h"

#include "task/buttonTask.h"
#include "task/clockTask.h"
#include "task/soundTask.h"
#include "task/weatherTask.h"

#include "webserver/webServer_esp32.h"

extern SemaphoreHandle_t lvglmutex;
extern SemaphoreHandle_t i2cmutex;

Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC /* DC */, LCD_CS /* CS */, SPI_SCLK /* SCK */, SPI_MOSI /* MOSI */, SPI_MISO /* MISO */);
Arduino_GFX *gfx = new Arduino_ST7796(bus, LCD_RST /* RST */, 1 /* rotation */, true, LCD_HOR_RES, LCD_VER_RES);
TCA9554 TCA(0x20);    

/* lvgl variables*/
uint32_t screenWidth;        
uint32_t screenHeight;
uint32_t bufSize;
lv_disp_draw_buf_t draw_buf;
lv_color_t *disp_draw_buf;
lv_disp_drv_t disp_drv;

/* initial value for brightness */ 
const int brightnessInit = 1;        

/* wifi credentials */
const char* ssid     = "Zero";                     
const char* password = "maremmamagliala2018"; 


/* Lvgl, ui, wifi and tasks setup (gui_start)
 * other functions like wifi_start() and
 * display_LVGL() functions. There are two mutex,
 * one for lvgl and the other for i2c.
 */


void display_time(char* timeStr, char* dateStr, const char* wDay)
{
  lv_label_set_text(ui_time, timeStr);

  lv_label_set_text(ui_date, dateStr);

  lv_label_set_text(ui_weekdays, wDay);
}

void display_alarmIcon(int alarm_id, bool state)
{
  static lv_obj_t *ui_alarmsIcon[3] = {ui_alarm1, ui_alarm2, ui_alarm3};

  if (alarm_id < 0 || alarm_id >= 3) return;

  if (state) 
  {
    lv_obj_clear_flag(ui_alarmsIcon[alarm_id], LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(ui_alarmsIcon[alarm_id], LV_OBJ_FLAG_HIDDEN);
  }
}

void display_weather(int weatherCode, float temperature, bool isDay)
{
  char buff[10];
  snprintf(buff, sizeof(buff), "%d°C", (int)roundf(temperature));
  lv_label_set_text(ui_temperature, buff);

  static lv_obj_t *lastIcon = NULL;
  lv_obj_t *icon = NULL;

  switch (weatherCode)
  {
  case 0:
    if (isDay) {
      icon = ui_sun;
    } else {
      icon = ui_moon;
    }
    break;

  case 1: case 2: case 3:
    icon = ui_cloudy;
    break;

  case 51: case 53: case 55: case 56: case 57:
  case 61: case 63: case 65: case 66: case 67:
  case 80: case 81: case 82:
    icon = ui_rain;
    break;

  case 95: case 96: case 99:
    icon = ui_storm;
    break;

  case 71: case 73: case 75: case 77: case 85:
  case 86:
    icon = ui_snow;
    break;

  case 45: case 48:
    icon = ui_fog;
    break;

  default:
    return;
  }

  if (icon == lastIcon) return;
  if (lastIcon) lv_obj_add_flag(lastIcon, LV_OBJ_FLAG_HIDDEN);
  
  lv_obj_clear_flag(icon, LV_OBJ_FLAG_HIDDEN);
  lastIcon = icon;
}

void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) 
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);

  lv_disp_flush_ready(disp_drv);
}

void lcd_reset(void) 
{
  TCA.write1(1, 1);
  delay(10);
  TCA.write1(1, 0);
  delay(10);
  TCA.write1(1, 1);
  delay(200);
}

/*GFX print function*/
void GFX_print(const char* text, uint16_t color = RGB565_WHITE, int textSize = 2)
{  
  gfx->setTextColor(color);
  gfx->setTextSize(textSize);

  if (text[0] == '\n') {
    gfx->setCursor(10, gfx->getCursorY() + 8 * textSize);
    text++;
  }

  gfx->print(text); 

  if (text[strlen(text) - 1] == '\n') {
    gfx->setCursor(10, gfx->getCursorY());
  }
}

/*NTP configuration*/
void ntp_config(arduino_event_id_t event, arduino_event_info_t info)
{
  configTzTime("CET-1CEST,M3.5.0/2,M10.5.0/3", "pool.ntp.org", "time.nist.gov");   
  log_d("WiFi reconnected - NTP and timezone configured");
}

/*Wifi autoreconnect*/
void wifi_disc_handler(arduino_event_id_t event, arduino_event_info_t info)
{
  static bool firstConnect = true;
  uint8_t reason = info.wifi_sta_disconnected.reason;

  log_d("WiFi disconnected - reason=%d", reason);

  if(firstConnect) firstConnect = false;

  /* this is because the library already manages the case of a failed connection on the first try ever (and because this 2 reason do not need reconnection) */
  if(reason != WIFI_REASON_ASSOC_LEAVE && reason != WIFI_REASON_ASSOC_FAIL && !firstConnect) 
  {
    log_d("Reconnecting...");
    WiFi.disconnect();
    WiFi.begin();
  }
}

/*Wifi setup*/
void wifi_start()  
{
  WiFi.onEvent(ntp_config, ARDUINO_EVENT_WIFI_STA_GOT_IP); // when the wifi is ready configure the ntp
  WiFi.onEvent(wifi_disc_handler, ARDUINO_EVENT_WIFI_STA_DISCONNECTED); // autoreconnect 
  // WARNING: if the password is wrong this callback will bomb the router with requests

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);
  WiFi.setAutoReconnect(false); // because now it's managed by custom callback
  
  GFX_print("Connecting to WiFi...");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    GFX_print(".");
    attempts++;
    delay(1000);
  }

  if (WiFi.status() != WL_CONNECTED) {
    GFX_print("\nWiFi connection failed, autoreconnect running\n");
    delay(2000);
  } else {
    GFX_print("\nWiFi connected!\n");
    delay(1000);
  }
}


void start_tasks()
{
  xTaskCreatePinnedToCore(
      button_task,   /* Task function */
      "Button Task", /* String with name of task */
      4096,          /* Stack size in bytes */
      NULL,          /* Parameter passed as input of the task */
      1,             /* Priority of the task */
      NULL,          /* Task handle */
      1);            /* Clock task on core */

  xTaskCreatePinnedToCore(
      clock_task,
      "Clock Task",
      4096,
      NULL,
      1,
      NULL,
      1);

  xTaskCreatePinnedToCore(
      sound_task,   
      "Sound Task", 
      4096,         
      NULL,         
      1,            
      NULL,         
      1);     
      
  xTaskCreatePinnedToCore(
      weather_task,   
      "Weather Task", 
      8192,         
      NULL,         
      1,            
      NULL,         
      1);   

  xTaskCreatePinnedToCore(
      webserver_task,   
      "Webserver Task", 
      4096,         
      NULL,         
      1,            
      NULL,         
      1);                  
}


void gui_start()
{
  /*GPIO expander*/
  TCA.begin();
  TCA.pinMode1(1, OUTPUT);  

  lcd_reset();
  
  /*GFX setup*/
  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(RGB565_BLACK);
  gfx->setCursor(10, 10);

  pinMode(GFX_BL, OUTPUT);    
  analogWrite(GFX_BL, brightnessInit);

  GFX_print("========= ALARM CLOCK STARTUP =========\n");
  delay(1000);

  /*Wifi setup*/
  wifi_start();

  /*LVGL setup*/
  gfx->fillScreen(RGB565_BLACK);
  lv_init();

  screenWidth = gfx->width();
  screenHeight = gfx->height();

  bufSize = screenWidth * 40;

  disp_draw_buf = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, bufSize);

  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*UI setup*/
  ui_init();
  
  /*application start*/
  start_tasks();
}
