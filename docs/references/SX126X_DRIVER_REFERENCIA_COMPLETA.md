# SX126X_DRIVER v2.3.2 — REFERENCIA COMPLETA
# Registros, Funciones, Enumeraciones y Código Fuente
# Para tesis: WiFi LoRa 32 V4.2 (ESP32-S3 + SX1262)

---

## PARTE 1: MAPA DE REGISTROS INTERNOS DEL CHIP

### 1.1 Registros de Paquetes y Sync Word

| Registro | Dirección | Tamaño | Función |
|---|---|---|---|
| SYNCWORDBASEADDRESS | 0x06C0 | 8 bytes | Sync word GFSK (hasta 64 bits) |
| LR_SYNCWORD | 0x0740 | 2 bytes | Sync word LoRa: 0x1424=privado, 0x3444=publico |
| CRCSEEDBASEADDRESS | 0x06BC | 2 bytes | Semilla CRC para GFSK |
| CRCPOLYBASEADDRESS | 0x06BE | 2 bytes | Polinomio CRC para GFSK |
| WHITSEEDBASEADDRESS | 0x06B8 | 2 bytes | Semilla de whitening GFSK |
| GFSK_NODE_ADDRESS | 0x06CD | 1 byte | Direccion de nodo GFSK |
| GFSK_BROADCAST_ADDRESS | 0x06CE | 1 byte | Direccion broadcast GFSK |
| RXTX_PAYLOAD_LEN | 0x06BB | 1 byte | Longitud del payload RX/TX |

### 1.2 Registros de Header LoRa

| Registro | Dirección | Bits | Función |
|---|---|---|---|
| LR_HEADER_CR | 0x0749 | [6:4] | Coding Rate del header recibido |
| LR_HEADER_CRC | 0x076B | [4] | Flag CRC del header (1=activo) |
| LR_SYNCH_TIMEOUT | 0x0706 | [7:0] | Timeout sincronizacion |

### 1.3 Registros de Ganancia y RF

| Registro | Dirección | Valores | Función |
|---|---|---|---|
| RXGAIN | 0x08AC | 0x94/0x96 | 0x94=ahorro energia, 0x96=boosted (+2dB) |
| OCP | 0x08E7 | 0x18/0x38 | Sobrecorriente PA: 60mA o 140mA |
| ANA_LNA | 0x08E2 | — | Control del LNA |
| ANA_MIXER | 0x08E5 | — | Control del mixer RX |
| XTATRIM | 0x0911 | default 0x12 | Trimming del cristal |

### 1.4 Registros de Workarounds (Errata)

| Registro | Dirección | Bits | Errata |
|---|---|---|---|
| IQ_POLARITY | 0x0736 | [0] | DS 15.4: Corrige IQ invertida LoRa |
| TX_MODULATION | 0x0889 | [2] | DS 15.1: Corrige modulacion BW=500kHz |
| TX_CLAMP_CFG | 0x08D8 | [4:1] | DS 15.2: Resistencia a desajuste antena |

### 1.5 Registros de Sistema

| Registro | Dirección | Función |
|---|---|---|
| RTC_CTRL | 0x0902 | Control del reloj RTC interno |
| EVT_CLR | 0x0944 | Limpieza de eventos (bit[1]=timeout) |
| RX_ADDRESS_POINTER | 0x0803 | Puntero buffer RX |
| RNGBASEADDRESS | 0x0819 | Generador numeros aleatorios (32 bits) |
| RETENTION_LIST_BASE_ADDRESS | 0x029F | Lista retencion registros en sleep |

### 1.6 Registros GPIO DIO3

| Registro | Dirección | Bit | Función |
|---|---|---|---|
| OUT_DIS_REG | 0x0580 | [3] | Deshabilitar salida DIO3 |
| IN_EN_REG | 0x0583 | [3] | Habilitar entrada DIO3 |

### 1.7 Registros Bitbang (BPSK/Sigfox)

| Registro | Dirección | Bits | Función |
|---|---|---|---|
| BITBANG_A_REG | 0x0680 | [6:4], enable=0x10 | Bitbang TX registro A |
| BITBANG_B_REG | 0x0587 | [3:0], enable=0x0C | Bitbang TX registro B |

---

## PARTE 2: OPCODES SPI (Comandos internos)

| Opcode | Valor | Comando |
|---|---|---|
| SET_SLEEP | 0x84 | Entrar en sleep |
| SET_STANDBY | 0x80 | Entrar en standby |
| SET_FS | 0xC1 | Frequency Synthesis |
| SET_TX | 0x83 | Transmitir |
| SET_RX | 0x82 | Recibir |
| SET_STOP_TIMER_ON_PREAMBLE | 0x9F | Parar timer al detectar preambulo |
| SET_RX_DUTY_CYCLE | 0x94 | Duty cycle RX |
| SET_CAD | 0xC5 | Channel Activity Detection |
| SET_TX_CW | 0xD1 | Onda continua |
| SET_TX_INFINITE_PREAMBLE | 0xD2 | Preambulo infinito |
| SET_REG_MODE | 0x96 | Seleccionar regulador |
| CALIBRATE | 0x89 | Calibracion |
| CALIBRATE_IMAGE | 0x98 | Calibracion de imagen |
| SET_PA_CFG | 0x95 | Configurar PA |
| SET_RX_TX_FALLBACK_MODE | 0x93 | Modo fallback |
| WRITE_REGISTER | 0x0D | Escribir registro |
| READ_REGISTER | 0x1D | Leer registro |
| WRITE_BUFFER | 0x0E | Escribir buffer |
| READ_BUFFER | 0x1E | Leer buffer |
| SET_DIO_IRQ_PARAMS | 0x08 | Configurar IRQ |
| GET_IRQ_STATUS | 0x12 | Leer estado IRQ |
| CLR_IRQ_STATUS | 0x02 | Limpiar IRQ |
| SET_DIO2_AS_RF_SWITCH | 0x9D | DIO2 como RF switch |
| SET_DIO3_AS_TCXO_CTRL | 0x97 | DIO3 como control TCXO |
| SET_RF_FREQUENCY | 0x86 | Establecer frecuencia |
| SET_PKT_TYPE | 0x8A | Tipo de paquete |
| GET_PKT_TYPE | 0x11 | Leer tipo de paquete |
| SET_TX_PARAMS | 0x8E | Potencia y rampa |
| SET_MODULATION_PARAMS | 0x8B | Parametros de modulacion |
| SET_PKT_PARAMS | 0x8C | Parametros de paquete |
| SET_CAD_PARAMS | 0x88 | Parametros CAD |
| SET_BUFFER_BASE_ADDRESS | 0x8F | Base del buffer TX/RX |
| SET_LORA_SYMB_NUM_TIMEOUT | 0xA0 | Timeout por simbolos LoRa |
| GET_STATUS | 0xC0 | Leer estado del chip |
| GET_RX_BUFFER_STATUS | 0x13 | Estado del buffer RX |
| GET_PKT_STATUS | 0x14 | Estado del paquete |
| GET_RSSI_INST | 0x15 | RSSI instantaneo |
| GET_STATS | 0x10 | Estadisticas |
| RESET_STATS | 0x00 | Resetear estadisticas |
| GET_DEVICE_ERRORS | 0x17 | Leer errores |
| CLR_DEVICE_ERRORS | 0x07 | Limpiar errores |

---

## PARTE 3: ENUMERACIONES COMPLETAS (sx126x.h)

### 3.1 Status de retorno

```c
typedef enum sx126x_status_e
{
    SX126X_STATUS_OK                  = 0,
    SX126X_STATUS_UNSUPPORTED_FEATURE = 1,
    SX126X_STATUS_UNKNOWN_VALUE       = 2,
    SX126X_STATUS_ERROR               = 3,
} sx126x_status_t;
```

### 3.2 Modos de Sleep

```c
typedef enum sx126x_sleep_cfgs_e
{
    SX126X_SLEEP_CFG_COLD_START = 0x00,  // RAM se pierde, reinicio completo
    SX126X_SLEEP_CFG_WARM_START = 0x04,  // RAM se retiene, arranque rapido
} sx126x_sleep_cfgs_t;
```

### 3.3 Modos de Standby

```c
typedef enum sx126x_standby_cfgs_e
{
    SX126X_STANDBY_CFG_RC   = 0x00,  // Oscilador RC 13MHz (~1.6uA)
    SX126X_STANDBY_CFG_XOSC = 0x01,  // Cristal externo (~5uA, mas preciso)
} sx126x_standby_cfgs_t;
```

### 3.4 Modo del regulador

```c
typedef enum sx126x_reg_mods_e
{
    SX126X_REG_MODE_LDO  = 0x00,  // Solo LDO
    SX126X_REG_MODE_DCDC = 0x01,  // DC-DC + LDO (mas eficiente)
} sx126x_reg_mods_t;
```

### 3.5 Modo fallback despues de TX/RX

```c
typedef enum sx126x_fallback_modes_e
{
    SX126X_FALLBACK_STDBY_RC   = 0x20,
    SX126X_FALLBACK_STDBY_XOSC = 0x30,
    SX126X_FALLBACK_FS         = 0x40,
} sx126x_fallback_modes_t;
```

### 3.6 Estados del chip

