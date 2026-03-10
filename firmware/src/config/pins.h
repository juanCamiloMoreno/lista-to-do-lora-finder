#ifndef PINS_H
#define PINS_H

/* Heltec WiFi LoRa 32 V4 pin definitions */

/* LoRa (SX1262 built-in) */
#define LORA_SCK    9
#define LORA_MISO   11
#define LORA_MOSI   10
#define LORA_CS     8
#define LORA_RST    12
#define LORA_DIO1   14
#define LORA_BUSY   13

/* OLED display (built-in, I2C) */
#define OLED_SDA    17
#define OLED_SCL    18
#define OLED_RST    21

/* GPS (UART) */
#define GPS_RX      33
#define GPS_TX      34

/* Compass (I2C — shared bus with OLED) */
#define COMPASS_SDA OLED_SDA
#define COMPASS_SCL OLED_SCL

#endif /* PINS_H */
