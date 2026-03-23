/*
 * gps_uart_test.ino
 * Prueba mínima de comunicación UART con el módulo GNSS.
 * Heltec WiFi LoRa 32 V4 + módulo GNSS FPC
 *
 * Abrir Serial Monitor a 115200 baud.
 */

/* Pines L76K según Meshtastic heltec_v4 */
#define GPS_RX_PIN   38   // GPS TX → ESP RX  ("bits going TOWARDS the CPU")
#define GPS_TX_PIN   39   // GPS RX ← ESP TX  ("bits going TOWARDS the GPS")
#define GPS_PWR_PIN  34   // PIN_GPS_EN — active LOW (LOW=encendido, HIGH=apagado)
#define GPS_RST_PIN  42   // PIN_GPS_RESET — LOW=reset (>100ms), HIGH=normal
#define GPS_WAKE_PIN 40   // PIN_GPS_STANDBY — HIGH=force wake, LOW=allow sleep
#define GPS_BAUD     9600 // L76K default

HardwareSerial gpsSerial(1);

static uint32_t total_bytes    = 0;
static uint32_t total_lines    = 0;
static uint32_t last_report_ms = 0;
static char     line_buf[128];
static uint8_t  line_len = 0;

void setup()
{
    Serial.begin(115200);
    delay(3000);

    Serial.println("\n=== GPS UART RAW TEST ===");
    Serial.printf("RX=GPIO%d  TX=GPIO%d  PWR=GPIO%d  baud=%d\n",
                  GPS_RX_PIN, GPS_TX_PIN, GPS_PWR_PIN, GPS_BAUD);

    /* RST HIGH = normal, WAKE HIGH = force wake */
    pinMode(GPS_RST_PIN,  OUTPUT); digitalWrite(GPS_RST_PIN,  HIGH);
    pinMode(GPS_WAKE_PIN, OUTPUT); digitalWrite(GPS_WAKE_PIN, HIGH);
    /* EN active LOW: LOW = módulo encendido */
    pinMode(GPS_PWR_PIN,  OUTPUT); digitalWrite(GPS_PWR_PIN,  LOW);
    Serial.println("EN(GPIO34)=LOW(on)  RST=HIGH  WAKE=HIGH");

    delay(3000);   // esperar arranque del módulo

    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    Serial.println("UART1 abierto. Esperando datos...\n");

    last_report_ms = millis();
}

void loop()
{
    while (gpsSerial.available() > 0) {
        uint8_t b = (uint8_t)gpsSerial.read();
        total_bytes++;
        if (b >= 0x20 && b < 0x7F)
            Serial.printf("%02X('%c') ", b, b);
        else
            Serial.printf("%02X ", b);
        if (total_bytes % 16 == 0) Serial.println();
    }

    uint32_t now = millis();
    if (now - last_report_ms >= 5000) {
        last_report_ms = now;
        Serial.printf("[DIAG %lus] bytes=%lu  lineas_NMEA=%lu\n",
                      (unsigned long)(now / 1000),
                      (unsigned long)total_bytes,
                      (unsigned long)total_lines);
        if (total_bytes == 0)
            Serial.println("  !! Sin datos");
    }
}