```c
typedef enum sx126x_chip_modes_e
{
    SX126X_CHIP_MODE_UNUSED    = 0,
    SX126X_CHIP_MODE_RFU       = 1,
    SX126X_CHIP_MODE_STBY_RC   = 2,
    SX126X_CHIP_MODE_STBY_XOSC = 3,
    SX126X_CHIP_MODE_FS        = 4,
    SX126X_CHIP_MODE_RX        = 5,
    SX126X_CHIP_MODE_TX        = 6,
} sx126x_chip_modes_t;
```

### 3.7 Mascaras de Interrupcion (IRQ)

```c
typedef enum sx126x_irq_masks_e
{
    SX126X_IRQ_NONE              = 0x0000,
    SX126X_IRQ_TX_DONE           = 0x0001,  // TX completada
    SX126X_IRQ_RX_DONE           = 0x0002,  // Paquete recibido
    SX126X_IRQ_PREAMBLE_DETECTED = 0x0004,  // Preambulo detectado
    SX126X_IRQ_SYNC_WORD_VALID   = 0x0008,  // Sync word valida (GFSK)
    SX126X_IRQ_HEADER_VALID      = 0x0010,  // Header LoRa valido
    SX126X_IRQ_HEADER_ERROR      = 0x0020,  // Error en header LoRa
    SX126X_IRQ_CRC_ERROR         = 0x0040,  // Error CRC
    SX126X_IRQ_CAD_DONE          = 0x0080,  // CAD completado
    SX126X_IRQ_CAD_DETECTED      = 0x0100,  // Actividad LoRa detectada
    SX126X_IRQ_TIMEOUT           = 0x0200,  // Timeout RX/TX
    SX126X_IRQ_LR_FHSS_HOP      = 0x4000,  // Salto LR-FHSS
    SX126X_IRQ_ALL               = 0xFFFF,
} sx126x_irq_mask_t;
```

### 3.8 Calibracion

```c
typedef enum sx126x_cal_mask_e
{
    SX126X_CAL_RC64K     = 0x01,
    SX126X_CAL_RC13M     = 0x02,
    SX126X_CAL_PLL       = 0x04,
    SX126X_CAL_ADC_PULSE = 0x08,
    SX126X_CAL_ADC_BULK_N= 0x10,
    SX126X_CAL_ADC_BULK_P= 0x20,
    SX126X_CAL_IMAGE     = 0x40,
    SX126X_CAL_ALL       = 0x7F,
} sx126x_cal_mask_t;
```

### 3.9 Voltajes TCXO

```c
typedef enum sx126x_tcxo_ctrl_voltages_e
{
    SX126X_TCXO_CTRL_1_6V = 0x00,
    SX126X_TCXO_CTRL_1_7V = 0x01,
    SX126X_TCXO_CTRL_1_8V = 0x02,
    SX126X_TCXO_CTRL_2_2V = 0x03,
    SX126X_TCXO_CTRL_2_4V = 0x04,
    SX126X_TCXO_CTRL_2_7V = 0x05,
    SX126X_TCXO_CTRL_3_0V = 0x06,
    SX126X_TCXO_CTRL_3_3V = 0x07,
} sx126x_tcxo_ctrl_voltages_t;
```

### 3.10 Tipos de Paquete

```c
typedef enum sx126x_pkt_types_e
{
    SX126X_PKT_TYPE_GFSK    = 0x00,
    SX126X_PKT_TYPE_LORA    = 0x01,
    SX126X_PKT_TYPE_BPSK    = 0x03,
    SX126X_PKT_TYPE_LR_FHSS = 0x04,
} sx126x_pkt_type_t;
```

### 3.11 Tiempos de Rampa PA

```c
typedef enum sx126x_ramp_time_e
{
    SX126X_RAMP_10_US   = 0x00,
    SX126X_RAMP_20_US   = 0x01,
    SX126X_RAMP_40_US   = 0x02,
    SX126X_RAMP_80_US   = 0x03,
    SX126X_RAMP_200_US  = 0x04,
    SX126X_RAMP_800_US  = 0x05,
    SX126X_RAMP_1700_US = 0x06,
    SX126X_RAMP_3400_US = 0x07,
} sx126x_ramp_time_t;
```

### 3.12 Spreading Factor LoRa

```c
typedef enum sx126x_lora_sf_e
{
    SX126X_LORA_SF5  = 0x05,   // Mas rapido, menor alcance
    SX126X_LORA_SF6  = 0x06,
    SX126X_LORA_SF7  = 0x07,   // Balance tipico
    SX126X_LORA_SF8  = 0x08,
    SX126X_LORA_SF9  = 0x09,
    SX126X_LORA_SF10 = 0x0A,
    SX126X_LORA_SF11 = 0x0B,
    SX126X_LORA_SF12 = 0x0C,   // Mas lento, maximo alcance
} sx126x_lora_sf_t;
```

### 3.13 Bandwidth LoRa

```c
typedef enum sx126x_lora_bw_e
{
    SX126X_LORA_BW_500 = 6,    // 500.0 kHz  -> maxima velocidad
    SX126X_LORA_BW_250 = 5,    // 250.0 kHz
    SX126X_LORA_BW_125 = 4,    // 125.0 kHz  <- Default
    SX126X_LORA_BW_062 = 3,    //  62.5 kHz
    SX126X_LORA_BW_041 = 10,   //  41.67 kHz
    SX126X_LORA_BW_031 = 2,    //  31.25 kHz
    SX126X_LORA_BW_020 = 9,    //  20.83 kHz
    SX126X_LORA_BW_015 = 1,    //  15.63 kHz
    SX126X_LORA_BW_010 = 8,    //  10.42 kHz
    SX126X_LORA_BW_007 = 0,    //   7.81 kHz -> maximo alcance
} sx126x_lora_bw_t;
```

### 3.14 Coding Rate LoRa

```c
typedef enum sx126x_lora_cr_e
{
    SX126X_LORA_CR_4_5 = 0x01, // 25% overhead  <- Default
    SX126X_LORA_CR_4_6 = 0x02, // 50% overhead
    SX126X_LORA_CR_4_7 = 0x03, // 75% overhead
    SX126X_LORA_CR_4_8 = 0x04, // 100% overhead (maxima proteccion)
} sx126x_lora_cr_t;
```

### 3.15 Parametros GFSK - Pulse Shape

```c
typedef enum sx126x_gfsk_pulse_shape_e
{
    SX126X_GFSK_PULSE_SHAPE_OFF   = 0x00,
    SX126X_GFSK_PULSE_SHAPE_BT_03 = 0x08,
    SX126X_GFSK_PULSE_SHAPE_BT_05 = 0x09,
    SX126X_GFSK_PULSE_SHAPE_BT_07 = 0x0A,
    SX126X_GFSK_PULSE_SHAPE_BT_1  = 0x0B,
} sx126x_gfsk_pulse_shape_t;
```

### 3.16 Bandwidth GFSK

```c
typedef enum sx126x_gfsk_bw_e
{
    SX126X_GFSK_BW_4800   = 0x1F,  //   4.8 kHz
    SX126X_GFSK_BW_5800   = 0x17,  //   5.8 kHz
    SX126X_GFSK_BW_7300   = 0x0F,  //   7.3 kHz
    SX126X_GFSK_BW_9700   = 0x1E,  //   9.7 kHz
    SX126X_GFSK_BW_11700  = 0x16,  //  11.7 kHz
    SX126X_GFSK_BW_14600  = 0x0E,  //  14.6 kHz
    SX126X_GFSK_BW_19500  = 0x1D,  //  19.5 kHz
    SX126X_GFSK_BW_23400  = 0x15,  //  23.4 kHz
    SX126X_GFSK_BW_29300  = 0x0D,  //  29.3 kHz
    SX126X_GFSK_BW_39000  = 0x1C,  //  39.0 kHz
    SX126X_GFSK_BW_46900  = 0x14,  //  46.9 kHz
    SX126X_GFSK_BW_58600  = 0x0C,  //  58.6 kHz
    SX126X_GFSK_BW_78200  = 0x1B,  //  78.2 kHz
    SX126X_GFSK_BW_93800  = 0x13,  //  93.8 kHz
    SX126X_GFSK_BW_117300 = 0x0B,  // 117.3 kHz <- Default
    SX126X_GFSK_BW_156200 = 0x1A,  // 156.2 kHz
    SX126X_GFSK_BW_187200 = 0x12,  // 187.2 kHz
    SX126X_GFSK_BW_234300 = 0x0A,  // 234.3 kHz
    SX126X_GFSK_BW_312000 = 0x19,  // 312.0 kHz
    SX126X_GFSK_BW_373600 = 0x11,  // 373.6 kHz
    SX126X_GFSK_BW_467000 = 0x09,  // 467.0 kHz
} sx126x_gfsk_bw_t;
```

### 3.17 Preamble Detector GFSK

```c
typedef enum sx126x_gfsk_preamble_detector_e
{
    SX126X_GFSK_PREAMBLE_DETECTOR_OFF       = 0x00,
    SX126X_GFSK_PREAMBLE_DETECTOR_MIN_8BITS = 0x04,
    SX126X_GFSK_PREAMBLE_DETECTOR_MIN_16BITS= 0x05,
    SX126X_GFSK_PREAMBLE_DETECTOR_MIN_24BITS= 0x06,
    SX126X_GFSK_PREAMBLE_DETECTOR_MIN_32BITS= 0x07,
} sx126x_gfsk_preamble_detector_t;
```

