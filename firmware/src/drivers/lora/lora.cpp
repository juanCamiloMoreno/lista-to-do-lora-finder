#include "lora.h"
#include "config/pins.h"
#include "config/system_config.h"

#include <RadioLib.h>
#include <SPI.h>
#include <Arduino.h>

/* ── SPI personalizado para el SX1262 ──────────────────────────────────────
 * El SX1262 está en el bus FSPI del ESP32-S3.
 * RadioLib necesita el SPIClass explícito cuando los pines no son los default.
 * ─────────────────────────────────────────────────────────────────────────── */
static SPIClass _spi(FSPI);

/* ── Instancia RadioLib ─────────────────────────────────────────────────────
 * Module(cs, irq/DIO1, rst, busy, spi)
 * ─────────────────────────────────────────────────────────────────────────── */
static SX1262 _radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY, _spi);

static bool            _ready        = false;
static bool            _in_rx        = false;
static volatile bool   _pkt_flag     = false;   /* seteado desde ISR al llegar paquete */
static uint8_t         _current_sf   = LORA_SF;

static void IRAM_ATTR _on_rx(void) { _pkt_flag = true; }

/* ── Implementación ─────────────────────────────────────────────────────── */

bool lora_init(void)
{
    /* Iniciar SPI con pines del Heltec V4 */
    _spi.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

    /* Radio.begin(freq_MHz, bw_kHz, sf, cr, syncWord, power_dBm, preamble) */
    int state = _radio.begin(
        LORA_FREQUENCY   / 1e6f,   /* Hz → MHz */
        LORA_BANDWIDTH   / 1e3f,   /* Hz → kHz */
        LORA_SF,                   /* Spreading Factor */
        5,                         /* Coding Rate 4/5 */
        RADIOLIB_SX126X_SYNC_WORD_PRIVATE,
        LORA_TX_POWER,
        8                          /* preamble length */
    );

    if (state != RADIOLIB_ERR_NONE) {
        Serial.printf("[lora] init error: %d\n", state);
        _ready = false;
        return false;
    }

    /* TCXO 1.8 V por DIO3 — imprescindible en Heltec V4 para RX estable */
    state = _radio.setTCXO(1.8f);
    if (state != RADIOLIB_ERR_NONE) {
        Serial.printf("[lora] setTCXO error: %d\n", state);
        /* No fatal: continúa, pero RX puede ser poco fiable */
    }

    /* DIO2 como switch de antena RF (requerido en Heltec V4) */
    _radio.setDio2AsRfSwitch(true);

    _ready = true;
    Serial.printf("[lora] OK  %.3f MHz  BW=%.0f kHz  SF%d  %d dBm\n",
                  LORA_FREQUENCY / 1e6f,
                  LORA_BANDWIDTH / 1e3f,
                  LORA_SF, LORA_TX_POWER);
    return true;
}

bool lora_send(const uint8_t *data, size_t len)
{
    if (!_ready || !data || len == 0) return false;

    _in_rx = false;
    int state = _radio.transmit(data, len);

    if (state != RADIOLIB_ERR_NONE) {
        Serial.printf("[lora] TX error: %d\n", state);
        return false;
    }
    return true;
}

bool lora_start_rx(void)
{
    if (!_ready) return false;

    _radio.setPacketReceivedAction(_on_rx);
    int state = _radio.startReceive();
    if (state != RADIOLIB_ERR_NONE) {
        Serial.printf("[lora] RX start error: %d\n", state);
        return false;
    }
    _pkt_flag = false;
    _in_rx    = true;
    return true;
}

int lora_receive(uint8_t *buf, size_t max_len,
                 int16_t *rssi_out, float *snr_out)
{
    if (!_ready || !_in_rx || !buf) return -1;

    /* No bloqueante: flag seteado por ISR en DIO1 */
    if (!_pkt_flag) return 0;
    _pkt_flag = false;

    size_t pkt_len = _radio.getPacketLength();
    if (pkt_len == 0 || pkt_len > max_len) return 0;

    int state = _radio.readData(buf, pkt_len);
    if (state != RADIOLIB_ERR_NONE) {
        Serial.printf("[lora] RX read error: %d\n", state);
        return -1;
    }

    if (rssi_out) *rssi_out = _radio.getRSSI();
    if (snr_out)  *snr_out  = _radio.getSNR();

    /* Volver a modo recepción para el siguiente paquete */
    _radio.startReceive();

    return (int)pkt_len;
}

bool lora_is_ready(void) { return _ready; }

bool lora_set_sf(uint8_t sf)
{
    if (!_ready || sf < 7 || sf > 12) return false;
    int state = _radio.setSpreadingFactor(sf);
    if (state != RADIOLIB_ERR_NONE) {
        Serial.printf("[lora] set SF error: %d\n", state);
        return false;
    }
    _current_sf = sf;
    _in_rx = false;
    Serial.printf("[lora] SF cambiado a SF%d  ToA~%lums\n", sf, (unsigned long)(((1u<<sf)*8u*(13u + ((8u*17u - 4u*sf + 44u + 4u*(sf)-1u)/(4u*(sf)))*5u))/1000u));
    return lora_start_rx();
}

uint8_t lora_get_sf(void) { return _current_sf; }
