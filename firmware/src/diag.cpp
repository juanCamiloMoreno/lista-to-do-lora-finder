#include "diag.h"
#include "config/pins.h"
#include "config/system_config.h"
#include "drivers/battery/battery.h"
#include "drivers/compass/compass.h"
#include "drivers/gps/gps.h"
#include "drivers/lora/lora.h"

#include <Arduino.h>
#include <Wire.h>

/* ── Helpers internos ───────────────────────────────────────────────────── */

static void _sep(void)
{
    Serial.println("  ----------------------------------------");
}

/* Escanea un bus I2C y lista los dispositivos encontrados */
static void _i2c_scan(TwoWire &bus, const char *bus_name)
{
    Serial.printf("  Bus %-6s: ", bus_name);
    uint8_t found = 0;
    for (uint8_t addr = 1; addr < 127; addr++) {
        bus.beginTransmission(addr);
        if (bus.endTransmission() == 0) {
            Serial.printf("0x%02X ", addr);
            found++;
        }
    }
    if (found == 0) Serial.print("(ninguno)");
    Serial.printf("  [%d dispositivo%s]\n", found, found == 1 ? "" : "s");
}

/* ── Informe completo ───────────────────────────────────────────────────── */

void diag_report(void)
{
    Serial.println();
    Serial.println("========================================");
    Serial.println("  DIAGNOSTICO DEL SISTEMA — LoRa Finder");
    Serial.println("========================================");

    /* ── 1. Sistema (ESP32-S3) ─────────────────────────────────────────── */
    Serial.println("\n[1] SISTEMA (ESP32-S3)");
    _sep();
    Serial.printf("  Chip modelo    : %s  rev%d\n",
                  ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf("  CPU cores      : %d  @  %lu MHz\n",
                  ESP.getChipCores(), ESP.getCpuFreqMHz());
    Serial.printf("  Flash          : %lu KB  (%lu KB libre)\n",
                  ESP.getFlashChipSize() / 1024,
                  ESP.getFreeSketchSpace() / 1024);

    /* Memoria heap */
    Serial.printf("  Heap total     : %lu KB\n",
                  ESP.getHeapSize() / 1024);
    Serial.printf("  Heap libre     : %lu KB  (min: %lu KB)\n",
                  ESP.getFreeHeap() / 1024,
                  ESP.getMinFreeHeap() / 1024);

    /* PSRAM */
    if (ESP.getPsramSize() > 0) {
        Serial.printf("  PSRAM          : %lu KB total  /  %lu KB libre\n",
                      ESP.getPsramSize() / 1024,
                      ESP.getFreePsram() / 1024);
    } else {
        Serial.println("  PSRAM          : no detectada");
    }

    /* MAC address */
    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    Serial.printf("  MAC (base)     : %02X:%02X:%02X:%02X:%02X:%02X\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    /* ── 2. Batería ────────────────────────────────────────────────────── */
    Serial.println("\n[2] BATERIA");
    _sep();
    Serial.printf("  ADC pin        : GPIO%d  (ADC1_CH0)\n", BAT_ADC_PIN);
    Serial.printf("  CTRL pin       : GPIO%d  OUTPUT HIGH\n", BAT_CTRL_PIN);
    Serial.printf("  Multiplicador  : 4.9  (divisor de alta impedancia)\n");

    battery_data_t bat = {};
    battery_read(&bat);
    Serial.printf("  Voltaje        : %.3f V\n", bat.voltage);
    Serial.printf("  Nivel          : %d %%\n", bat.percent);
    const char *st = (bat.status == BAT_STATUS_CHARGING)   ? "Cargando"    :
                     (bat.status == BAT_STATUS_FULL)        ? "Llena"       :
                                                              "Descargando";
    Serial.printf("  Estado         : %s\n", st);
    if (bat.voltage < 0.1f)
        Serial.println("  AVISO          : voltaje = 0, verificar bateria conectada");

    /* ── 3. Display SSD1306 ────────────────────────────────────────────── */
    Serial.println("\n[3] DISPLAY (SSD1306 OLED)");
    _sep();
    Serial.printf("  Bus            : Wire (I2C0)\n");
    Serial.printf("  Pines          : SDA=GPIO%d  SCL=GPIO%d  RST=GPIO%d\n",
                  OLED_SDA, OLED_SCL, OLED_RST);
    Serial.printf("  Resolucion     : %dx%d px\n", DISPLAY_WIDTH, DISPLAY_HEIGHT);
    Serial.printf("  VEXT_ENABLE    : GPIO%d  (LOW=ON)\n", VEXT_ENABLE);

    /* ── 4. I2C — escaneo de ambos buses ──────────────────────────────── */
    Serial.println("\n[4] I2C — DISPOSITIVOS DETECTADOS");
    _sep();
    _i2c_scan(Wire,  "Wire ");   /* OLED bus */
    _i2c_scan(Wire1, "Wire1");   /* Compás bus */

    /* ── 5. Brújula QMC5883L ───────────────────────────────────────────── */
    Serial.println("\n[5] BRUJULA (QMC5883L)");
    _sep();
    Serial.printf("  Bus            : Wire1 (I2C1)\n");
    Serial.printf("  Pines          : SDA=GPIO%d  SCL=GPIO%d\n", MAG_SDA, MAG_SCL);
    Serial.printf("  Direccion I2C  : 0x%02X\n", COMPASS_I2C_ADDR);
    Serial.printf("  Velocidad bus  : %d kHz\n", I2C_CLOCK_HZ / 1000);
    Serial.printf("  Filtro EMA     : alpha=%.2f\n", COMPASS_FILTER_ALPHA);
    Serial.printf("  Calibracion    : %s\n",
                  compass_has_calibration() ? "guardada en NVS" : "pendiente");

    compass_data_t mag = {};
    if (compass_read(&mag)) {
        Serial.printf("  Lectura actual : heading=%.1f deg  X=%d  Y=%d  Z=%d\n",
                      mag.heading, mag.x, mag.y, mag.z);
    } else {
        Serial.println("  ERROR          : compass_read() fallo");
    }

    /* ── 6. GPS ────────────────────────────────────────────────────────── */
    Serial.println("\n[6] GPS");
    _sep();
    Serial.printf("  Interfaz       : UART2\n");
    Serial.printf("  Pines          : RX=GPIO%d  TX=GPIO%d\n",
                  GPS_RX_PIN, GPS_TX_PIN);
    Serial.printf("  Baud rate      : %d\n", GPS_BAUD);
    Serial.printf("  Parser         : TinyGPSPlus\n");

    gps_update();
    gps_data_t gps = gps_get_data();
    Serial.printf("  Fix valido     : %s\n", gps.valid ? "SI" : "NO (buscando)");
    Serial.printf("  Satelites      : %d\n", gps.satellites);
    if (gps.valid) {
        Serial.printf("  Posicion       : %.6f, %.6f\n",
                      gps.latitude, gps.longitude);
        Serial.printf("  Altitud        : %.1f m\n", gps.altitude);
        Serial.printf("  Velocidad      : %.1f km/h\n", gps.speed);
    }

    /* ── 7. LoRa SX1262 ────────────────────────────────────────────────── */
    Serial.println("\n[7] LORA (SX1262)");
    _sep();
    Serial.printf("  Interfaz       : SPI\n");
    Serial.printf("  Pines          : SCK=GPIO%d  MISO=GPIO%d  MOSI=GPIO%d\n",
                  LORA_SCK, LORA_MISO, LORA_MOSI);
    Serial.printf("                   CS=GPIO%d   RST=GPIO%d\n",
                  LORA_CS, LORA_RST);
    Serial.printf("                   DIO1=GPIO%d  BUSY=GPIO%d\n",
                  LORA_DIO1, LORA_BUSY);
    Serial.printf("  Frecuencia cfg : %.0f MHz\n", LORA_FREQUENCY / 1e6);
    Serial.printf("  Bandwidth      : %.0f kHz\n", LORA_BANDWIDTH / 1e3);
    Serial.printf("  Spreading Fac. : SF%d\n", LORA_SF);
    Serial.printf("  TX Power       : %d dBm\n", LORA_TX_POWER);
    Serial.printf("  Estado         : %s\n",
                  lora_is_ready() ? "OK" : "ERROR — revisar SPI/pines");

    /* ── Cierre ────────────────────────────────────────────────────────── */
    Serial.println();
    Serial.println("========================================");
    Serial.printf("  Heap libre al final: %lu KB\n", ESP.getFreeHeap() / 1024);
    Serial.println("========================================\n");
}