### 3.18 Filtrado de Direcciones GFSK

```c
typedef enum sx126x_gfsk_address_filtering_e
{
    SX126X_GFSK_ADDRESS_FILTERING_DISABLE                    = 0x00,
    SX126X_GFSK_ADDRESS_FILTERING_NODE_ADDRESS               = 0x01,
    SX126X_GFSK_ADDRESS_FILTERING_NODE_AND_BROADCAST_ADDRESSES = 0x02,
} sx126x_gfsk_address_filtering_t;
```

### 3.19 CRC GFSK

```c
typedef enum sx126x_gfsk_crc_types_e
{
    SX126X_GFSK_CRC_OFF         = 0x01,
    SX126X_GFSK_CRC_1_BYTE      = 0x00,
    SX126X_GFSK_CRC_2_BYTES     = 0x02,
    SX126X_GFSK_CRC_1_BYTE_INV  = 0x04,
    SX126X_GFSK_CRC_2_BYTES_INV = 0x06,
} sx126x_gfsk_crc_types_t;
```

### 3.20 DC Free (Whitening) GFSK

```c
typedef enum sx126x_gfsk_dc_free_e
{
    SX126X_GFSK_DC_FREE_OFF       = 0x00,
    SX126X_GFSK_DC_FREE_WHITENING = 0x01,
} sx126x_gfsk_dc_free_t;
```

### 3.21 Modo Header LoRa

```c
typedef enum sx126x_lora_pkt_len_modes_e
{
    SX126X_LORA_PKT_EXPLICIT = 0x00,  // Header incluido en paquete
    SX126X_LORA_PKT_IMPLICIT = 0x01,  // Sin header (longitud fija)
} sx126x_lora_pkt_len_modes_t;
```

### 3.22 CAD - Simbolos y Modos

```c
typedef enum sx126x_cad_symbs_e
{
    SX126X_CAD_01_SYMB = 0x00,  // 1 simbolo (rapido, menos fiable)
    SX126X_CAD_02_SYMB = 0x01,  // 2 simbolos <- Recomendado
    SX126X_CAD_04_SYMB = 0x02,
    SX126X_CAD_08_SYMB = 0x03,
    SX126X_CAD_16_SYMB = 0x04,  // 16 simbolos (lento, mas fiable)
} sx126x_cad_symbs_t;

typedef enum sx126x_cad_exit_modes_e
{
    SX126X_CAD_ONLY = 0x00,  // Solo detecta -> vuelve a STDBY_RC
    SX126X_CAD_RX   = 0x01,  // Si detecta -> entra en RX
    SX126X_CAD_LBT  = 0x02,  // Si NO detecta -> transmite (Listen-Before-Talk)
} sx126x_cad_exit_modes_t;
```

### 3.23 Errores del Dispositivo

```c
typedef enum sx126x_errors_e
{
    SX126X_ERRORS_RC64K_CALIBRATION = 0x0001,
    SX126X_ERRORS_RC13M_CALIBRATION = 0x0002,
    SX126X_ERRORS_PLL_CALIBRATION   = 0x0004,
    SX126X_ERRORS_ADC_CALIBRATION   = 0x0008,
    SX126X_ERRORS_IMG_CALIBRATION   = 0x0010,
    SX126X_ERRORS_XOSC_START        = 0x0020,
    SX126X_ERRORS_PLL_LOCK          = 0x0040,
    SX126X_ERRORS_PA_RAMP           = 0x0100,
} sx126x_errors_t;
```

### 3.24 Comando Status

```c
typedef enum sx126x_cmd_status_e
{
    SX126X_CMD_STATUS_RESERVED       = 0,
    SX126X_CMD_STATUS_RFU            = 1,
    SX126X_CMD_STATUS_DATA_AVAILABLE = 2,
    SX126X_CMD_STATUS_CMD_TIMEOUT    = 3,
    SX126X_CMD_STATUS_CMD_PROCESS_ERROR = 4,
    SX126X_CMD_STATUS_CMD_EXEC_FAILURE  = 5,
    SX126X_CMD_STATUS_CMD_TX_DONE    = 6,
} sx126x_cmd_status_t;
```

---

## PARTE 4: ESTRUCTURAS DE DATOS

### 4.1 Configuracion PA

```c
typedef struct sx126x_pa_cfg_params_s
{
    uint8_t pa_duty_cycle;  // 0x00-0x07
    uint8_t hp_max;         // 0x00-0x07
    uint8_t device_sel;     // 0x00=SX1262, 0x01=SX1261
    uint8_t pa_lut;         // Siempre 0x01
} sx126x_pa_cfg_params_t;
```

### 4.2 Modulacion LoRa

```c
typedef struct sx126x_mod_params_lora_s
{
    sx126x_lora_sf_t sf;    // Spreading Factor
    sx126x_lora_bw_t bw;    // Bandwidth
    sx126x_lora_cr_t cr;    // Coding Rate
    uint8_t          ldro;  // Low Data Rate Optimization (0=off, 1=on)
} sx126x_mod_params_lora_t;
```

### 4.3 Paquete LoRa

```c
typedef struct sx126x_pkt_params_lora_s
{
    uint16_t                    preamble_len_in_symb;  // 1-65535
    sx126x_lora_pkt_len_modes_t header_type;           // Explicit/Implicit
    uint8_t                     pld_len_in_bytes;      // 0-255
    bool                        crc_is_on;
    bool                        invert_iq_is_on;
} sx126x_pkt_params_lora_t;
```

### 4.4 Modulacion GFSK

```c
typedef struct sx126x_mod_params_gfsk_s
{
    uint32_t                  br_in_bps;       // Bitrate en bps
    uint32_t                  fdev_in_hz;      // Desviacion de frecuencia
    sx126x_gfsk_pulse_shape_t pulse_shape;     // Filtro gaussiano
    sx126x_gfsk_bw_t          bw_dsb_param;    // Ancho de banda RX
} sx126x_mod_params_gfsk_t;
```

### 4.5 Paquete GFSK

```c
typedef struct sx126x_pkt_params_gfsk_s
{
    uint16_t                        preamble_len_in_bits;
    sx126x_gfsk_preamble_detector_t preamble_detector;
    uint8_t                         sync_word_len_in_bits;  // 0-64
    sx126x_gfsk_address_filtering_t address_filtering;
    sx126x_gfsk_pkt_len_modes_t     header_type;
    uint8_t                         pld_len_in_bytes;
    sx126x_gfsk_crc_types_t         crc_type;
    sx126x_gfsk_dc_free_t           dc_free;
} sx126x_pkt_params_gfsk_t;
```

### 4.6 Modulacion y Paquete BPSK (Sigfox)

```c
typedef struct sx126x_mod_params_bpsk_s
{
    uint32_t                br_in_bps;
    sx126x_bpsk_pulse_shape_t pulse_shape;
} sx126x_mod_params_bpsk_t;

typedef struct sx126x_pkt_params_bpsk_s
{
    uint8_t  pld_len_in_bytes;
    uint16_t ramp_up_delay;
    uint16_t ramp_down_delay;
    uint16_t pld_len_in_bits;
} sx126x_pkt_params_bpsk_t;
```

### 4.7 Estado del Chip

```c
typedef struct sx126x_chip_status_s
{
    sx126x_cmd_status_t cmd_status;
    sx126x_chip_modes_t chip_mode;
} sx126x_chip_status_t;
```

### 4.8 Buffer RX Status

```c
typedef struct sx126x_rx_buffer_status_s
{
    uint8_t pld_len_in_bytes;
    uint8_t buffer_start_pointer;
} sx126x_rx_buffer_status_t;
```

### 4.9 Estado de Paquete LoRa Recibido

```c
typedef struct sx126x_pkt_status_lora_s
{
    int8_t rssi_pkt_in_dbm;          // RSSI del paquete
    int8_t snr_pkt_in_db;            // SNR del paquete
    int8_t signal_rssi_pkt_in_dbm;   // RSSI de senal estimada
} sx126x_pkt_status_lora_t;
```

### 4.10 Estado de Paquete GFSK Recibido

```c
typedef struct sx126x_pkt_status_gfsk_s
{
    sx126x_rx_status_gfsk_t rx_status;
    int8_t rssi_sync;
    int8_t rssi_avg;
} sx126x_pkt_status_gfsk_t;

typedef struct sx126x_rx_status_gfsk_s
{
    bool pkt_sent;
    bool pkt_received;
    bool abort_error;
    bool length_error;
    bool crc_error;
    bool adrs_error;
} sx126x_rx_status_gfsk_t;
```

### 4.11 Estadisticas

```c
typedef struct sx126x_stats_lora_s
{
    uint16_t nb_pkt_received;
    uint16_t nb_pkt_crc_error;
    uint16_t nb_pkt_header_error;
} sx126x_stats_lora_t;

typedef struct sx126x_stats_gfsk_s
{
    uint16_t nb_pkt_received;
    uint16_t nb_pkt_crc_error;
    uint16_t nb_pkt_len_error;
} sx126x_stats_gfsk_t;
```

