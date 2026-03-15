#ifndef PINS_H
#define PINS_H

/* ─────────────────────────────────────────────────────────────────────────
 * Heltec WiFi LoRa 32 V4 (ESP32-S3)
 * ───────────────────────────────────────────────────────────────────────── */

/* Power rail para OLED y sensores (LOW = encendido) */
#define VEXT_ENABLE  36

/* Bus I2C 0 (Wire)  — OLED SSD1306 */
#define OLED_SDA     17
#define OLED_SCL     18
#define OLED_RST     21

/* Bus I2C 1 (Wire1) — Compás QMC5883L */
#define MAG_SDA      5
#define MAG_SCL      6

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

/* Batería (Heltec WiFi LoRa 32 V4) */
#define BAT_ADC_PIN   1   /* GPIO1 ADC1_CH0 — entrada del divisor */
#define BAT_CTRL_PIN  37  /* GPIO37 OUTPUT HIGH habilita el divisor */

/* LED RGB WS2812B (pin de datos) */
#define RGB_LED_PIN   38

/* Botones de navegación (pull-up interno, presionado = LOW) */
#define BTN_UP_PIN      2
#define BTN_DOWN_PIN    3
#define BTN_SELECT_PIN  4

/* Alertas hápticas y sonoras */
#define BUZZER_PIN      7   /* Buzzer activo — directo al GPIO */
#define MOTOR_VIB_PIN  15   /* Motor vibrador — vía transistor NPN */

#endif /* PINS_H */
