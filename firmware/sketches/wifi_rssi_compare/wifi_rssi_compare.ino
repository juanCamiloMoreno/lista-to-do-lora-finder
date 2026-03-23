/*
 * wifi_rssi_compare.ino  —  C-10: Comparación WiFi RSSI vs LoRa RSSI
 * ─────────────────────────────────────────────────────────────────────────────
 * Sketch separado para evaluar si el RSSI de WiFi ofrece mayor resolución
 * de proximidad que el RSSI de LoRa a distancias cortas (1–100 m).
 *
 * MODO BEACON (comentar MODO_SCANNER):
 *   El nodo crea un SoftAP con SSID fijo "LORAFINDER_PROBE".
 *   El otro nodo ejecuta en MODO_SCANNER y mide RSSI.
 *
 * MODO_SCANNER (activar definiendo #define MODO_SCANNER):
 *   El nodo escanea redes WiFi buscando "LORAFINDER_PROBE" cada 2 s.
 *   Imprime por Serial: distancia, RSSI_wifi y RSSI_lora (si disponible).
 *
 * Uso para el experimento:
 *   1. Flashear ambos Heltec V4 con este sketch.
 *   2. Uno en BEACON, otro en SCANNER.
 *   3. Colocar a 1 m, 5 m, 10 m, 25 m, 50 m, 100 m.
 *   4. Capturar al menos 30 muestras por distancia en el SCANNER.
 *   5. Copiar el output Serial a un CSV y analizar gradiente RSSI vs distancia.
 *
 * Hardware: Heltec WiFi LoRa 32 V4 (ESP32-S3)
 * ─────────────────────────────────────────────────────────────────────────────
 */

#include <WiFi.h>

/* ── Configuración ─────────────────────────────────────────────────── */

/* Descomenta la siguiente línea para compilar el modo scanner */
// #define MODO_SCANNER

#define PROBE_SSID   "LORAFINDER_PROBE"
#define PROBE_PASS   ""             /* Red abierta — solo para medición */
#define SCAN_INTERVAL_MS  2000UL   /* intervalo entre escaneos          */
#define SAMPLES_PER_DIST  30       /* muestras objetivo por distancia   */
#define SERIAL_BAUD  115200

/* ─────────────────────────────────────────────────────────────────── */

#ifndef MODO_SCANNER

/* ════════════════════════════════════════════════════════════════════
 * MODO BEACON — crea SoftAP con SSID conocido
 * ════════════════════════════════════════════════════════════════════ */

void setup()
{
    Serial.begin(SERIAL_BAUD);
    delay(1000);
    Serial.println("\n[BEACON] Iniciando SoftAP...");

    WiFi.mode(WIFI_AP);
    /* TX power máximo para mayor rango en el SoftAP */
    WiFi.setTxPower(WIFI_POWER_19_5dBm);

    if (WiFi.softAP(PROBE_SSID, PROBE_PASS, 1 /* canal */, 0 /* no oculto */)) {
        Serial.printf("[BEACON] SoftAP '%s' activo  IP: %s\n",
                      PROBE_SSID, WiFi.softAPIP().toString().c_str());
    } else {
        Serial.println("[BEACON] ERROR al crear SoftAP");
    }
}

void loop()
{
    static uint32_t t = 0;
    if (millis() - t >= 5000) {
        t = millis();
        Serial.printf("[BEACON] Clientes conectados: %d\n",
                      WiFi.softAPgetStationNum());
    }
}

#else   /* MODO_SCANNER */

/* ════════════════════════════════════════════════════════════════════
 * MODO SCANNER — escanea WiFi y reporta RSSI por Serial
 * ════════════════════════════════════════════════════════════════════ */

static uint32_t _sample_count    = 0;
static uint32_t _last_scan_ms    = 0;
static int      _last_wifi_rssi  = 0;
static bool     _probe_found     = false;

void setup()
{
    Serial.begin(SERIAL_BAUD);
    delay(1000);

    Serial.println("\n[SCANNER] WiFi RSSI vs LoRa RSSI — Experimento comparativo C-10");
    Serial.println("[SCANNER] Formato CSV: sample,dist_m,wifi_rssi,nota");
    Serial.println("[SCANNER] Ingresa la distancia actual por Serial (ej: '10') antes de medir");
    Serial.println();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
}

static int _current_dist_m = 0;

void loop()
{
    /* Leer distancia ingresada por usuario en Serial Monitor */
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        int d = input.toInt();
        if (d > 0) {
            _current_dist_m = d;
            _sample_count   = 0;
            Serial.printf("[SCANNER] === Distancia configurada: %d m  (30 muestras requeridas) ===\n", d);
        }
    }

    if (_current_dist_m == 0) {
        delay(500);
        return;
    }

    uint32_t now = millis();
    if (now - _last_scan_ms < SCAN_INTERVAL_MS) return;
    _last_scan_ms = now;

    /* Escaneo WiFi (bloqueante ~300 ms) */
    int n = WiFi.scanNetworks(false /* blocking */, true /* show hidden */);

    _probe_found    = false;
    _last_wifi_rssi = -120;   /* valor por defecto si no se encuentra */

    for (int i = 0; i < n; i++) {
        if (WiFi.SSID(i) == PROBE_SSID) {
            _last_wifi_rssi = WiFi.RSSI(i);
            _probe_found    = true;
            break;
        }
    }

    WiFi.scanDelete();

    if (_probe_found) {
        _sample_count++;
        /* CSV: num_muestra, distancia_m, rssi_wifi */
        Serial.printf("%lu,%d,%d\n",
                      (unsigned long)_sample_count,
                      _current_dist_m,
                      _last_wifi_rssi);

        if (_sample_count >= SAMPLES_PER_DIST) {
            Serial.printf("[SCANNER] === %d muestras completadas para %d m ===\n",
                          SAMPLES_PER_DIST, _current_dist_m);
            Serial.println("[SCANNER] Ingresa nueva distancia para continuar.");
            _current_dist_m = 0;   /* esperar nueva distancia */
        }
    } else {
        Serial.printf("[SCANNER] Beacon '%s' no encontrado (scan %d redes)\n",
                      PROBE_SSID, n);
    }
}

#endif  /* MODO_SCANNER */