### 4.12 Parametros CAD

```c
typedef struct sx126x_cad_params_s
{
    sx126x_cad_symbs_t      cad_symb_nb;      // Simbolos a escanear
    uint8_t                 cad_detect_peak;   // Umbral deteccion (22-25)
    uint8_t                 cad_detect_min;    // Filtro pico minimo
    sx126x_cad_exit_modes_t cad_exit_mode;     // Accion post-CAD
    uint32_t                cad_timeout;       // Timeout para RX post-CAD
} sx126x_cad_params_t;
```

---

## PARTE 5: CONSTANTES Y MACROS

```c
// Timeout
#define SX126X_MAX_TIMEOUT_IN_RTC_STEP  0x00FFFFFE
#define SX126X_RX_SINGLE_MODE           0x00000000
#define SX126X_RX_CONTINUOUS            0x00FFFFFF

// Overcurrent Protection
#define SX126X_OCP_PARAM_VALUE_60_MA    0x18
#define SX126X_OCP_PARAM_VALUE_140_MA   0x38

// Cristal
#define SX126X_XTAL_TRIMMING_CAPACITOR_DEFAULT_VALUE  0x12

// LoRa
#define SX126X_MAX_LORA_SYMB_NUM_TIMEOUT  248
#define SX126X_MAX_NB_REG_IN_RETENTION    4
#define SX126X_IMAGE_CALIBRATION_STEP_IN_MHZ  4

// PLL
#define SX126X_XTAL_FREQ              32000000UL
#define SX126X_RTC_FREQ_IN_HZ         64000UL
#define SX126X_PLL_STEP_SHIFT_AMOUNT  14
#define SX126X_PLL_STEP_SCALED        (SX126X_XTAL_FREQ >> (25 - SX126X_PLL_STEP_SHIFT_AMOUNT))
```

---

## PARTE 6: CODIGO FUENTE DE FUNCIONES CLAVE (sx126x.c)

### 6.1 Configurar Frecuencia

```c
sx126x_status_t sx126x_set_rf_freq( const void* context, const uint32_t freq_in_hz )
{
    const uint32_t freq = sx126x_convert_freq_in_hz_to_pll_step( freq_in_hz );
    return sx126x_set_rf_freq_in_pll_steps( context, freq );
}

sx126x_status_t sx126x_set_rf_freq_in_pll_steps( const void* context, const uint32_t freq )
{
    const uint8_t buf[SX126X_SIZE_SET_RF_FREQUENCY] = {
        SX126X_SET_RF_FREQUENCY,       // 0x86
        ( uint8_t )( freq >> 24 ),
        ( uint8_t )( freq >> 16 ),
        ( uint8_t )( freq >> 8 ),
        ( uint8_t )( freq >> 0 ),
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf, SX126X_SIZE_SET_RF_FREQUENCY, 0, 0 );
}

uint32_t sx126x_convert_freq_in_hz_to_pll_step( uint32_t freq_in_hz )
{
    uint32_t steps_int;
    uint32_t steps_frac;

    steps_int  = freq_in_hz / SX126X_PLL_STEP_SCALED;
    steps_frac = freq_in_hz - ( steps_int * SX126X_PLL_STEP_SCALED );

    return ( steps_int << SX126X_PLL_STEP_SHIFT_AMOUNT ) +
           ( ( ( steps_frac << SX126X_PLL_STEP_SHIFT_AMOUNT ) + ( SX126X_PLL_STEP_SCALED >> 1 ) ) /
             SX126X_PLL_STEP_SCALED );
}
```

### 6.2 Calibracion de Imagen

```c
sx126x_status_t sx126x_cal_img( const void* context, const uint8_t freq1, const uint8_t freq2 )
{
    const uint8_t buf[SX126X_SIZE_CALIBRATE_IMAGE] = {
        SX126X_CALIBRATE_IMAGE,   // 0x98
        freq1,
        freq2,
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf, SX126X_SIZE_CALIBRATE_IMAGE, 0, 0 );
}

sx126x_status_t sx126x_cal_img_in_mhz( const void* context,
                                        const uint16_t freq1_in_mhz,
                                        const uint16_t freq2_in_mhz )
{
    const uint8_t freq1 = freq1_in_mhz / SX126X_IMAGE_CALIBRATION_STEP_IN_MHZ;
    const uint8_t freq2 = ( freq2_in_mhz + SX126X_IMAGE_CALIBRATION_STEP_IN_MHZ - 1 )
                          / SX126X_IMAGE_CALIBRATION_STEP_IN_MHZ;
    return sx126x_cal_img( context, freq1, freq2 );
}
```

### 6.3 Configurar Modulacion LoRa (SF + BW + CR)

```c
sx126x_status_t sx126x_set_lora_mod_params( const void* context,
                                             const sx126x_mod_params_lora_t* params )
{
    const uint8_t buf[SX126X_SIZE_SET_MODULATION_PARAMS_LORA] = {
        SX126X_SET_MODULATION_PARAMS,  // 0x8B
        ( uint8_t )( params->sf ),     // Byte 1: SF  (0x05-0x0C)
        ( uint8_t )( params->bw ),     // Byte 2: BW  (0-10)
        ( uint8_t )( params->cr ),     // Byte 3: CR  (0x01-0x04)
        params->ldro & 0x01,           // Byte 4: LDRO (0 o 1)
    };

    sx126x_status_t status =
        ( sx126x_status_t ) sx126x_hal_write( context, buf,
                                               SX126X_SIZE_SET_MODULATION_PARAMS_LORA, 0, 0 );
    if( status == SX126X_STATUS_OK )
    {
        // WORKAROUND - Modulation Quality with 500 kHz BW (DS 15.1)
        status = sx126x_tx_modulation_workaround( context, SX126X_PKT_TYPE_LORA, params->bw );
    }
    return status;
}
```

### 6.4 Workaround BW 500kHz (funcion interna)

```c
static sx126x_status_t sx126x_tx_modulation_workaround( const void* context,
                                                         sx126x_pkt_type_t pkt_type,
                                                         sx126x_lora_bw_t  bw )
{
    uint8_t reg_value = 0;
    sx126x_status_t status = sx126x_read_register( context, SX126X_REG_TX_MODULATION,
                                                    &reg_value, 1 );  // Reg 0x0889
    if( status == SX126X_STATUS_OK )
    {
        if( pkt_type == SX126X_PKT_TYPE_LORA )
        {
            if( bw == SX126X_LORA_BW_500 )
                reg_value &= ~( 1 << 2 );  // Bit 2 = 0 para BW500
            else
                reg_value |= ( 1 << 2 );   // Bit 2 = 1 para otros BW
        }
        status = sx126x_write_register( context, SX126X_REG_TX_MODULATION, &reg_value, 1 );
    }
    return status;
}
```

### 6.5 Configurar Paquete LoRa

```c
sx126x_status_t sx126x_set_lora_pkt_params( const void* context,
                                             const sx126x_pkt_params_lora_t* params )
{
    const uint8_t buf[SX126X_SIZE_SET_PKT_PARAMS_LORA] = {
        SX126X_SET_PKT_PARAMS,                          // 0x8C
        ( uint8_t )( params->preamble_len_in_symb >> 8 ),
        ( uint8_t )( params->preamble_len_in_symb >> 0 ),
        ( uint8_t )( params->header_type ),
        params->pld_len_in_bytes,
        ( uint8_t )( params->crc_is_on ? 1 : 0 ),
        ( uint8_t )( params->invert_iq_is_on ? 1 : 0 ),
    };

    sx126x_status_t status =
        ( sx126x_status_t ) sx126x_hal_write( context, buf,
                                               SX126X_SIZE_SET_PKT_PARAMS_LORA, 0, 0 );
    if( status == SX126X_STATUS_OK )
    {
        // WORKAROUND - Optimizing the Inverted IQ Operation (DS 15.4)
        uint8_t reg_value = 0;
        status = sx126x_read_register( context, SX126X_REG_IQ_POLARITY,
                                        &reg_value, 1 );  // Reg 0x0736
        if( status == SX126X_STATUS_OK )
        {
            if( params->invert_iq_is_on == true )
                reg_value &= ~( 1 << 0 );  // Bit 0 = 0
            else
                reg_value |= ( 1 << 0 );   // Bit 0 = 1
            status = sx126x_write_register( context, SX126X_REG_IQ_POLARITY,
                                             &reg_value, 1 );
        }
    }
    return status;
}
```

### 6.6 Configurar PA y Potencia TX

```c
sx126x_status_t sx126x_set_pa_cfg( const void* context, const sx126x_pa_cfg_params_t* params )
{
    const uint8_t buf[SX126X_SIZE_SET_PA_CFG] = {
        SX126X_SET_PA_CFG,          // 0x95
        params->pa_duty_cycle,
        params->hp_max,
        params->device_sel,
        params->pa_lut,
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf, SX126X_SIZE_SET_PA_CFG, 0, 0 );
}

sx126x_status_t sx126x_set_tx_params( const void* context,
                                       const int8_t pwr_in_dbm,
                                       const sx126x_ramp_time_t ramp_time )
{
    const uint8_t buf[SX126X_SIZE_SET_TX_PARAMS] = {
        SX126X_SET_TX_PARAMS,       // 0x8E
        ( uint8_t ) pwr_in_dbm,
        ( uint8_t ) ramp_time,
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf, SX126X_SIZE_SET_TX_PARAMS, 0, 0 );
}
```

