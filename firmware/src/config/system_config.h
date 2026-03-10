#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

/* LoRa */
#define LORA_FREQUENCY      915E6   /* Hz — adjust to your region */
#define LORA_BANDWIDTH      125E3
#define LORA_SF             7
#define LORA_TX_POWER       14      /* dBm */

/* GPS */
#define GPS_BAUD            9600

/* Display */
#define DISPLAY_WIDTH       128
#define DISPLAY_HEIGHT      64

/* Compass */
#define COMPASS_I2C_ADDR    0x1E    /* Default HMC5883L address */

#endif /* SYSTEM_CONFIG_H */
