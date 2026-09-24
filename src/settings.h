#ifndef SETTINGS_H
#define SETTINGS_H

#define GFX_BL 6  // Backlight pin

#define SPI_MISO 2  // SPI pin
#define SPI_MOSI 1
#define SPI_SCLK 5

#define LCD_CS -1  // LCD pin
#define LCD_DC 3
#define LCD_RST -1
#define LCD_HOR_RES 320
#define LCD_VER_RES 480

#define I2C_SDA 8   // I2C pin
#define I2C_SCL 7

#define I2S_NUM I2S_NUM_0  // I2S0 port
#define I2S_MCK_PIN 12     // MCLK pin
#define I2S_BCK_PIN 13     // BCLK pin
#define I2S_LRCK_PIN 15    // LRCLK pin
#define I2S_DOUT_PIN 16    // dataout
#define I2S_DIN_PIN 14     // datain

#define EXAMPLE_RECV_BUF_SIZE (2400)
#define EXAMPLE_SAMPLE_RATE (44100)
#define EXAMPLE_MCLK_MULTIPLE (256)  // If not using 24-bit data width, 256 should be enough
#define EXAMPLE_MCLK_FREQ_HZ (EXAMPLE_SAMPLE_RATE * EXAMPLE_MCLK_MULTIPLE)
#define VOICE_VOLUME (70)    // VOLUME CONTROL (0-100)

#ifdef CONFIG_IDF_TARGET_ESP32S3  // SD pin
#define SD_CLK 11
#define SD_CMD 10
#define SD_D0  9
#endif

#define BUTTON 21  // button pin

#endif