### 6.7 Control de Estado

```c
sx126x_status_t sx126x_set_sleep( const void* context, const sx126x_sleep_cfgs_t cfg )
{
    const uint8_t buf[SX126X_SIZE_SET_SLEEP] = {
        SX126X_SET_SLEEP,   // 0x84
        ( uint8_t ) cfg,
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf, SX126X_SIZE_SET_SLEEP, 0, 0 );
}

sx126x_status_t sx126x_set_standby( const void* context, const sx126x_standby_cfgs_t cfg )
{
    const uint8_t buf[SX126X_SIZE_SET_STANDBY] = {
        SX126X_SET_STANDBY,  // 0x80
        ( uint8_t ) cfg,
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf, SX126X_SIZE_SET_STANDBY, 0, 0 );
}

sx126x_status_t sx126x_set_tx( const void* context, const uint32_t timeout_in_ms )
{
    if( timeout_in_ms > SX126X_MAX_TIMEOUT_IN_MS )
        return SX126X_STATUS_UNKNOWN_VALUE;
    const uint32_t timeout_in_rtc_step = sx126x_convert_timeout_in_ms_to_rtc_step( timeout_in_ms );
    return sx126x_set_tx_with_timeout_in_rtc_step( context, timeout_in_rtc_step );
}

sx126x_status_t sx126x_set_tx_with_timeout_in_rtc_step( const void* context,
                                                         const uint32_t timeout_in_rtc_step )
{
    const uint8_t buf[SX126X_SIZE_SET_TX] = {
        SX126X_SET_TX,                              // 0x83
        ( uint8_t )( timeout_in_rtc_step >> 16 ),
        ( uint8_t )( timeout_in_rtc_step >> 8 ),
        ( uint8_t )( timeout_in_rtc_step >> 0 ),
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf, SX126X_SIZE_SET_TX, 0, 0 );
}

sx126x_status_t sx126x_set_rx( const void* context, const uint32_t timeout_in_ms )
{
    if( timeout_in_ms > SX126X_MAX_TIMEOUT_IN_MS )
        return SX126X_STATUS_UNKNOWN_VALUE;
    const uint32_t timeout_in_rtc_step = sx126x_convert_timeout_in_ms_to_rtc_step( timeout_in_ms );
    return sx126x_set_rx_with_timeout_in_rtc_step( context, timeout_in_rtc_step );
}

sx126x_status_t sx126x_set_rx_with_timeout_in_rtc_step( const void* context,
                                                         const uint32_t timeout_in_rtc_step )
{
    const uint8_t buf[SX126X_SIZE_SET_RX] = {
        SX126X_SET_RX,                              // 0x82
        ( uint8_t )( timeout_in_rtc_step >> 16 ),
        ( uint8_t )( timeout_in_rtc_step >> 8 ),
        ( uint8_t )( timeout_in_rtc_step >> 0 ),
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf, SX126X_SIZE_SET_RX, 0, 0 );
}
```

### 6.8 Duty Cycle RX

```c
sx126x_status_t sx126x_set_rx_duty_cycle( const void* context,
                                           const uint32_t rx_time_in_ms,
                                           const uint32_t sleep_time_in_ms )
{
    const uint32_t rx_time_rtc    = sx126x_convert_timeout_in_ms_to_rtc_step( rx_time_in_ms );
    const uint32_t sleep_time_rtc = sx126x_convert_timeout_in_ms_to_rtc_step( sleep_time_in_ms );
    return sx126x_set_rx_duty_cycle_with_timings_in_rtc_step( context, rx_time_rtc, sleep_time_rtc );
}

sx126x_status_t sx126x_set_rx_duty_cycle_with_timings_in_rtc_step(
    const void* context,
    const uint32_t rx_time_in_rtc_step,
    const uint32_t sleep_time_in_rtc_step )
{
    const uint8_t buf[SX126X_SIZE_SET_RX_DUTY_CYCLE] = {
        SX126X_SET_RX_DUTY_CYCLE,                       // 0x94
        ( uint8_t )( rx_time_in_rtc_step >> 16 ),
        ( uint8_t )( rx_time_in_rtc_step >> 8 ),
        ( uint8_t )( rx_time_in_rtc_step >> 0 ),
        ( uint8_t )( sleep_time_in_rtc_step >> 16 ),
        ( uint8_t )( sleep_time_in_rtc_step >> 8 ),
        ( uint8_t )( sleep_time_in_rtc_step >> 0 ),
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf,
                                                  SX126X_SIZE_SET_RX_DUTY_CYCLE, 0, 0 );
}
```

### 6.9 IRQ

```c
sx126x_status_t sx126x_set_dio_irq_params( const void* context,
                                            const uint16_t irq_mask,
                                            const uint16_t dio1_mask,
                                            const uint16_t dio2_mask,
                                            const uint16_t dio3_mask )
{
    const uint8_t buf[SX126X_SIZE_SET_DIO_IRQ_PARAMS] = {
        SX126X_SET_DIO_IRQ_PARAMS,       // 0x08
        ( uint8_t )( irq_mask >> 8 ),
        ( uint8_t )( irq_mask >> 0 ),
        ( uint8_t )( dio1_mask >> 8 ),
        ( uint8_t )( dio1_mask >> 0 ),
        ( uint8_t )( dio2_mask >> 8 ),
        ( uint8_t )( dio2_mask >> 0 ),
        ( uint8_t )( dio3_mask >> 8 ),
        ( uint8_t )( dio3_mask >> 0 ),
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf,
                                                  SX126X_SIZE_SET_DIO_IRQ_PARAMS, 0, 0 );
}

sx126x_status_t sx126x_get_irq_status( const void* context, sx126x_irq_mask_t* irq )
{
    const uint8_t buf[SX126X_SIZE_GET_IRQ_STATUS] = { SX126X_GET_IRQ_STATUS };  // 0x12
    uint8_t       irq_local[sizeof( sx126x_irq_mask_t )] = { 0x00 };

    sx126x_status_t status =
        ( sx126x_status_t ) sx126x_hal_read( context, buf, SX126X_SIZE_GET_IRQ_STATUS,
                                              irq_local, sizeof( sx126x_irq_mask_t ) );
    if( status == SX126X_STATUS_OK )
        *irq = ( ( sx126x_irq_mask_t ) irq_local[0] << 8 ) + ( ( sx126x_irq_mask_t ) irq_local[1] );
    return status;
}

sx126x_status_t sx126x_clear_irq_status( const void* context, const sx126x_irq_mask_t irq_mask )
{
    const uint8_t buf[SX126X_SIZE_CLR_IRQ_STATUS] = {
        SX126X_CLR_IRQ_STATUS,           // 0x02
        ( uint8_t )( irq_mask >> 8 ),
        ( uint8_t )( irq_mask >> 0 ),
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf, SX126X_SIZE_CLR_IRQ_STATUS, 0, 0 );
}

sx126x_status_t sx126x_get_and_clear_irq_status( const void* context, sx126x_irq_mask_t* irq )
{
    sx126x_irq_mask_t irq_local;
    sx126x_status_t status = sx126x_get_irq_status( context, &irq_local );
    if( status == SX126X_STATUS_OK )
    {
        status = sx126x_clear_irq_status( context, irq_local );
        if( ( status == SX126X_STATUS_OK ) && ( irq != NULL ) )
            *irq = irq_local;
    }
    return status;
}
```

### 6.10 DIO2/DIO3 Control

```c
sx126x_status_t sx126x_set_dio2_as_rf_sw_ctrl( const void* context, const bool enable )
{
    const uint8_t buf[SX126X_SIZE_SET_DIO2_AS_RF_SWITCH_CTRL] = {
        SX126X_SET_DIO2_AS_RF_SWITCH_CTRL,  // 0x9D
        ( uint8_t )( enable ? 1 : 0 ),
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf,
                                                  SX126X_SIZE_SET_DIO2_AS_RF_SWITCH_CTRL, 0, 0 );
}

sx126x_status_t sx126x_set_dio3_as_tcxo_ctrl( const void* context,
                                               const sx126x_tcxo_ctrl_voltages_t tcxo_voltage,
                                               const uint32_t timeout )
{
    const uint8_t buf[SX126X_SIZE_SET_DIO3_AS_TCXO_CTRL] = {
        SX126X_SET_DIO3_AS_TCXO_CTRL,  // 0x97
        ( uint8_t ) tcxo_voltage,
        ( uint8_t )( timeout >> 16 ),
        ( uint8_t )( timeout >> 8 ),
        ( uint8_t )( timeout >> 0 ),
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf,
                                                  SX126X_SIZE_SET_DIO3_AS_TCXO_CTRL, 0, 0 );
}
```

### 6.11 Buffer y Registros

