#ifndef PINS_H
#define PINS_H

/* ─────────────────────────────────────────────────────────────────────────
 * Heltec WiFi LoRa 32 V4 (ESP32-S3)
 * ───────────────────────────────────────────────────────────────────────── */

/* Power rail para OLED y sensores (LOW = encendido) */
#define VEXT_ENABLE  36

/* Bus I2C — compartido por OLED (SSD1306) y compás (QMC5883L) */
#define OLED_SDA     41
#define OLED_SCL     42
#define OLED_RST     21

#define I2C_SDA      OLED_SDA
#define I2C_SCL      OLED_SCL

/* SX1262 LoRa (integrado en la placa, SPI) */
#define LORA_SCK     9
#define LORA_MISO    11
#define LORA_MOSI    10
#define LORA_CS      8
#define LORA_RST     12
#define LORA_DIO1    14
#define LORA_BUSY    13

/* GPS (UART2) */
#define GPS_RX_PIN   33
#define GPS_TX_PIN   34

#endif /* PINS_H */