```c
sx126x_status_t sx126x_set_buffer_base_address( const void* context,
                                                 const uint8_t tx_base_address,
                                                 const uint8_t rx_base_address )
{
    const uint8_t buf[SX126X_SIZE_SET_BUFFER_BASE_ADDRESS] = {
        SX126X_SET_BUFFER_BASE_ADDRESS,   // 0x8F
        tx_base_address,
        rx_base_address,
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf,
                                                  SX126X_SIZE_SET_BUFFER_BASE_ADDRESS, 0, 0 );
}

sx126x_status_t sx126x_write_buffer( const void* context, const uint8_t offset,
                                      const uint8_t* buffer, const uint8_t size )
{
    const uint8_t buf[SX126X_SIZE_WRITE_BUFFER] = { SX126X_WRITE_BUFFER, offset };  // 0x0E
    return ( sx126x_status_t ) sx126x_hal_write( context, buf,
                                                  SX126X_SIZE_WRITE_BUFFER, buffer, size );
}

sx126x_status_t sx126x_read_buffer( const void* context, const uint8_t offset,
                                     uint8_t* buffer, const uint8_t size )
{
    const uint8_t buf[SX126X_SIZE_READ_BUFFER] = { SX126X_READ_BUFFER, offset };  // 0x1E
    return ( sx126x_status_t ) sx126x_hal_read( context, buf,
                                                 SX126X_SIZE_READ_BUFFER, buffer, size );
}

sx126x_status_t sx126x_write_register( const void* context, const uint16_t address,
                                        const uint8_t* buffer, const uint8_t size )
{
    const uint8_t buf[SX126X_SIZE_WRITE_REGISTER] = {
        SX126X_WRITE_REGISTER,              // 0x0D
        ( uint8_t )( address >> 8 ),
        ( uint8_t )( address >> 0 ),
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf,
                                                  SX126X_SIZE_WRITE_REGISTER, buffer, size );
}

sx126x_status_t sx126x_read_register( const void* context, const uint16_t address,
                                       uint8_t* buffer, const uint8_t size )
{
    const uint8_t buf[SX126X_SIZE_READ_REGISTER] = {
        SX126X_READ_REGISTER,              // 0x1D
        ( uint8_t )( address >> 8 ),
        ( uint8_t )( address >> 0 ),
    };
    return ( sx126x_status_t ) sx126x_hal_read( context, buf,
                                                 SX126X_SIZE_READ_REGISTER, buffer, size );
}
```

### 6.12 Lectura de Status y Diagnostico

```c
sx126x_status_t sx126x_get_status( const void* context, sx126x_chip_status_t* radio_status )
{
    const uint8_t buf[SX126X_SIZE_GET_STATUS] = { SX126X_GET_STATUS };  // 0xC0
    uint8_t       status_local = 0;

    sx126x_status_t status =
        ( sx126x_status_t ) sx126x_hal_read( context, buf, SX126X_SIZE_GET_STATUS,
                                              &status_local, 1 );
    if( status == SX126X_STATUS_OK )
    {
        radio_status->cmd_status =
            ( sx126x_cmd_status_t )( ( status_local & SX126X_CMD_STATUS_MASK ) >> SX126X_CMD_STATUS_POS );
        radio_status->chip_mode =
            ( sx126x_chip_modes_t )( ( status_local & SX126X_CHIP_MODES_MASK ) >> SX126X_CHIP_MODES_POS );
    }
    return status;
}

sx126x_status_t sx126x_get_rx_buffer_status( const void* context,
                                              sx126x_rx_buffer_status_t* rx_buffer_status )
{
    const uint8_t buf[SX126X_SIZE_GET_RX_BUFFER_STATUS] = { SX126X_GET_RX_BUFFER_STATUS };  // 0x13
    uint8_t       status_local[sizeof( sx126x_rx_buffer_status_t )] = { 0x00 };

    sx126x_status_t status =
        ( sx126x_status_t ) sx126x_hal_read( context, buf, SX126X_SIZE_GET_RX_BUFFER_STATUS,
                                              status_local, sizeof( sx126x_rx_buffer_status_t ) );
    if( status == SX126X_STATUS_OK )
    {
        rx_buffer_status->pld_len_in_bytes    = status_local[0];
        rx_buffer_status->buffer_start_pointer = status_local[1];
    }
    return status;
}

sx126x_status_t sx126x_get_lora_pkt_status( const void* context,
                                             sx126x_pkt_status_lora_t* pkt_status )
{
    const uint8_t buf[SX126X_SIZE_GET_PKT_STATUS] = { SX126X_GET_PKT_STATUS };  // 0x14
    uint8_t       pkt_status_local[3] = { 0x00 };

    sx126x_status_t status =
        ( sx126x_status_t ) sx126x_hal_read( context, buf, SX126X_SIZE_GET_PKT_STATUS,
                                              pkt_status_local, 3 );
    if( status == SX126X_STATUS_OK )
    {
        pkt_status->rssi_pkt_in_dbm        = ( int8_t )( -pkt_status_local[0] >> 1 );
        pkt_status->snr_pkt_in_db          = ( ( ( int8_t ) pkt_status_local[1] ) >> 2 );
        pkt_status->signal_rssi_pkt_in_dbm = ( int8_t )( -pkt_status_local[2] >> 1 );
    }
    return status;
}

sx126x_status_t sx126x_get_rssi_inst( const void* context, int16_t* rssi_in_dbm )
{
    const uint8_t buf[SX126X_SIZE_GET_RSSI_INST] = { SX126X_GET_RSSI_INST };  // 0x15
    uint8_t       rssi_local = 0x00;

    sx126x_status_t status =
        ( sx126x_status_t ) sx126x_hal_read( context, buf, SX126X_SIZE_GET_RSSI_INST,
                                              &rssi_local, 1 );
    if( status == SX126X_STATUS_OK )
        *rssi_in_dbm = ( int8_t )( -rssi_local >> 1 );
    return status;
}
```

### 6.13 Estadisticas

```c
sx126x_status_t sx126x_get_lora_stats( const void* context, sx126x_stats_lora_t* stats )
{
    const uint8_t buf[SX126X_SIZE_GET_STATS] = { SX126X_GET_STATS };  // 0x10
    uint8_t       stats_local[6] = { 0 };

    sx126x_status_t status =
        ( sx126x_status_t ) sx126x_hal_read( context, buf, SX126X_SIZE_GET_STATS,
                                              stats_local, sizeof( stats_local ) );
    if( status == SX126X_STATUS_OK )
    {
        stats->nb_pkt_received     = ( ( uint16_t ) stats_local[0] << 8 ) + stats_local[1];
        stats->nb_pkt_crc_error    = ( ( uint16_t ) stats_local[2] << 8 ) + stats_local[3];
        stats->nb_pkt_header_error = ( ( uint16_t ) stats_local[4] << 8 ) + stats_local[5];
    }
    return status;
}

sx126x_status_t sx126x_reset_stats( const void* context )
{
    const uint8_t buf[SX126X_SIZE_RESET_STATS] = {
        SX126X_RESET_STATS, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf, SX126X_SIZE_RESET_STATS, 0, 0 );
}
```

### 6.14 Funciones Avanzadas de Registro

```c
sx126x_status_t sx126x_cfg_rx_boosted( const void* context, const bool state )
{
    uint8_t reg_value = ( state == true ) ? 0x96 : 0x94;
    return sx126x_write_register( context, SX126X_REG_RXGAIN, &reg_value, 1 );  // 0x08AC
}

sx126x_status_t sx126x_set_lora_sync_word( const void* context, const uint8_t sync_word )
{
    uint8_t buffer[2] = { 0x00 };
    sx126x_status_t status = sx126x_read_register( context, SX126X_REG_LR_SYNCWORD,
                                                    buffer, 2 );  // 0x0740
    if( status == SX126X_STATUS_OK )
    {
        buffer[0] = ( buffer[0] & ~0xF0 ) | ( sync_word & 0xF0 );
        buffer[1] = ( buffer[1] & ~0xF0 ) | ( ( sync_word & 0x0F ) << 4 );
        status = sx126x_write_register( context, SX126X_REG_LR_SYNCWORD, buffer, 2 );
    }
    return status;
}

sx126x_status_t sx126x_cfg_tx_clamp( const void* context )
{
    uint8_t reg_value = 0;
    sx126x_status_t status = sx126x_read_register( context, SX126X_REG_TX_CLAMP_CFG,
                                                    &reg_value, 1 );  // 0x08D8
    if( status == SX126X_STATUS_OK )
    {
        reg_value |= SX126X_REG_TX_CLAMP_CFG_MASK;  // bits [4:1] = 1111
        status = sx126x_write_register( context, SX126X_REG_TX_CLAMP_CFG, &reg_value, 1 );
    }
    return status;
}

sx126x_status_t sx126x_set_ocp_value( const void* context, const uint8_t ocp_in_step_of_2_5_ma )
{
    return sx126x_write_register( context, SX126X_REG_OCP, &ocp_in_step_of_2_5_ma, 1 ); // 0x08E7
}

sx126x_status_t sx126x_set_trimming_capacitor_values( const void* context,
                                                       const uint8_t trimming_cap_xta,
                                                       const uint8_t trimming_cap_xtb )
{
    uint8_t buf[2] = { trimming_cap_xta, trimming_cap_xtb };
    return sx126x_write_register( context, SX126X_REG_XTATRIM, buf, 2 );  // 0x0911
}
```

### 6.15 Generador de Numeros Aleatorios

```c
sx126x_status_t sx126x_get_random_numbers( const void* context,
                                            uint32_t* numbers, unsigned int n )
{
    uint8_t tmp_ana_lna   = 0x00;
    uint8_t tmp_ana_mixer = 0x00;
    uint8_t tmp           = 0x00;

    sx126x_status_t status;

    // 1. Guardar registros LNA y Mixer
    status = sx126x_read_register( context, SX126X_REG_ANA_LNA, &tmp_ana_lna, 1 );
    if( status != SX126X_STATUS_OK ) return status;

    status = sx126x_read_register( context, SX126X_REG_ANA_MIXER, &tmp_ana_mixer, 1 );
    if( status != SX126X_STATUS_OK ) return status;

    // 2. Deshabilitar LNA y Mixer -> genera ruido puro
    tmp = tmp_ana_lna & ~( 1 << 0 );
    status = sx126x_write_register( context, SX126X_REG_ANA_LNA, &tmp, 1 );
    if( status != SX126X_STATUS_OK ) return status;

    tmp = tmp_ana_mixer & ~( 1 << 7 );
    status = sx126x_write_register( context, SX126X_REG_ANA_MIXER, &tmp, 1 );
    if( status != SX126X_STATUS_OK ) return status;

    // 3. Poner en RX continuo
    status = sx126x_set_rx_with_timeout_in_rtc_step( context, SX126X_RX_CONTINUOUS );
    if( status != SX126X_STATUS_OK ) return status;

    // 4. Leer numeros aleatorios del registro RNG
    for( unsigned int i = 0; i < n; i++ )
    {
        status = sx126x_read_register( context, SX126X_REG_RNGBASEADDRESS,
                                        ( uint8_t* ) &numbers[i], 4 );  // 0x0819
        if( status != SX126X_STATUS_OK ) return status;
    }

    // 5. Volver a standby
    status = sx126x_set_standby( context, SX126X_STANDBY_CFG_RC );
    if( status != SX126X_STATUS_OK ) return status;

    // 6. Restaurar LNA y Mixer
    status = sx126x_write_register( context, SX126X_REG_ANA_LNA, &tmp_ana_lna, 1 );
    if( status != SX126X_STATUS_OK ) return status;

    status = sx126x_write_register( context, SX126X_REG_ANA_MIXER, &tmp_ana_mixer, 1 );
    return status;
}
```

### 6.16 Calculo Time-on-Air LoRa

```c
uint32_t sx126x_get_lora_time_on_air_in_ms( const sx126x_pkt_params_lora_t* pkt_p,
                                              const sx126x_mod_params_lora_t* mod_p )
{
    const uint32_t numerator   = 1000U * sx126x_get_lora_time_on_air_numerator( pkt_p, mod_p );
    const uint32_t denominator = sx126x_get_lora_bw_in_hz( mod_p->bw );
    // ceil()
    return ( numerator + denominator - 1 ) / denominator;
}

uint32_t sx126x_get_lora_time_on_air_numerator( const sx126x_pkt_params_lora_t* pkt_p,
                                                  const sx126x_mod_params_lora_t* mod_p )
{
    const int32_t pld_len_in_bytes = pkt_p->pld_len_in_bytes;
    const int32_t sf               = mod_p->sf;
    const bool    pld_is_fix       = pkt_p->header_type == SX126X_LORA_PKT_IMPLICIT;
    const int32_t cr_denom         = mod_p->cr + 4;

    int32_t ceil_denominator;
    int32_t ceil_numerator =
        ( pld_len_in_bytes << 3 ) + ( pkt_p->crc_is_on ? 16 : 0 ) - ( 4 * sf ) + ( pld_is_fix ? 0 : 20 );

    if( sf <= 6 )
        ceil_denominator = 4 * sf;
    else
        ceil_denominator = 4 * ( sf - ( mod_p->ldro << 1 ) );

    if( ceil_numerator > 0 )
        ceil_numerator = ( ( ( ceil_numerator + ceil_denominator - 1 ) / ceil_denominator ) * cr_denom )
                         + 8;
    else
        ceil_numerator = 8;

    // SF5-6: 6.25 simbolos fijos de preambulo; SF7+: 4.25
    int32_t intermed;
    if( sf <= 6 )
        intermed = ceil_numerator * ( 1 << sf ) +
                   ( pkt_p->preamble_len_in_symb << ( sf + 2 ) ) + ( 25 * ( 1 << sf ) );
    else
        intermed = ceil_numerator * ( 1 << sf ) +
                   ( pkt_p->preamble_len_in_symb << ( sf + 2 ) ) + ( 17 * ( 1 << sf ) );

    return ( uint32_t ) intermed;
}

// Conversion BW enum a Hz
uint32_t sx126x_get_lora_bw_in_hz( sx126x_lora_bw_t bw )
{
    uint32_t bw_in_hz = 0;
    switch( bw )
    {
        case SX126X_LORA_BW_007: bw_in_hz = 7812UL;   break;
        case SX126X_LORA_BW_010: bw_in_hz = 10417UL;  break;
        case SX126X_LORA_BW_015: bw_in_hz = 15625UL;  break;
        case SX126X_LORA_BW_020: bw_in_hz = 20833UL;  break;
        case SX126X_LORA_BW_031: bw_in_hz = 31250UL;  break;
        case SX126X_LORA_BW_041: bw_in_hz = 41667UL;  break;
        case SX126X_LORA_BW_062: bw_in_hz = 62500UL;  break;
        case SX126X_LORA_BW_125: bw_in_hz = 125000UL; break;
        case SX126X_LORA_BW_250: bw_in_hz = 250000UL; break;
        case SX126X_LORA_BW_500: bw_in_hz = 500000UL; break;
    }
    return bw_in_hz;
}
```

### 6.17 Conversion de Timeout

```c
uint32_t sx126x_convert_timeout_in_ms_to_rtc_step( uint32_t timeout_in_ms )
{
    return ( uint32_t )( timeout_in_ms * ( SX126X_RTC_FREQ_IN_HZ / 1000 ) );
    // timeout_ms * 64 (porque RTC = 64kHz, 1 step = 15.625us)
}
```

### 6.18 CAD

```c
sx126x_status_t sx126x_set_cad( const void* context )
{
    const uint8_t buf[SX126X_SIZE_SET_CAD] = { SX126X_SET_CAD };  // 0xC5
    return ( sx126x_status_t ) sx126x_hal_write( context, buf, SX126X_SIZE_SET_CAD, 0, 0 );
}

sx126x_status_t sx126x_set_cad_params( const void* context, const sx126x_cad_params_t* params )
{
    const uint8_t buf[SX126X_SIZE_SET_CAD_PARAMS] = {
        SX126X_SET_CAD_PARAMS,                        // 0x88
        ( uint8_t ) params->cad_symb_nb,
        params->cad_detect_peak,
        params->cad_detect_min,
        ( uint8_t ) params->cad_exit_mode,
        ( uint8_t )( params->cad_timeout >> 16 ),
        ( uint8_t )( params->cad_timeout >> 8 ),
        ( uint8_t )( params->cad_timeout >> 0 ),
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf, SX126X_SIZE_SET_CAD_PARAMS, 0, 0 );
}
```

### 6.19 Calibracion General y Regulador

```c
sx126x_status_t sx126x_cal( const void* context, const sx126x_cal_mask_t param )
{
    const uint8_t buf[SX126X_SIZE_CALIBRATE] = {
        SX126X_CALIBRATE,            // 0x89
        ( uint8_t ) param,
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf, SX126X_SIZE_CALIBRATE, 0, 0 );
}

sx126x_status_t sx126x_set_reg_mode( const void* context, const sx126x_reg_mods_t mode )
{
    const uint8_t buf[SX126X_SIZE_SET_REG_MODE] = {
        SX126X_SET_REG_MODE,         // 0x96
        ( uint8_t ) mode,
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf, SX126X_SIZE_SET_REG_MODE, 0, 0 );
}

sx126x_status_t sx126x_set_rx_tx_fallback_mode( const void* context,
                                                 const sx126x_fallback_modes_t fallback_mode )
{
    const uint8_t buf[SX126X_SIZE_SET_RX_TX_FALLBACK_MODE] = {
        SX126X_SET_RX_TX_FALLBACK_MODE,  // 0x93
        ( uint8_t ) fallback_mode,
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf,
                                                  SX126X_SIZE_SET_RX_TX_FALLBACK_MODE, 0, 0 );
}
```

### 6.20 Reset, Wakeup, Errores

```c
sx126x_status_t sx126x_reset( const void* context )
{
    return ( sx126x_status_t ) sx126x_hal_reset( context );
}

sx126x_status_t sx126x_wakeup( const void* context )
{
    return ( sx126x_status_t ) sx126x_hal_wakeup( context );
}

sx126x_status_t sx126x_get_device_errors( const void* context, sx126x_errors_mask_t* errors )
{
    const uint8_t buf[SX126X_SIZE_GET_DEVICE_ERRORS] = { SX126X_GET_DEVICE_ERRORS };  // 0x17
    uint8_t       errors_local[sizeof( sx126x_errors_mask_t )] = { 0x00 };

    sx126x_status_t status =
        ( sx126x_status_t ) sx126x_hal_read( context, buf, SX126X_SIZE_GET_DEVICE_ERRORS,
                                              errors_local, sizeof( sx126x_errors_mask_t ) );
    if( status == SX126X_STATUS_OK )
        *errors = ( ( sx126x_errors_mask_t ) errors_local[0] << 8 ) + errors_local[1];
    return status;
}

sx126x_status_t sx126x_clear_device_errors( const void* context )
{
    const uint8_t buf[SX126X_SIZE_CLR_DEVICE_ERRORS] = {
        SX126X_CLR_DEVICE_ERRORS, 0x00,
    };
    return ( sx126x_status_t ) sx126x_hal_write( context, buf, SX126X_SIZE_CLR_DEVICE_ERRORS, 0, 0 );
}
```

---

## PARTE 7: HAL - Las 4 funciones que debes implementar

```c
// === sx126x_hal.h ===

// 1. Escribir datos al chip via SPI
sx126x_hal_status_t sx126x_hal_write(
    const void*    context,         // Tu estructura con SPI, pines, etc.
    const uint8_t* command,         // Opcode + parametros
    uint16_t       command_length,
    const uint8_t* data,            // Datos opcionales (payload)
    uint16_t       data_length
);
// Protocolo: NSS=LOW -> BUSY=LOW -> enviar command -> enviar data -> NSS=HIGH

// 2. Leer datos del chip via SPI
sx126x_hal_status_t sx126x_hal_read(
    const void*    context,
    const uint8_t* command,
    uint16_t       command_length,
    uint8_t*       data,            // Buffer de recepcion
    uint16_t       data_length
);
// Protocolo: NSS=LOW -> BUSY=LOW -> enviar command -> leer data -> NSS=HIGH

// 3. Reset del chip
sx126x_hal_status_t sx126x_hal_reset( const void* context );
// Protocolo: RESET=LOW -> delay 1ms -> RESET=HIGH -> delay 6ms

// 4. Despertar del sleep
sx126x_hal_status_t sx126x_hal_wakeup( const void* context );
// Protocolo: NSS=LOW -> delay 1ms -> NSS=HIGH
```

---

## PARTE 8: SECUENCIA COMPLETA DE INICIALIZACION

```c
// === Ejemplo completo para WiFi LoRa 32 V4.2 (SX1262 @ 915 MHz) ===

// 1. Reset hardware
sx126x_reset( context );

// 2. Modo standby
sx126x_set_standby( context, SX126X_STANDBY_CFG_RC );

// 3. Regulador DC-DC (mas eficiente)
sx126x_set_reg_mode( context, SX126X_REG_MODE_DCDC );

// 4. DIO2 como RF switch automatico
sx126x_set_dio2_as_rf_sw_ctrl( context, true );

// 5. Si usa TCXO (WiFi LoRa 32 usa cristal, no TCXO normalmente)
// sx126x_set_dio3_as_tcxo_ctrl( context, SX126X_TCXO_CTRL_1_8V, timeout );

// 6. Calibracion completa
sx126x_cal( context, SX126X_CAL_ALL );

// 7. Calibracion de imagen para banda 915 MHz
sx126x_cal_img_in_mhz( context, 902, 928 );

// 8. Workaround TX clamp
sx126x_cfg_tx_clamp( context );

// 9. Tipo de paquete: LoRa
sx126x_set_pkt_type( context, SX126X_PKT_TYPE_LORA );

// 10. Frecuencia: 915 MHz
sx126x_set_rf_freq( context, 915000000 );

// 11. Configurar Power Amplifier (SX1262)
sx126x_pa_cfg_params_t pa_cfg = {
    .pa_duty_cycle = 0x04,
    .hp_max        = 0x07,
    .device_sel    = 0x00,   // SX1262
    .pa_lut        = 0x01,
};
sx126x_set_pa_cfg( context, &pa_cfg );

// 12. Potencia TX: 14 dBm, rampa 40us
sx126x_set_tx_params( context, 14, SX126X_RAMP_40_US );

// 13. Modulacion LoRa: SF7, BW125, CR4/5, LDRO=off
sx126x_mod_params_lora_t mod_params = {
    .sf   = SX126X_LORA_SF7,
    .bw   = SX126X_LORA_BW_125,
    .cr   = SX126X_LORA_CR_4_5,
    .ldro = 0,
};
sx126x_set_lora_mod_params( context, &mod_params );

// 14. Paquete LoRa: preambulo 8, explicit header, 20 bytes, CRC on
sx126x_pkt_params_lora_t pkt_params = {
    .preamble_len_in_symb = 8,
    .header_type          = SX126X_LORA_PKT_EXPLICIT,
    .pld_len_in_bytes     = 20,
    .crc_is_on            = true,
    .invert_iq_is_on      = false,
};
sx126x_set_lora_pkt_params( context, &pkt_params );

// 15. Sync word: red privada (0x12)
sx126x_set_lora_sync_word( context, 0x12 );

// 16. Buffer base: TX=0, RX=0
sx126x_set_buffer_base_address( context, 0x00, 0x00 );

// 17. IRQ: TX_DONE + RX_DONE + TIMEOUT en DIO1
sx126x_set_dio_irq_params( context,
    SX126X_IRQ_TX_DONE | SX126X_IRQ_RX_DONE | SX126X_IRQ_TIMEOUT |
    SX126X_IRQ_CRC_ERROR | SX126X_IRQ_HEADER_ERROR,
    SX126X_IRQ_TX_DONE | SX126X_IRQ_RX_DONE | SX126X_IRQ_TIMEOUT |
    SX126X_IRQ_CRC_ERROR | SX126X_IRQ_HEADER_ERROR,
    SX126X_IRQ_NONE,
    SX126X_IRQ_NONE
);

// 18. Opcional: RX boosted para mayor sensibilidad
sx126x_cfg_rx_boosted( context, true );
```

### Transmitir un paquete:

```c
uint8_t payload[] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F };  // "Hello"
sx126x_write_buffer( context, 0, payload, sizeof(payload) );
sx126x_set_tx( context, 2000 );  // TX con timeout 2 segundos

// Esperar IRQ DIO1...
sx126x_irq_mask_t irq;
sx126x_get_and_clear_irq_status( context, &irq );
if( irq & SX126X_IRQ_TX_DONE ) {
    // Transmision exitosa
}
```

### Recibir un paquete:

```c
sx126x_set_rx( context, 5000 );  // RX con timeout 5 segundos

// Esperar IRQ DIO1...
sx126x_irq_mask_t irq;
sx126x_get_and_clear_irq_status( context, &irq );
if( irq & SX126X_IRQ_RX_DONE ) {
    sx126x_rx_buffer_status_t buf_status;
    sx126x_get_rx_buffer_status( context, &buf_status );

    uint8_t payload[255];
    sx126x_read_buffer( context, buf_status.buffer_start_pointer,
                        payload, buf_status.pld_len_in_bytes );

    sx126x_pkt_status_lora_t pkt_status;
    sx126x_get_lora_pkt_status( context, &pkt_status );
    // pkt_status.rssi_pkt_in_dbm  -> potencia recibida
    // pkt_status.snr_pkt_in_db    -> relacion senal/ruido
}
```

---

## PARTE 9: TABLA DE TRAMAS SPI POR FUNCION

| Funcion | Trama SPI (hex) | Descripcion |
|---|---|---|
| set_sleep(WARM) | 84 04 | Sleep con retencion RAM |
| set_standby(RC) | 80 00 | Standby RC 13MHz |
| set_standby(XOSC) | 80 01 | Standby cristal |
| set_tx(2000ms) | 83 01 F4 00 | TX timeout 2s |
| set_rx(5000ms) | 82 04 E2 00 | RX timeout 5s |
| set_rx(continuous) | 82 FF FF FF | RX continuo |
| set_cad() | C5 | Iniciar CAD |
| set_tx_cw() | D1 | Onda continua |
| set_rf_freq(915MHz) | 86 39 30 00 00 | 915 MHz |
| set_rf_freq(868MHz) | 86 36 40 00 00 | 868 MHz |
| cal(ALL) | 89 7F | Calibrar todo |
| cal_img(902,928) | 98 E1 E9 | Imagen 902-928 MHz |
| set_pkt_type(LORA) | 8A 01 | Modo LoRa |
| set_mod_params(SF7,BW125,CR4/5) | 8B 07 04 01 00 | Modulacion LoRa |
| set_mod_params(SF12,BW125,CR4/8) | 8B 0C 04 04 01 | SF12+LDRO |
| set_pa_cfg(SX1262) | 95 04 07 00 01 | PA SX1262 full |
| set_tx_params(14dBm) | 8E 0E 02 | 14dBm rampa 40us |
| set_tx_params(22dBm) | 8E 16 02 | 22dBm rampa 40us |
| set_reg_mode(DCDC) | 96 01 | Regulador DC-DC |
| set_dio2_rf_sw(true) | 9D 01 | DIO2 RF switch |
| get_status() | C0 | Leer estado |
| get_rssi_inst() | 15 | RSSI instantaneo |
| write_buffer(0,data) | 0E 00 [data...] | Cargar payload |
| read_buffer(0) | 1E 00 [data...] | Leer payload |

---

Documento generado para tesis.
Fuente: sx126x_driver-2.3.2 (Semtech Corporation, Clear BSD License)
