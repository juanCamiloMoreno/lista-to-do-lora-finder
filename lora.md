mira en el siguiente carpeta hai varias carpetas, cada una de ella tiene
las librerias,de lora, y en la carpeta mayor estan los datasheets de los
elementos de la tesis, quiero que analises las librerias y me des un
panorama de lo que sepuede hacer y de lo que no

Explored LoRa library directories and compiled comprehensive file report

Explored LoRa library directories and compiled comprehensive file report

Show more

Show more

Aqui tienes el panorama completo basado en el analisis de las librerias
y datasheets:

**Panorama del Proyecto LoRa**

**Hardware disponible (Datasheets)**

  -----------------------------------------------------------------------
  **Componente**        **Descripcion**
  --------------------- -------------------------------------------------
  **WiFi LoRa 32 V4.2** Placa principal con ESP32-S3 + SX1262 integrado

  **ESP32-S3**          Microcontrolador principal (WiFi + BLE)

  **SX1261/1262**       Transceptor LoRa de Semtech

  **BME280**            Sensor ambiental (temperatura, humedad, presion)

  **L76K GNSS**         Modulo GPS/GNSS de Quectel

  **SSD1315**           Controlador OLED (display)

  **Buzzer SMD9018**    Buzzer SMD

  **GC1109**            Componente adicional

  **ICOE CM121**        Modulo adicional

  **Expansion Board     Placa de expansion
  V0.7**                
  -----------------------------------------------------------------------

**Librerias disponibles**

**1. sx126x_driver-2.3.2 --- Driver C del SX126x**

El driver de bajo nivel mas importante. Expone **+50 funciones** para
control directo del chip:

-   Configurar frecuencia, potencia, modulacion (LoRa/GFSK/BPSK)

-   Transmitir y recibir paquetes

-   CAD (Channel Activity Detection)

-   RSSI instantaneo, estadisticas, errores

-   Control de IRQ, sleep, wakeup

**Limitacion:** Requiere que el usuario implemente el HAL
(sx126x_hal_write, sx126x_hal_read, etc.) para la plataforma objetivo.
Las implementaciones incluidas son para STM32, **no para ESP32**.

**2. SWSD003-2.3.0 --- SDK de ejemplos (Transceiver mode)**

Ejemplos completos de uso en modo transceptor puro (sin stack LoRaWAN):

-   Ping-pong entre dos dispositivos

-   PER (Packet Error Rate testing)

-   Escaneo espectral / RSSI heatmap

-   Onda continua (CW) para pruebas de RF

-   CAD demo

**Limitacion:** Los ejemplos y HAL estan implementados
para **NUCLEO-L476RG (STM32L4)**, no para ESP32.

**3. SWL2001-4.5.0 --- LoRa Basics Modem (Stack LoRaWAN completo)**

Stack LoRaWAN L2 1.0.4 completo con:

-   FUOTA (actualizacion firmware OTA)

-   Geolocationcon LoRa Edge

-   Integracion LoRaCloud

-   Certificacion LCTT

**Limitacion importante:** Esta disenado para **STM32U5, STM32L4 y
NRF52840**. El soporte para ESP32 **no esta incluido**. Portar el LBM al
ESP32 requiere implementar toda la capa HAL manualmente.

**4. sx126x_cadexample --- Ejemplo CAD**

Codigo de ejemplo especifico para Channel Activity Detection con soporte
multi-MCU STM32.

**5. SX126xDevKit_Nucleo_L476RG_191017 --- Firmware pre-compilado**

Binario listo para flashear en NUCLEO-L476RG. Solo util si se tiene esa
placa.

**6. SX1261Calculator_setup --- Herramienta de calculo**

Utilidad Windows para calcular parametros de configuracion del
SX1261/1262.

**Lo que SE PUEDE hacer**

-   **Comunicacion LoRa punto a punto** entre dos placas WiFi LoRa 32,
    usando el driver sx126x_driver portado a ESP32 via Arduino o ESP-IDF

-   **Leer sensores BME280** (temperatura/humedad/presion) y
    transmitirlos por LoRa

-   **GNSS + LoRa**: leer posicion GPS con L76K y enviarla por LoRa

-   **Display OLED SSD1315**: mostrar datos localmente

-   **CAD (deteccion de canal)**: saber si hay actividad LoRa antes de
    transmitir

-   **Ping-pong RF**: pruebas de enlace bidireccional

-   **Medicion de RSSI y calidad de enlace**

-   **LoRaWAN** usando librerias de terceros para Arduino/ESP32 (LMIC,
    RadioLib), ya que el stack SWL2001 necesaria portarse

**Lo que NO SE PUEDE hacer directamente (sin trabajo adicional)**

-   **Usar SWL2001 (LoRa Basics Modem) en ESP32 directamente** --- el
    HAL no existe para ESP32, habria que portarlo completamente

-   **Usar los ejemplos SWSD003 directamente en la placa** --- son para
    STM32, no para ESP32

-   **Flashear el binario precompilado** en la WiFi LoRa 32 --- es para
    NUCLEO-L476RG (STM32)

-   **FUOTA certificado con Semtech LBM** --- requiere hardware
    especifico (LR1110/LR1120 o STM32)

**Conclusion**

El proyecto tiene todos los componentes de hardware necesarios para un
nodo LoRa completo con GPS y sensores. La **brecha principal** es que
las librerias oficiales de Semtech estan orientadas a STM32, por lo que
para trabajar en la **WiFi LoRa 32 (ESP32-S3)** lo mas practico es
usar **RadioLib** o **LMIC** en Arduino/ESP-IDF, que ya tienen soporte
para SX1262 en ESP32, y complementarlas con el conocimiento de los
datasheets que tienes.

enfoquemosnos en las tres librerias de lora, quiero que me des una
descricion de funciones y llamadas a registros completa en la cual
desvriba las ventjas de usar una o la otra

Voy a hacer un análisis profundo de las tres librerías LoRa. Déjame leer
los archivos clave de cada una.

Completed thorough analysis of three LoRa libraries and SDKs

Completed thorough analysis of three LoRa libraries and SDKs

Show more

Show more

Show more

Show more

Show more

Show more

Show more

Show more

Show more

Show more

Show more

Show more

Show more

Show more

Show more

Show more

Show more

Aqui tienes el análisis comparativo completo de las tres librerías LoRa:

**Análisis Comparativo Completo: Las 3 Librerías LoRa**

**1. sx126x_driver-2.3.2 --- Driver de Bajo Nivel**

**Nivel de abstracción**

**Capa más baja**: Habla directamente con el chip SX1261/62/68 via SPI.
Cada función mapea 1:1 a un comando SPI del datasheet.

**Funciones principales (\~60 funciones públicas)**

  -------------------------------------------------------------------------------------------------------------------------------------------------
  **Categoría**            **Funciones**                                                                                          **Descripción**
  ------------------------ ------------------------------------------------------------------------------------------------------ -----------------
  **Control de estado**    set_sleep(), set_standby(), set_fs(), reset(), wakeup()                                                Manejan los modos
                                                                                                                                  de operación del
                                                                                                                                  chip

  **TX**                   set_tx(), set_tx_cw(), set_tx_infinite_preamble()                                                      Transmisión con
                                                                                                                                  timeout, onda
                                                                                                                                  continua,
                                                                                                                                  preámbulo
                                                                                                                                  infinito

  **RX**                   set_rx(), set_rx_duty_cycle(), stop_timer_on_preamble()                                                Recepción con
                                                                                                                                  timeout, duty
                                                                                                                                  cycling para
                                                                                                                                  ahorro de energía

  **CAD**                  set_cad(), set_cad_params()                                                                            Channel Activity
                                                                                                                                  Detection (solo
                                                                                                                                  LoRa)

  **Configuración RF**     set_rf_freq(), set_pkt_type(), set_tx_params(), set_pa_cfg()                                           Frecuencia, tipo
                                                                                                                                  de paquete,
                                                                                                                                  potencia,
                                                                                                                                  amplificador

  **Modulación LoRa**      set_lora_mod_params(), set_lora_pkt_params(), set_lora_sync_word()                                     SF5-SF12, BW
                                                                                                                                  7.8kHz-500kHz, CR
                                                                                                                                  4/5-4/8, LDRO

  **Modulación GFSK**      set_gfsk_mod_params(), set_gfsk_pkt_params(), set_gfsk_sync_word(), set_gfsk_crc_seed()                Bitrate,
                                                                                                                                  desviación,
                                                                                                                                  filtro, CRC,
                                                                                                                                  whitening

  **Modulación BPSK**      set_bpsk_mod_params(), set_bpsk_pkt_params()                                                           Para protocolo
                                                                                                                                  Sigfox

  **LR-FHSS**              lr_fhss_init(), lr_fhss_build_frame(), lr_fhss_handle_hop()                                            Frequency Hopping
                                                                                                                                  Spread Spectrum

  **IRQ**                  set_dio_irq_params(), get_irq_status(), clear_irq_status()                                             Control de
                                                                                                                                  interrupciones
                                                                                                                                  DIO1/2/3

  **Buffer**               write_buffer(), read_buffer(), set_buffer_base_address()                                               Acceso al buffer
                                                                                                                                  TX/RX de 256
                                                                                                                                  bytes

  **Registros**            write_register(), read_register()                                                                      Acceso directo a
                                                                                                                                  registros
                                                                                                                                  internos

  **Estado/Diagnóstico**   get_status(), get_rssi_inst(), get_lora_pkt_status(), get_gfsk_stats(), get_device_errors()            RSSI, SNR,
                                                                                                                                  contadores de
                                                                                                                                  paquetes, errores

  **Calibración**          cal(), cal_img(), cal_img_in_mhz()                                                                     Calibración RC,
                                                                                                                                  PLL, ADC, imagen

  **Utilidades**           get_random_numbers(), get_lora_time_on_air_in_ms(), get_gfsk_time_on_air_in_ms()                       RNG del chip,
                                                                                                                                  cálculo de
                                                                                                                                  airtime

  **Avanzado**             cfg_rx_boosted(), set_dio2_as_rf_sw_ctrl(), set_dio3_as_tcxo_ctrl(), set_trimming_capacitor_values()   RF switch, TCXO,
                                                                                                                                  trimming cristal
  -------------------------------------------------------------------------------------------------------------------------------------------------

**Registros clave**

  ------------------------------------------------------------------------------
  **Registro**            **Dirección**   **Función**
  ----------------------- --------------- --------------------------------------
  SYNCWORDBASEADDRESS     0x06C0          Sync word GFSK

  LR_SYNCWORD             0x0740          Sync word LoRa (0x12=privado,
                                          0x34=público)

  RXGAIN                  0x08AC          Ganancia RX (0x94=ahorro,
                                          0x96=boosted)

  OCP                     0x08E7          Protección de sobrecorriente PA

  RNGBASEADDRESS          0x0819          Generador de números aleatorios

  IQ_POLARITY             0x0736          Corrección polaridad IQ

  TX_CLAMP_CFG            0x08D8          TX clamp (workaround)

  XTATRIM                 0x0911          Trimming capacitor cristal
  ------------------------------------------------------------------------------

**HAL requerido (4 funciones)**

sx126x_hal_write() // Escritura SPI

sx126x_hal_read() // Lectura SPI

sx126x_hal_reset() // Reset GPIO

sx126x_hal_wakeup() // Despertar del sleep

**Ventajas**

-   **Mínimo overhead**: sin capas intermedias

-   **Control total**: acceso a cada registro y función del chip

-   **Portable**: solo 4 funciones HAL para portar a cualquier MCU

-   **Ligero**: \~15KB de flash, mínimo RAM

-   **Soporta TODO**: LoRa, GFSK, BPSK, LR-FHSS

-   **Ideal para**: comunicación punto-a-punto sin LoRaWAN

**Limitaciones**

-   **Sin stack LoRaWAN**: no maneja join, sesiones, MAC commands

-   **Sin gestión de tiempo**: tú manejas los timers

-   **Sin ADR**: tú decides manualmente SF/BW/potencia

-   **Sin duty cycle**: tú calculas los límites regionales

-   **Sin encriptación**: datos viajan en claro

**2. SWSD003-2.3.0 --- SDK de Ejemplos (Transceiver Mode)**

**Nivel de abstracción**

**Capa media**: Envuelve el driver sx126x_driver con un framework de
aplicación, HAL completo para STM32L4, y 9 aplicaciones de ejemplo
listas para compilar.

**Relación con el driver**

Incluye internamente el sx126x_driver-2.3.2. Agrega:

-   HAL implementado para NUCLEO-L476RG

-   Framework común (apps_common) de inicialización

-   Sistema de callbacks para IRQ

-   Configuración por shields (9 shields soportados)

-   9 aplicaciones demostrativas completas

**Aplicaciones incluidas**

  ---------------------------------------------------------------------------
  **Ejemplo**     **Modo**    **Qué hace**
  --------------- ----------- -----------------------------------------------
  **Ping-Pong**   LoRa/GFSK   Comunicación bidireccional entre 2 nodos,
                              auto-detección master/slave

  **PER**         LoRa/GFSK   Medición de Packet Error Rate con contador
                              rolling y verificación payload

  **CAD**         LoRa        Channel Activity Detection con 3 modos: solo
                              detección, RX si detecta, TX si libre (LBT)

  **Sigfox**      DBPSK       Uplink Sigfox PHY compliant, 7 configuraciones
                              regionales (RC1-RC7)

  **TX CW**       Tono RF     Onda continua para testing de potencia y
                              certificación

  **TX Infinite   LoRa        Preámbulo infinito para pruebas de AGC y
  Preamble**                  sincronización

  **Spectral      RSSI        Histograma RSSI por canal, heat map
  Scan**                      electromagnético (30 canales, 100 muestras)

  **Spectrum      RSSI        Gráfico espectral en tiempo real (requiere
  Display**                   terminal VT100)

  **LR-FHSS**     LR-FHSS     Frequency Hopping Spread Spectrum, máxima
                              resistencia a interferencia
  ---------------------------------------------------------------------------

**Parámetros configurables globales**

  --------------------------------------------------------------------------
  **Parámetro**                       **Default**   **Rango**
  ----------------------------------- ------------- ------------------------
  PACKET_TYPE                         LoRa          LoRa, GFSK

  RF_FREQ_IN_HZ                       490 MHz       Depende del shield

  TX_OUTPUT_POWER_DBM                 14 dBm        -17 a +22 dBm

  LORA_SPREADING_FACTOR               SF7           SF5 a SF12

  LORA_BANDWIDTH                      125 kHz       7.8 kHz a 500 kHz

  LORA_CODING_RATE                    4/5           4/5 a 4/8

  PAYLOAD_LENGTH                      7 bytes       0-255

  ENABLE_RX_BOOST_MODE                false         true/false
  --------------------------------------------------------------------------

**Flujo típico de una aplicación**

main()

→ apps_common_shield_init() // Configura GPIO según shield

→ apps_common_sx126x_init() // Reset + modo radio

→ apps_common_sx126x_radio_init() // SF, BW, freq, potencia

→ loop:

→ apps_common_sx126x_irq_process() // Despacha eventos

→ on_tx_done() / on_rx_done() / on_rx_timeout() / on_cad_done()

**Ventajas**

-   **Ejemplos listos para compilar**: 9 aplicaciones funcionales

-   **HAL completo para STM32L4**: no hay que implementar nada

-   **Referencia de diseño**: muestra cómo usar correctamente el driver

-   **Herramientas de diagnóstico**: spectral scan, PER testing

-   **Soporte multi-shield**: 9 shields con configs de PA optimizadas

-   **Ideal para**: prototipar rápido en NUCLEO-L476RG, aprender el
    driver

**Limitaciones**

-   **Solo STM32L4**: para ESP32 hay que reimplementar todo el HAL

-   **Sin LoRaWAN**: solo modo transceiver (punto-a-punto)

-   **Sin gestión de red**: no hay join, sesiones, ni MAC

-   **Atado al hardware Semtech**: shields específicos de evaluación

**3. SWL2001-4.5.0 --- LoRa Basics Modem (Stack LoRaWAN Completo)**

**Nivel de abstracción**

**Capa más alta**: Stack LoRaWAN completo certificado. Maneja todo:
desde la radio hasta los servicios cloud.

**API Principal (\~120 funciones públicas)**

  --------------------------------------------------------------------------------------------------------------
  **Categoría**        **Funciones clave**                                                 **Descripción**
  -------------------- ------------------------------------------------------------------- ---------------------
  **Inicialización**   smtc_modem_init(), smtc_modem_run_engine()                          Arranque del modem,
                                                                                           loop principal

  **Red**              join_network(), leave_network(), set_region()                       Join OTAA, gestión de
                                                                                           red

  **TX/RX**            request_uplink(), request_emergency_uplink(), get_downlink_data()   Envío/recepción
                                                                                           LoRaWAN

  **Credenciales**     set_deveui(), set_joineui(), set_appkey(), set_nwkkey()             EUI, keys de 128 bits

  **Clase A/B/C**      set_class(), get_class(), class_b_set_ping_slot_periodicity()       Cambio de clase
                                                                                           LoRaWAN

  **ADR**              adr_set_profile(), set_nb_trans()                                   Network-controlled,
                                                                                           mobile-long-range,
                                                                                           mobile-low-power,
                                                                                           custom

  **Multicast**        multicast_set_grp_config(), multicast_class_c_start_session()       Hasta 4 grupos
                                                                                           simultáneos

  **FUOTA**            Automático vía paquetes TS003-TS007                                 Firmware Update Over
                                                                                           The Air

  **Stream**           stream_init(), stream_add_data()                                    Buffer +
                                                                                           fragmentación
                                                                                           automática

  **File Upload**      file_upload_init(), file_upload_start()                             Subida de archivos
                                                                                           hasta 8180 bytes

  **Device Mgmt**      dm_enable(), dm_set_periodic_info_fields()                          Telemetría periódica
                                                                                           al cloud

  **Geolocation**      gnss_scan(), gnss_set_constellations()                              GPS/BEIDOU scanning
                                                                                           (solo LR11xx)

  **ALCSync**          start_alcsync_service(), get_alcsync_time()                         Sincronización de
                                                                                           reloj

  **LBT/CSMA**         lbt_set_parameters(), csma_set_state()                              Listen-Before-Talk,
                                                                                           CSMA

  **Store&Forward**    store_and_forward_flash_add_data()                                  Buffer datos cuando
                                                                                           no hay red

  **Relay TX**         relay_tx_enable(), relay_tx_get_sync_status()                       Protocolo
                                                                                           Wake-on-Radio

  **Alarma**           alarm_start_timer(), alarm_clear_timer()                            Timer interno del
                                                                                           modem

  **MAC**              trig_lorawan_mac_request()                                          Link Check, Device
                                                                                           Time, Ping Slot Info

  **Test**             test_tx(), test_rx_continuous(), test_tx_cw()                       Modo testing RF
  --------------------------------------------------------------------------------------------------------------

**Eventos del sistema (29 tipos)**

  -----------------------------------------------------------------------
  **Evento**                         **Cuándo se dispara**
  ---------------------------------- ------------------------------------
  RESET                              Modem reiniciado

  JOINED                             Join LoRaWAN exitoso

  JOINFAIL                           Fallo en Join

  TXDONE                             Uplink completado

  DOWNDATA                           Downlink recibido

  ALARM                              Timer expiró

  LINK_CHECK                         Respuesta link check

  CLASS_B_STATUS                     Cambio estado Class B

  LORAWAN_MAC_TIME                   Respuesta Device Time

  LORAWAN_FUOTA_DONE                 FUOTA completado

  STREAM_DONE                        Stream vaciado

  UPLOAD_DONE                        File upload completado

  GNSS_SCAN_DONE                     Escaneo GNSS completado

  WIFI_SCAN_DONE                     Escaneo WiFi completado
  -----------------------------------------------------------------------

**Regiones soportadas**

AS923 (Grp1-4), AU915, CN470, EU868, IN865, KR920, RU864, US915, WW2G4

**Radios soportados**

LR1110, LR1120, LR1121, SX1261, SX1262, SX1268, SX1280/81, SX1272/76
(experimental)

**HAL requerido (\~25 funciones)**

  ----------------------------------------------------------------------------------
  **Categoría**    **Funciones**
  ---------------- -----------------------------------------------------------------
  **Tiempo**       get_time_in_s(), get_time_in_ms(), get_time_in_100us()

  **Timer/IRQ**    start_timer(), stop_timer(), enable/disable_modem_irq()

  **NVM**          context_store(), context_restore(), context_flash_pages_erase()

  **Radio**        start_radio_tcxo(), stop_radio_tcxo(), set_ant_switch()

  **RNG**          get_random_nb_in_range()

  **Telemetría**   get_battery_level(), get_temperature(), get_voltage_mv()

  **Sistema**      reset_mcu(), reload_wdog(), on_panic()
  ----------------------------------------------------------------------------------

**Ventajas**

-   **Stack LoRaWAN certificado L2 1.0.4**: cumple el estándar

-   **Clase A, B y C**: todos los modos de operación

-   **FUOTA**: actualización de firmware remota

-   **ADR automático**: el servidor controla SF/potencia

-   **Multicast**: hasta 4 sesiones simultáneas

-   **Duty cycle automático**: cumple regulaciones regionales

-   **Encriptación AES-128**: datos protegidos

-   **Servicios cloud**: stream, file upload, device management

-   **Store & Forward**: no pierde datos sin red

-   **CSMA/LBT**: evita colisiones

**Limitaciones**

-   **Complejidad alta**: \~25 funciones HAL que implementar

-   **NVM obligatoria**: necesita almacenamiento no volátil para
    contexto LoRaWAN

-   **Memoria**: mínimo 40KB Flash + 6KB RAM

-   **Solo para redes LoRaWAN**: no sirve para punto-a-punto directo

-   **Geolocation solo LR11xx**: no disponible con SX126x

-   **Portado solo a STM32/NRF52840**: para ESP32 se necesita portar
    todo el HAL

**Tabla Comparativa Final**

  ---------------------------------------------------------------------------------------
  **Criterio**        **sx126x_driver**        **SWSD003 SDK**          **SWL2001 LBM**
  ------------------- ------------------------ ------------------------ -----------------
  **Nivel**           Driver bajo nivel        SDK + Ejemplos           Stack LoRaWAN
                                                                        completo

  **Funciones HAL**   4                        4 + MCU HAL              \~25

  **Flash**           \~15 KB                  \~30 KB                  \~40-60 KB

  **RAM**             \~1 KB                   \~2 KB                   \~6+ KB

  **LoRaWAN**         No                       No                       Si (L2 1.0.4)

  **Punto a punto**   Si                       Si (ejemplos)            No directo

  **Clase A/B/C**     N/A                      N/A                      Si

  **OTAA/ABP**        N/A                      N/A                      Si

  **ADR**             Manual                   Manual                   Automático

  **Duty Cycle**      Manual                   Manual                   Automático

  **Encriptación**    No                       No                       AES-128

  **FUOTA**           No                       No                       Si

  **Multicast**       No                       No                       Si (4 grupos)

  **Store&Forward**   No                       No                       Si

  **CSMA/LBT**        No                       No                       Si

  **Modulaciones**    LoRa+GFSK+BPSK+LR-FHSS   LoRa+GFSK+BPSK+LR-FHSS   Solo LoRa (via
                                                                        stack)

  **Portabilidad      Fácil (4 funciones)      Difícil (HAL STM32)      Muy difícil (\~25
  ESP32**                                                               funciones)

  **Curva             Media                    Baja (ejemplos listos)   Alta
  aprendizaje**                                                         

  **Uso recomendado** Driver base para tu      Prototipado rápido en    Producto LoRaWAN
                      propia app               STM32                    certificado
  ---------------------------------------------------------------------------------------

**Recomendación para tu tesis (WiFi LoRa 32 V4.2 con ESP32-S3)**

**Para comunicación punto-a-punto (sensores BME280 + GPS):**

-   Usa sx126x_driver-2.3.2 como base. Solo necesitas implementar 4
    funciones HAL para el ESP32 via SPI. Es la ruta más directa y
    liviana.

**Para conectar a una red LoRaWAN (TTN, Chirpstack):**

-   Portar SWL2001 al ESP32 es posible pero complejo. En la práctica,
    para ESP32 es más práctico usar **RadioLib** (que internamente
    implementa lo mismo que el sx126x_driver) con su propio stack
    LoRaWAN ya portado.

**Para entender cómo funciona todo:**

-   Los ejemplos de SWSD003 (ping-pong, PER, CAD) son excelentes como
    referencia de diseño, incluso si no usas STM32. El flujo de
    inicialización y manejo de IRQ es transferible a cualquier
    plataforma.

usemoas esta, sx126x_driver ahora nesecito la explicaicon de los
registros y el codigo

Voy a hacer un análisis exhaustivo del código fuente del driver y todos
sus registros.

Read complete SX126x radio driver codebase thoroughly

Read complete SX126x radio driver codebase thoroughly

Show more

Show more

Show more

Show more

Ahora tengo toda la información. Déjame compilar el análisis completo y
exhaustivo del sx126x_driver-2.3.2.

**sx126x_driver-2.3.2 --- Análisis Completo de Registros, Funciones y
Código**

**PARTE 1: MAPA COMPLETO DE REGISTROS (sx126x_regs.h)**

Cada registro es una dirección de memoria interna del chip SX1261/62/68
accesible vía SPI con write_register() / read_register().

**1.1 Registros de Configuración de Paquetes**

  ------------------------------------------------------------------------------------
  **Registro**             **Dirección**   **Tamaño**   **Función**
  ------------------------ --------------- ------------ ------------------------------
  SYNCWORDBASEADDRESS      0x06C0          8 bytes      Sync word GFSK (patrón de
                                                        sincronización, hasta 64 bits)

  LR_SYNCWORD              0x0740          2 bytes      Sync word LoRa: 0x1424=red
                                                        privada, 0x3444=red pública
                                                        (LoRaWAN)

  CRCSEEDBASEADDRESS       0x06BC          2 bytes      Semilla CRC para GFSK

  CRCPOLYBASEADDRESS       0x06BE          2 bytes      Polinomio CRC para GFSK

  WHITSEEDBASEADDRESS      0x06B8          2 bytes      Semilla de whitening
                                                        (aleatorización de datos GFSK)

  GFSK_NODE_ADDRESS        0x06CD          1 byte       Dirección de nodo GFSK para
                                                        filtrado de paquetes

  GFSK_BROADCAST_ADDRESS   0x06CE          1 byte       Dirección broadcast GFSK

  RXTX_PAYLOAD_LEN         0x06BB          1 byte       Longitud del payload RX/TX
  ------------------------------------------------------------------------------------

**1.2 Registros de Recepción LoRa (Header Parsing)**

  --------------------------------------------------------------------------------------
  **Registro**       **Dirección**   **Bits**   **Función**
  ------------------ --------------- ---------- ----------------------------------------
  LR_HEADER_CR       0x0749          \[6:4\]    Coding Rate extraído del header LoRa
                                                recibido

  LR_HEADER_CRC      0x076B          \[4\]      Flag de CRC del header LoRa recibido
                                                (1=CRC activo)

  LR_SYNCH_TIMEOUT   0x0706          \[7:0\]    Timeout de sincronización: symbols =
                                                \[7:3\] × 2\^(2×\[2:0\]+1)
  --------------------------------------------------------------------------------------

**1.3 Registros de Control de Ganancia y RF**

  --------------------------------------------------------------------------------------
  **Registro**   **Dirección**   **Valores**             **Función**
  -------------- --------------- ----------------------- -------------------------------
  RXGAIN         0x08AC          0x94 / 0x96             Ganancia RX: 0x94=ahorro de
                                                         energía, 0x96=modo boosted
                                                         (+\~2dB sensibilidad)

  OCP            0x08E7          0x18=60mA, 0x38=140mA   Protección de sobrecorriente
                                                         del amplificador de potencia

  ANA_LNA        0x08E2          ---                     Deshabilitación del LNA (Low
                                                         Noise Amplifier)

  ANA_MIXER      0x08E5          ---                     Deshabilitación del mixer de
                                                         recepción

  XTATRIM        0x0911          default 0x12            Capacitores de trimming del
                                                         cristal (ajuste fino de
                                                         frecuencia)
  --------------------------------------------------------------------------------------

**1.4 Registros de Workarounds (Errata del chip)**

  --------------------------------------------------------------------------------------
  **Registro**    **Dirección**   **Bits**   **Workaround**
  --------------- --------------- ---------- -------------------------------------------
  IQ_POLARITY     0x0736          \[0\]      **DS §15.4**: Corrige operación IQ
                                             invertida en LoRa

  TX_MODULATION   0x0889          \[2\]      **DS §15.1**: Corrige calidad de modulación
                                             con BW=500kHz

  TX_CLAMP_CFG    0x08D8          \[4:1\]    **DS §15.2**: Mejora resistencia a
                                             desajuste de antena
  --------------------------------------------------------------------------------------

**1.5 Registros de Control del Sistema**

  --------------------------------------------------------------------------------
  **Registro**                  **Dirección**   **Función**
  ----------------------------- --------------- ----------------------------------
  RTC_CTRL                      0x0902          Control del reloj RTC interno

  EVT_CLR                       0x0944          Limpieza de eventos (bit \[1\] =
                                                timeout)

  RX_ADDRESS_POINTER            0x0803          Puntero de dirección del buffer RX

  RNGBASEADDRESS                0x0819          Generador de números aleatorios
                                                (32 bits)

  RETENTION_LIST_BASE_ADDRESS   0x029F          Base de la lista de retención de
                                                registros en sleep
  --------------------------------------------------------------------------------

**1.6 Registros GPIO (DIO3)**

  ------------------------------------------------------------------------------
  **Registro**        **Dirección**   **Bit**   **Función**
  ------------------- --------------- --------- --------------------------------
  OUT_DIS_REG         0x0580          \[3\]     Deshabilitar salida DIO3

  IN_EN_REG           0x0583          \[3\]     Habilitar entrada DIO3
  ------------------------------------------------------------------------------

**1.7 Registros de Bitbang (BPSK/Sigfox)**

  -------------------------------------------------------------------------------
  **Registro**    **Dirección**   **Bits**          **Función**
  --------------- --------------- ----------------- -----------------------------
  BITBANG_A_REG   0x0680          \[6:4\],          Bitbang TX registro A (para
                                  enable=0x10       BPSK)

  BITBANG_B_REG   0x0587          \[3:0\],          Bitbang TX registro B (para
                                  enable=0x0C       BPSK)
  -------------------------------------------------------------------------------

**PARTE 2: TODAS LAS ENUMERACIONES Y TIPOS DE DATOS (sx126x.h)**

**2.1 Tipos de Retorno**

typedef enum {

SX126X_STATUS_OK = 0,

SX126X_STATUS_UNSUPPORTED_FEATURE,

SX126X_STATUS_UNKNOWN_VALUE,

SX126X_STATUS_ERROR,

} sx126x_status_t;

**2.2 Modos de Operación**

// Modos de Sleep

typedef enum {

SX126X_SLEEP_CFG_COLD_START = 0x00, // RAM se pierde, reinicio completo

SX126X_SLEEP_CFG_WARM_START = 0x04, // RAM se retiene, arranque rápido

} sx126x_sleep_cfgs_t;

// Modos de Standby

typedef enum {

SX126X_STANDBY_CFG_RC = 0x00, // Oscilador RC 13MHz (\~400nA)

SX126X_STANDBY_CFG_XOSC = 0x01, // Cristal externo (\~2µA, más preciso)

} sx126x_standby_cfgs_t;

// Modo del regulador

typedef enum {

SX126X_REG_MODE_LDO = 0x00, // Solo LDO (menos eficiente, más simple)

SX126X_REG_MODE_DCDC = 0x01, // DC-DC + LDO (más eficiente, necesita
inductor)

} sx126x_reg_mods_t;

// Modo fallback después de TX/RX

typedef enum {

SX126X_FALLBACK_STDBY_RC = 0x20, // Vuelve a STDBY_RC

SX126X_FALLBACK_STDBY_XOSC = 0x30, // Vuelve a STDBY_XOSC

SX126X_FALLBACK_FS = 0x40, // Queda en Frequency Synthesis

} sx126x_fallback_modes_t;

// Estados del chip (lectura)

typedef enum {

SX126X_CHIP_MODE_UNUSED = 0,

SX126X_CHIP_MODE_RFU = 1,

SX126X_CHIP_MODE_STBY_RC = 2,

SX126X_CHIP_MODE_STBY_XOSC = 3,

SX126X_CHIP_MODE_FS = 4,

SX126X_CHIP_MODE_RX = 5,

SX126X_CHIP_MODE_TX = 6,

} sx126x_chip_modes_t;

**2.3 Interrupciones (IRQ)**

typedef enum {

SX126X_IRQ_NONE = 0x0000,

SX126X_IRQ_TX_DONE = 0x0001, // Transmisión completada

SX126X_IRQ_RX_DONE = 0x0002, // Paquete recibido correctamente

SX126X_IRQ_PREAMBLE_DETECTED = 0x0004, // Preámbulo detectado

SX126X_IRQ_SYNC_WORD_VALID = 0x0008, // Sync word válida (GFSK)

SX126X_IRQ_HEADER_VALID = 0x0010, // Header LoRa válido

SX126X_IRQ_HEADER_ERROR = 0x0020, // Error en header LoRa

SX126X_IRQ_CRC_ERROR = 0x0040, // Error CRC en payload

SX126X_IRQ_CAD_DONE = 0x0080, // CAD completado

SX126X_IRQ_CAD_DETECTED = 0x0100, // Actividad LoRa detectada en CAD

SX126X_IRQ_TIMEOUT = 0x0200, // Timeout de RX/TX

SX126X_IRQ_LR_FHSS_HOP = 0x4000, // Salto de frecuencia LR-FHSS

SX126X_IRQ_ALL = 0xFFFF,

} sx126x_irq_mask_t;

**2.4 Calibración**

typedef enum {

SX126X_CAL_RC64K = 0x01, // Oscilador RC 64kHz

SX126X_CAL_RC13M = 0x02, // Oscilador RC 13MHz

SX126X_CAL_PLL = 0x04, // PLL (sintetizador)

SX126X_CAL_ADC_PULSE= 0x08, // ADC pulso

SX126X_CAL_ADC_BULK_N=0x10, // ADC bulk N

SX126X_CAL_ADC_BULK_P=0x20, // ADC bulk P

SX126X_CAL_IMAGE = 0x40, // Calibración de imagen

SX126X_CAL_ALL = 0x7F, // Todas las calibraciones

} sx126x_cal_mask_t;

**2.5 Tipos de Paquete**

typedef enum {

SX126X_PKT_TYPE_GFSK = 0x00, // FSK gaussiano

SX126X_PKT_TYPE_LORA = 0x01, // LoRa

SX126X_PKT_TYPE_BPSK = 0x03, // BPSK (Sigfox)

SX126X_PKT_TYPE_LR_FHSS = 0x04, // LR-FHSS

} sx126x_pkt_type_t;

**2.6 Parámetros LoRa**

// Spreading Factor

typedef enum {

SX126X_LORA_SF5 = 0x05, // Más rápido, menor alcance

SX126X_LORA_SF6 = 0x06,

SX126X_LORA_SF7 = 0x07, // ← Default balance velocidad/alcance

SX126X_LORA_SF8 = 0x08,

SX126X_LORA_SF9 = 0x09,

SX126X_LORA_SF10 = 0x0A,

SX126X_LORA_SF11 = 0x0B,

SX126X_LORA_SF12 = 0x0C, // Más lento, máximo alcance

} sx126x_lora_sf_t;

// Bandwidth

typedef enum {

SX126X_LORA_BW_500 = 0x06, // 500 kHz (máxima velocidad)

SX126X_LORA_BW_250 = 0x05, // 250 kHz

SX126X_LORA_BW_125 = 0x04, // 125 kHz ← Default

SX126X_LORA_BW_062 = 0x03, // 62.5 kHz

SX126X_LORA_BW_041 = 0x0A, // 41.67 kHz

SX126X_LORA_BW_031 = 0x09, // 31.25 kHz

SX126X_LORA_BW_020 = 0x08, // 20.83 kHz

SX126X_LORA_BW_015 = 0x07, // 15.63 kHz

SX126X_LORA_BW_010 = 0x02, // 10.42 kHz

SX126X_LORA_BW_007 = 0x01, // 7.81 kHz (máximo alcance)

} sx126x_lora_bw_t;

// Coding Rate (redundancia para corrección de errores)

typedef enum {

SX126X_LORA_CR_4_5 = 0x01, // 4/5 → 25% overhead ← Default

SX126X_LORA_CR_4_6 = 0x02, // 4/6 → 50% overhead

SX126X_LORA_CR_4_7 = 0x03, // 4/7 → 75% overhead

SX126X_LORA_CR_4_8 = 0x04, // 4/8 → 100% overhead (máxima protección)

} sx126x_lora_cr_t;

**2.7 Parámetros GFSK**

// Anchos de banda RX GFSK (21 valores)

typedef enum {

SX126X_GFSK_BW_4800 = 0x1F, // 4.8 kHz

SX126X_GFSK_BW_5800 = 0x17, // 5.8 kHz

SX126X_GFSK_BW_7300 = 0x0F, // 7.3 kHz

SX126X_GFSK_BW_9700 = 0x1E, // 9.7 kHz

SX126X_GFSK_BW_11700 = 0x16, // 11.7 kHz

SX126X_GFSK_BW_14600 = 0x0E, // 14.6 kHz

SX126X_GFSK_BW_19500 = 0x1D, // 19.5 kHz

SX126X_GFSK_BW_23400 = 0x15, // 23.4 kHz

SX126X_GFSK_BW_29300 = 0x0D, // 29.3 kHz

SX126X_GFSK_BW_39000 = 0x1C, // 39.0 kHz

SX126X_GFSK_BW_46900 = 0x14, // 46.9 kHz

SX126X_GFSK_BW_58600 = 0x0C, // 58.6 kHz

SX126X_GFSK_BW_78200 = 0x1B, // 78.2 kHz

SX126X_GFSK_BW_93800 = 0x13, // 93.8 kHz

SX126X_GFSK_BW_117300 = 0x0B, // 117.3 kHz ← Default

SX126X_GFSK_BW_156200 = 0x1A, // 156.2 kHz

SX126X_GFSK_BW_187200 = 0x12, // 187.2 kHz

SX126X_GFSK_BW_234300 = 0x0A, // 234.3 kHz

SX126X_GFSK_BW_312000 = 0x19, // 312.0 kHz

SX126X_GFSK_BW_373600 = 0x11, // 373.6 kHz

SX126X_GFSK_BW_467000 = 0x09, // 467.0 kHz

} sx126x_gfsk_bw_t;

// Pulse shape (filtro gaussiano)

typedef enum {

SX126X_GFSK_PULSE_SHAPE_OFF = 0x00, // Sin filtro

SX126X_GFSK_PULSE_SHAPE_BT_03 = 0x08, // BT=0.3

SX126X_GFSK_PULSE_SHAPE_BT_05 = 0x09, // BT=0.5

SX126X_GFSK_PULSE_SHAPE_BT_07 = 0x0A, // BT=0.7

SX126X_GFSK_PULSE_SHAPE_BT_1 = 0x0B, // BT=1.0

} sx126x_gfsk_pulse_shape_t;

**2.8 CAD (Channel Activity Detection)**

// Número de símbolos a escanear

typedef enum {

SX126X_CAD_01_SYMB = 0x00, // 1 símbolo (más rápido, menos fiable)

SX126X_CAD_02_SYMB = 0x01, // 2 símbolos ← Recomendado

SX126X_CAD_04_SYMB = 0x02, // 4 símbolos

SX126X_CAD_08_SYMB = 0x03, // 8 símbolos

SX126X_CAD_16_SYMB = 0x04, // 16 símbolos (más lento, más fiable)

} sx126x_cad_symbs_t;

// Modo de salida después del CAD

typedef enum {

SX126X_CAD_ONLY = 0x00, // Solo detecta → vuelve a STDBY_RC

SX126X_CAD_RX = 0x01, // Si detecta → entra en RX automático

SX126X_CAD_LBT = 0x02, // Si NO detecta → transmite (Listen-Before-Talk)

} sx126x_cad_exit_modes_t;

**2.9 Estructuras de Datos Principales**

// Configuración del Power Amplifier

typedef struct {

uint8_t pa_duty_cycle; // Duty cycle del PA (0x00-0x07)

uint8_t hp_max; // Potencia máxima HP PA (0x00-0x07)

uint8_t device_sel; // 0x00=SX1262, 0x01=SX1261

uint8_t pa_lut; // Siempre 0x01

} sx126x_pa_cfg_params_t;

// Parámetros de modulación LoRa

typedef struct {

sx126x_lora_sf_t sf; // Spreading Factor

sx126x_lora_bw_t bw; // Bandwidth

sx126x_lora_cr_t cr; // Coding Rate

uint8_t ldro; // Low Data Rate Optimization (0=off, 1=on)

} sx126x_mod_params_lora_t;

// Parámetros de paquete LoRa

typedef struct {

uint16_t preamble_len_in_symb; // 1-65535

sx126x_lora_pkt_len_modes_t header_type; // Explicit/Implicit

uint8_t pld_len_in_bytes; // 0-255

bool crc_is_on;

bool invert_iq_is_on;

} sx126x_pkt_params_lora_t;

// Parámetros de modulación GFSK

typedef struct {

uint32_t br_in_bps; // Bitrate en bps

uint32_t fdev_in_hz; // Desviación de frecuencia en Hz

sx126x_gfsk_pulse_shape_t pulse_shape; // Filtro gaussiano

sx126x_gfsk_bw_t bw_dsb_param; // Ancho de banda RX

} sx126x_mod_params_gfsk_t;

// Estado del chip

typedef struct {

sx126x_cmd_status_t cmd_status; // Estado del último comando

sx126x_chip_modes_t chip_mode; // Modo actual del chip

} sx126x_chip_status_t;

// Estado de paquete LoRa recibido

typedef struct {

int8_t rssi_pkt_in_dbm; // RSSI del paquete

int8_t snr_pkt_in_db; // SNR del paquete

int8_t signal_rssi_pkt_in_dbm; // RSSI de señal estimada

} sx126x_pkt_status_lora_t;

// Estadísticas LoRa

typedef struct {

uint16_t nb_pkt_received; // Paquetes recibidos

uint16_t nb_pkt_crc_error; // Errores CRC

uint16_t nb_pkt_header_error; // Errores de header

} sx126x_stats_lora_t;

// Parámetros CAD

typedef struct {

sx126x_cad_symbs_t cad_symb_nb; // Símbolos a escanear

uint8_t cad_detect_peak; // Umbral de detección (22-25)

uint8_t cad_detect_min; // Filtro de pico mínimo

sx126x_cad_exit_modes_t cad_exit_mode; // Acción post-CAD

uint32_t cad_timeout; // Timeout para RX post-CAD

} sx126x_cad_params_t;

**PARTE 3: IMPLEMENTACIÓN COMPLETA DEL DRIVER (sx126x.c)**

**3.1 Comandos SPI Internos (Opcodes)**

Cada función del driver envía un opcode SPI seguido de los parámetros.
Estos son los opcodes internos:

  -----------------------------------------------------------------------------
  **Opcode**                       **Valor**   **Comando SPI**
  -------------------------------- ----------- --------------------------------
  SET_SLEEP                        0x84        Entrar en sleep

  SET_STANDBY                      0x80        Entrar en standby

  SET_FS                           0xC1        Frequency Synthesis

  SET_TX                           0x83        Transmitir

  SET_RX                           0x82        Recibir

  SET_STOP_TIMER_ON_PREAMBLE       0x9F        Parar timer al detectar
                                               preámbulo

  SET_RX_DUTY_CYCLE                0x94        Duty cycle RX

  SET_CAD                          0xC5        Channel Activity Detection

  SET_TX_CW                        0xD1        Onda continua

  SET_TX_INFINITE_PREAMBLE         0xD2        Preámbulo infinito

  SET_REG_MODE                     0x96        Seleccionar regulador

  CALIBRATE                        0x89        Calibración

  CALIBRATE_IMAGE                  0x98        Calibración de imagen

  SET_PA_CFG                       0x95        Configurar PA

  SET_RX_TX_FALLBACK_MODE          0x93        Modo fallback

  WRITE_REGISTER                   0x0D        Escribir registro

  READ_REGISTER                    0x1D        Leer registro

  WRITE_BUFFER                     0x0E        Escribir buffer

  READ_BUFFER                      0x1E        Leer buffer

  SET_DIO_IRQ_PARAMS               0x08        Configurar IRQ

  GET_IRQ_STATUS                   0x12        Leer estado IRQ

  CLR_IRQ_STATUS                   0x02        Limpiar IRQ

  SET_DIO2_AS_RF_SWITCH            0x9D        DIO2 como RF switch

  SET_DIO3_AS_TCXO_CTRL            0x97        DIO3 como control TCXO

  SET_RF_FREQUENCY                 0x86        Establecer frecuencia

  SET_PKT_TYPE                     0x8A        Tipo de paquete

  GET_PKT_TYPE                     0x11        Leer tipo de paquete

  SET_TX_PARAMS                    0x8E        Potencia y rampa

  SET_MODULATION_PARAMS            0x8B        Parámetros de modulación

  SET_PKT_PARAMS                   0x8C        Parámetros de paquete

  SET_CAD_PARAMS                   0x88        Parámetros CAD

  SET_BUFFER_BASE_ADDRESS          0x8F        Base del buffer TX/RX

  SET_LORA_SYMB_NUM_TIMEOUT        0xA0        Timeout por símbolos LoRa

  GET_STATUS                       0xC0        Leer estado del chip

  GET_RX_BUFFER_STATUS             0x13        Estado del buffer RX

  GET_PKT_STATUS                   0x14        Estado del paquete

  GET_RSSI_INST                    0x15        RSSI instantáneo

  GET_STATS                        0x10        Estadísticas

  RESET_STATS                      0x00        Resetear estadísticas

  GET_DEVICE_ERRORS                0x17        Leer errores

  CLR_DEVICE_ERRORS                0x07        Limpiar errores
  -----------------------------------------------------------------------------

**3.2 Funciones de Control de Estado --- Detalle de Implementación**

**sx126x_set_sleep(cfgs)**

SPI: \[0x84\] \[sleep_cfg_byte\]

-   COLD_START (0x00): RAM se pierde, reinicio completo al despertar
    (\~3.5ms)

-   WARM_START (0x04): RAM retiene configuración, arranque rápido
    (\~0.5ms)

-   **Consumo**: 600nA (cold) / 1.2µA (warm)

**sx126x_set_standby(cfg)**

SPI: \[0x80\] \[standby_cfg\]

-   STDBY_RC (0x00): oscilador RC 13MHz → baja precisión, bajo consumo
    (\~1.6µA)

-   STDBY_XOSC (0x01): cristal externo → alta precisión, mayor consumo
    (\~5µA)

**sx126x_set_tx(timeout_in_ms)**

Internamente:

timeout_rtc = timeout_in_ms × 64 (1 RTC step = 15.625µs)

SPI: \[0x83\] \[timeout_23:16\] \[timeout_15:8\] \[timeout_7:0\]

-   Si timeout = 0x000000: TX sin timeout (transmite y vuelve a
    fallback)

-   Si timeout = 0xFFFFFF: timeout nunca expira

-   **Consumo TX**: \~45mA @ +14dBm, \~118mA @ +22dBm (SX1262)

**sx126x_set_rx(timeout_in_ms)**

SPI: \[0x82\] \[timeout_23:16\] \[timeout_15:8\] \[timeout_7:0\]

-   timeout = 0x000000 → **Single mode**: recibe un paquete y sale

-   timeout = 0xFFFFFF → **Continuous mode**: recibe indefinidamente

-   **Consumo RX**: \~5.6mA (ahorro) / \~6.1mA (boosted)

**sx126x_set_rx_duty_cycle(rx_time_ms, sleep_time_ms)**

SPI: \[0x94\] \[rx_23:16\] \[rx_15:8\] \[rx_7:0\] \[sleep_23:16\]
\[sleep_15:8\] \[sleep_7:0\]

-   Alterna entre RX y Sleep automáticamente

-   Consumo promedio = (rx_time × I_rx + sleep_time × I_sleep) /
    (rx_time + sleep_time)

-   Ideal para nodos alimentados por batería

**sx126x_set_cad()**

SPI: \[0xC5\]

-   Solo funciona en modo LoRa

-   Ejecuta el CAD según los parámetros configurados
    con set_cad_params()

-   Genera IRQ CAD_DONE y opcionalmente CAD_DETECTED

**3.3 Funciones de Configuración RF --- Detalle**

**sx126x_set_rf_freq(freq_in_hz)**

Internamente:

pll_steps = (freq_hz × 2\^25) / 32000000

SPI: \[0x86\] \[freq_31:24\] \[freq_23:16\] \[freq_15:8\] \[freq_7:0\]

-   Rango SX1261: 150 MHz - 960 MHz

-   Rango SX1262: 150 MHz - 960 MHz

-   Resolución: \~0.95 Hz por PLL step

**sx126x_set_tx_params(power_dbm, ramp_time)**

SPI: \[0x8E\] \[power\] \[ramp_time\]

-   SX1261: -17 a +15 dBm

-   SX1262: -9 a +22 dBm

-   Ramp times: 10µs, 20µs, 40µs, 80µs, 200µs, 800µs, 1700µs, 3400µs

**sx126x_set_pa_cfg(pa_duty_cycle, hp_max, device_sel, pa_lut)**

SPI: \[0x95\] \[pa_duty_cycle\] \[hp_max\] \[device_sel\] \[pa_lut\]

Configuraciones típicas del PA:

  -----------------------------------------------------------------------------------
  **Chip**          **pa_duty_cycle**   **hp_max**   **device_sel**   **Potencia
                                                                      máx**
  ----------------- ------------------- ------------ ---------------- ---------------
  SX1261            0x04                0x00         0x01             +15 dBm

  SX1262 (14dBm)    0x02                0x02         0x00             +14 dBm

  SX1262 (22dBm)    0x04                0x07         0x00             +22 dBm
  -----------------------------------------------------------------------------------

**sx126x_set_lora_mod_params(params)**

SPI: \[0x8B\] \[sf\] \[bw\] \[cr\] \[ldro\]

-   Incluye **workaround automático**: si BW=500kHz,
    escribe TX_MODULATION\[2\]=0 (DS §15.1)

-   Si BW≠500kHz, escribe TX_MODULATION\[2\]=1

-   LDRO (Low Data Rate Optimization): obligatorio cuando symbol_time \>
    16.38ms

    -   Regla: activar si (SF ≥ 11 && BW ≤ 125kHz) o (SF = 12 && BW ≤
        250kHz)

**sx126x_set_lora_pkt_params(params)**

SPI: \[0x8C\] \[preamble_hi\] \[preamble_lo\] \[header_type\]
\[pld_len\] \[crc\] \[invert_iq\]

-   Incluye **workaround IQ**: lee registro IQ_POLARITY(0x0736),
    modifica bit\[0\]:

    -   Si invert_iq = true: bit\[0\] = 0

    -   Si invert_iq = false: bit\[0\] = 1

    -   (Corrección del datasheet §15.4)

**3.4 Funciones de Estado y Diagnóstico**

**sx126x_get_status()**

SPI: \[0xC0\] → respuesta en byte de status

Decodifica:

chip_mode = (status \>\> 4) & 0x07 → STBY_RC/STBY_XOSC/FS/RX/TX

cmd_status = (status \>\> 1) & 0x07 →
DATA_AVAILABLE/CMD_TIMEOUT/CMD_ERROR/TX_DONE

**sx126x_get_lora_pkt_status()**

SPI: \[0x14\] + 3 bytes respuesta

rssi_pkt = -raw\[0\] / 2 (dBm)

snr_pkt = (raw\[1\] \< 128) ? raw\[1\]/4 : (raw\[1\]-256)/4 (dB)

signal_rssi = -raw\[2\] / 2 (dBm)

**sx126x_get_rssi_inst()**

SPI: \[0x15\] → 1 byte

rssi = -raw / 2 (dBm)

**3.5 Funciones de Acceso a Registros Avanzadas**

**sx126x_cfg_rx_boosted(enable)**

Si enable=true: write_register(0x08AC, 0x96) → +\~2dB sensibilidad

Si enable=false: write_register(0x08AC, 0x94) → modo ahorro energía

**sx126x_set_lora_sync_word(sync_word)**

Lee registro 0x0740 (2 bytes)

byte1 = (sync_word & 0xF0) \| ((byte1_original) & 0x0F)

byte2 = ((sync_word & 0x0F) \<\< 4) \| ((byte2_original) & 0x0F)

Escribe registro 0x0740

-   sync_word = 0x12 → bytes = 0x14, 0x24 (red privada)

-   sync_word = 0x34 → bytes = 0x34, 0x44 (red pública/LoRaWAN)

**sx126x_cfg_tx_clamp()**

Lee registro 0x08D8

reg_value = (reg_value \| 0x1E) // Bits \[4:1\] = 1111

Escribe registro 0x08D8

-   Mejora resistencia a desajuste de antena (VSWR)

**sx126x_get_random_numbers(n, buffer)**

1\. Salva registros ANA_LNA(0x08E2), ANA_MIXER(0x08E5)

2\. Deshabilita LNA y mixer → genera ruido puro

3\. set_rx(0xFFFFFF) → RX continuo para alimentar el ADC

4\. Para cada número aleatorio:

\- Lee 4 bytes de registro RNGBASEADDRESS(0x0819)

5\. set_standby(STDBY_RC)

6\. Restaura ANA_LNA y ANA_MIXER

**sx126x_stop_rtc()**

1\. write_register(0x0902, 0x00) → detiene RTC

2\. Lee registro EVT_CLR(0x0944)

3\. reg_value \|= 0x02 → limpia evento timeout

4\. Escribe registro EVT_CLR

**sx126x_add_registers_to_retention_list(registers\[\], n)**

1\. Lee registro 0x029F → obtiene cantidad actual de registros retenidos

2\. Para cada nuevo registro:

\- Verifica que no esté ya en la lista

\- Verifica que no exceda MAX=4

\- Escribe la dirección (2 bytes) en la siguiente posición

3\. Actualiza el contador en 0x029F

-   Máximo 4 registros pueden sobrevivir al warm sleep

-   Default: retiene RXGAIN(0x08AC) y TX_MODULATION(0x0889)

**3.6 Cálculo de Time-on-Air**

**sx126x_get_lora_time_on_air_in_ms(pkt_params, mod_params)**

Fórmula (del datasheet):

Tsym = 2\^SF / BW_hz

Tpreamble = (preamble_len + 4.25) × Tsym

payloadSymbNb = 8 + max(ceil((8×PL - 4×SF + 28 + 16×CRC - 20×H) /
(4×(SF-2×DE))) × (CR+4), 0)

Donde:

PL = payload length

H = 0 si explicit header, 1 si implicit

DE = 1 si LDRO activado, 0 si no

CRC = 1 si CRC activado, 0 si no

Tpacket = Tpreamble + payloadSymbNb × Tsym

**PARTE 4: HAL --- Hardware Abstraction Layer (sx126x_hal.h)**

**4.1 Las 4 funciones que TÚ debes implementar**

// 1. Escribir datos al chip via SPI

sx126x_hal_status_t sx126x_hal_write(

const void\* context, // Tu estructura con SPI handle, pines, etc.

const uint8_t\* command, // Opcode + parámetros

uint16_t command_length,

const uint8_t\* data, // Datos opcionales (payload, registros)

uint16_t data_length

);

// Protocolo: NSS=LOW → esperar BUSY=LOW → enviar command → enviar data
→ NSS=HIGH

// 2. Leer datos del chip via SPI

sx126x_hal_status_t sx126x_hal_read(

const void\* context,

const uint8_t\* command,

uint16_t command_length,

uint8_t\* data, // Buffer donde se reciben los datos

uint16_t data_length

);

// Protocolo: NSS=LOW → esperar BUSY=LOW → enviar command → leer data →
NSS=HIGH

// 3. Reset del chip

sx126x_hal_status_t sx126x_hal_reset(

const void\* context

);

// Protocolo: RESET=LOW → delay 1ms → RESET=HIGH → delay 6ms

// 4. Despertar del sleep

sx126x_hal_status_t sx126x_hal_wakeup(

const void\* context

);

// Protocolo: NSS=LOW → delay 1ms → NSS=HIGH (el chip detecta el flanco)

**4.2 Implementación típica para ESP32**

// Ejemplo conceptual:

typedef struct {

spi_device_handle_t spi;

gpio_num_t nss;

gpio_num_t reset;

gpio_num_t busy;

gpio_num_t dio1;

} my_sx126x_context_t;

sx126x_hal_status_t sx126x_hal_write(const void\* ctx, \...) {

my_sx126x_context_t\* c = (my_sx126x_context_t\*)ctx;

while(gpio_get_level(c-\>busy)); // Esperar BUSY=LOW

gpio_set_level(c-\>nss, 0); // NSS=LOW

spi_device_transmit(c-\>spi, \...); // Enviar command+data

gpio_set_level(c-\>nss, 1); // NSS=HIGH

return SX126X_HAL_STATUS_OK;

}

**PARTE 5: LR-FHSS --- Frequency Hopping Spread Spectrum**

**5.1 Parámetros LR-FHSS**

// Tipo de modulación

LR_FHSS_V1_MODULATION_TYPE_GMSK_488 // GMSK a 488 bps

// Coding Rates

LR_FHSS_V1_CR_5_6 // 5/6 → menos redundancia, más rápido

LR_FHSS_V1_CR_2_3 // 2/3

LR_FHSS_V1_CR_1_2 // 1/2

LR_FHSS_V1_CR_1_3 // 1/3 → máxima redundancia, más robusto

// Grid (resolución de saltos de frecuencia)

LR_FHSS_V1_GRID_25391_HZ // 25.391 kHz (menos saltos, más rápido)

LR_FHSS_V1_GRID_3906_HZ // 3.906 kHz (más saltos, más resistente)

// Bandwidth (10 opciones)

LR_FHSS_V1_BW_39063_HZ // 39 kHz

LR_FHSS_V1_BW_85938_HZ // 86 kHz

LR_FHSS_V1_BW_136719_HZ // 137 kHz ← Default

LR_FHSS_V1_BW_183594_HZ // 184 kHz

LR_FHSS_V1_BW_335938_HZ // 336 kHz

LR_FHSS_V1_BW_386719_HZ // 387 kHz

LR_FHSS_V1_BW_722656_HZ // 723 kHz

LR_FHSS_V1_BW_773438_HZ // 773 kHz

LR_FHSS_V1_BW_1523438_HZ // 1523 kHz

LR_FHSS_V1_BW_1574219_HZ // 1574 kHz

**5.2 Funciones LR-FHSS**

  -------------------------------------------------------------------------------
  **Función**                              **Descripción**
  ---------------------------------------- --------------------------------------
  sx126x_lr_fhss_init()                    Configura radio para LR-FHSS
                                           (modulación + paquete)

  sx126x_lr_fhss_process_parameters()      Calcula parámetros de hop a partir de
                                           la configuración

  sx126x_lr_fhss_build_frame()             Construye frame completo: header +
                                           payload con FEC

  sx126x_lr_fhss_handle_hop()              Maneja cada salto de frecuencia (ISR
                                           del IRQ HOP)

  sx126x_lr_fhss_handle_tx_done()          Limpieza post-transmisión

  sx126x_lr_fhss_get_time_on_air_in_ms()   Calcula airtime del frame completo
  -------------------------------------------------------------------------------

**5.3 Procesamiento MAC LR-FHSS**

El archivo lr_fhss_mac.c implementa:

-   **Codificación Viterbi** (convolucional rate 1/2 y 1/3)

-   **Interleaving** de header y payload

-   **Whitening** con LFSR

-   **CRC** separado para header y payload

-   **Generación de secuencia de saltos** basada en polinomio LFSR

**RESUMEN VISUAL: Flujo de una Transmisión LoRa**

┌──────────────────────────────────────────────────────┐

│ INICIALIZACIÓN │

│ │

│ sx126x_reset() ← Reset hardware │

│ sx126x_set_standby(STDBY_RC) ← Modo base │

│ sx126x_set_reg_mode(DCDC) ← Regulador │

│ sx126x_set_dio2_as_rf_sw_ctrl() ← RF switch auto │

│ sx126x_set_dio3_as_tcxo_ctrl() ← Si usa TCXO │

│ sx126x_cal(CAL_ALL) ← Calibración │

│ sx126x_cfg_tx_clamp() ← Workaround │

│ sx126x_set_pkt_type(LORA) ← Modo LoRa │

│ sx126x_set_rf_freq(868000000) ← 868 MHz │

│ sx126x_set_pa_cfg(\...) ← Config PA │

│ sx126x_set_tx_params(14, 40us) ← 14dBm, 40µs │

│ sx126x_set_lora_mod_params( ← Modulación │

│ SF7, BW125, CR4_5, LDRO=0) │

│ sx126x_set_lora_pkt_params( ← Paquete │

│ preamble=8, EXPLICIT, len, │

│ crc=ON, iq=OFF) │

│ sx126x_set_lora_sync_word(0x12) ← Red privada │

│ sx126x_set_buffer_base_address(0,0) ← Buffers │

│ sx126x_set_dio_irq_params( ← Interrupciones │

│ TX_DONE \| RX_DONE \| TIMEOUT) │

└──────────────────────────────────────────────────────┘

│

▼

┌──────────────────────────────────────────────────────┐

│ TRANSMISIÓN │

│ │

│ sx126x_write_buffer(0, data, len) ← Cargar payload │

│ sx126x_set_tx(timeout_ms) ← ¡TRANSMITIR! │

│ \...espera IRQ en DIO1\... │

│ sx126x_get_and_clear_irq_status() ← ¿TX_DONE? │

└──────────────────────────────────────────────────────┘

│

▼

┌──────────────────────────────────────────────────────┐

│ RECEPCIÓN │

│ │

│ sx126x_set_rx(timeout_ms) ← ESCUCHAR │

│ \...espera IRQ en DIO1\... │

│ sx126x_get_and_clear_irq_status() ← ¿RX_DONE? │

│ sx126x_get_rx_buffer_status() ← Tamaño+puntero │

│ sx126x_read_buffer(offset, buf) ← Leer datos │

│ sx126x_get_lora_pkt_status() ← RSSI, SNR │

└──────────────────────────────────────────────────────┘

Este es el análisis completo del sx126x_driver-2.3.2. Con esta
información tienes todo lo necesario para implementar las 4 funciones
HAL en tu ESP32-S3 y controlar el SX1262 de tu placa WiFi LoRa 32 V4.2.
¿Quieres que profundice en alguna sección específica o que empecemos con
la implementación del HAL para ESP32?

dame comoun las lienas de codigo del modo configuracon de la banda, el
sf

Voy a buscar las líneas exactas del código fuente donde se configuran
estos parámetros.

Located exact SF, BW, and frequency band configuration code in
sx126x_driver library

Located exact SF, BW, and frequency band configuration code in
sx126x_driver library

Show more

Show more

Show more

Show more

Aquí tienes las líneas exactas del código fuente donde se configura
la **banda (frecuencia)**, el **SF (Spreading Factor)** y el **BW
(Bandwidth)**:

**1. CONFIGURACIÓN DE FRECUENCIA (Banda)**

**Función sx126x_set_rf_freq() --- archivo sx126x.c**

sx126x_status_t sx126x_set_rf_freq( const void\* context, const uint32_t
freq_in_hz )

{

const uint32_t freq = sx126x_convert_freq_in_hz_to_pll_step( freq_in_hz
);

return sx126x_set_rf_freq_in_pll_steps( context, freq );

}

Lo que hace internamente es **convertir Hz a pasos del PLL** y luego
enviar esos 4 bytes por SPI:

sx126x_status_t sx126x_set_rf_freq_in_pll_steps( const void\* context,
const uint32_t freq )

{

const uint8_t buf\[SX126X_SIZE_SET_RF_FREQUENCY\] = {

SX126X_SET_RF_FREQUENCY, // Opcode 0x86

( uint8_t )( freq \>\> 24 ), // Byte más significativo

( uint8_t )( freq \>\> 16 ),

( uint8_t )( freq \>\> 8 ),

( uint8_t )( freq \>\> 0 ), // Byte menos significativo

};

return ( sx126x_status_t ) sx126x_hal_write( context, buf,
SX126X_SIZE_SET_RF_FREQUENCY, 0, 0 );

}

**La fórmula de conversión Hz → PLL steps:**

#define SX126X_XTAL_FREQ 32000000UL

#define SX126X_PLL_STEP_SHIFT_AMOUNT ( 14 )

#define SX126X_PLL_STEP_SCALED ( SX126X_XTAL_FREQ \>\> ( 25 -
SX126X_PLL_STEP_SHIFT_AMOUNT ) )

uint32_t sx126x_convert_freq_in_hz_to_pll_step( uint32_t freq_in_hz )

{

uint32_t steps_int;

uint32_t steps_frac;

steps_int = freq_in_hz / SX126X_PLL_STEP_SCALED;

steps_frac = freq_in_hz - ( steps_int \* SX126X_PLL_STEP_SCALED );

return ( steps_int \<\< SX126X_PLL_STEP_SHIFT_AMOUNT ) +

( ( ( steps_frac \<\< SX126X_PLL_STEP_SHIFT_AMOUNT ) + (
SX126X_PLL_STEP_SCALED \>\> 1 ) ) /

SX126X_PLL_STEP_SCALED );

}

***Matemáticamente**: pll_steps = (freq_hz × 2²⁵) / 32000000\
Resolución: **\~0.95 Hz por step***

**Uso típico:**

// Configurar a 868 MHz (Europa)

sx126x_set_rf_freq( context, 868000000 );

// Configurar a 915 MHz (América)

sx126x_set_rf_freq( context, 915000000 );

// Configurar a 490 MHz (Asia)

sx126x_set_rf_freq( context, 490000000 );

**Calibración de imagen por banda --- sx126x_cal_img_in_mhz()**

Después de cambiar frecuencia, **hay que calibrar la imagen** para esa
banda:

#define SX126X_IMAGE_CALIBRATION_STEP_IN_MHZ 4

sx126x_status_t sx126x_cal_img_in_mhz( const void\* context,

const uint16_t freq1_in_mhz,

const uint16_t freq2_in_mhz )

{

// floor() para freq1

const uint8_t freq1 = freq1_in_mhz /
SX126X_IMAGE_CALIBRATION_STEP_IN_MHZ;

// ceil() para freq2

const uint8_t freq2 = ( freq2_in_mhz +
SX126X_IMAGE_CALIBRATION_STEP_IN_MHZ - 1 )

/ SX126X_IMAGE_CALIBRATION_STEP_IN_MHZ;

return sx126x_cal_img( context, freq1, freq2 );

}

sx126x_status_t sx126x_cal_img( const void\* context, const uint8_t
freq1, const uint8_t freq2 )

{

const uint8_t buf\[SX126X_SIZE_CALIBRATE_IMAGE\] = {

SX126X_CALIBRATE_IMAGE, // Opcode 0x98

freq1, // Límite inferior

freq2, // Límite superior

};

return ( sx126x_status_t ) sx126x_hal_write( context, buf,
SX126X_SIZE_CALIBRATE_IMAGE, 0, 0 );

}

**Uso típico:**

// Calibrar para banda 868 MHz

sx126x_cal_img_in_mhz( context, 863, 870 );

// Calibrar para banda 915 MHz

sx126x_cal_img_in_mhz( context, 902, 928 );

**2. CONFIGURACIÓN DE SF, BW, CR (Modulación LoRa)**

**Las enumeraciones --- archivo sx126x.h**

// ═══════════ SPREADING FACTOR ═══════════

typedef enum sx126x_lora_sf_e

{

SX126X_LORA_SF5 = 0x05, // Más rápido, menor alcance

SX126X_LORA_SF6 = 0x06,

SX126X_LORA_SF7 = 0x07, // ← Balance típico

SX126X_LORA_SF8 = 0x08,

SX126X_LORA_SF9 = 0x09,

SX126X_LORA_SF10 = 0x0A,

SX126X_LORA_SF11 = 0x0B,

SX126X_LORA_SF12 = 0x0C, // Más lento, máximo alcance

} sx126x_lora_sf_t;

// ═══════════ BANDWIDTH ═══════════

typedef enum sx126x_lora_bw_e

{

SX126X_LORA_BW_500 = 6, // 500.0 kHz → máxima velocidad

SX126X_LORA_BW_250 = 5, // 250.0 kHz

SX126X_LORA_BW_125 = 4, // 125.0 kHz ← Default

SX126X_LORA_BW_062 = 3, // 62.5 kHz

SX126X_LORA_BW_041 = 10, // 41.67 kHz

SX126X_LORA_BW_031 = 2, // 31.25 kHz

SX126X_LORA_BW_020 = 9, // 20.83 kHz

SX126X_LORA_BW_015 = 1, // 15.63 kHz

SX126X_LORA_BW_010 = 8, // 10.42 kHz

SX126X_LORA_BW_007 = 0, // 7.81 kHz → máximo alcance

} sx126x_lora_bw_t;

// ═══════════ CODING RATE ═══════════

typedef enum sx126x_lora_cr_e

{

SX126X_LORA_CR_4_5 = 0x01, // 25% overhead ← Default

SX126X_LORA_CR_4_6 = 0x02, // 50% overhead

SX126X_LORA_CR_4_7 = 0x03, // 75% overhead

SX126X_LORA_CR_4_8 = 0x04, // 100% overhead (máxima protección)

} sx126x_lora_cr_t;

**La estructura que agrupa todo:**

typedef struct sx126x_mod_params_lora_s

{

sx126x_lora_sf_t sf; // Spreading Factor

sx126x_lora_bw_t bw; // Bandwidth

sx126x_lora_cr_t cr; // Coding Rate

uint8_t ldro; // Low Data Rate Optimization (0=off, 1=on)

} sx126x_mod_params_lora_t;

**La función que envía todo al chip
--- sx126x_set_lora_mod_params() en sx126x.c**

sx126x_status_t sx126x_set_lora_mod_params( const void\* context,

const sx126x_mod_params_lora_t\* params )

{

const uint8_t buf\[SX126X_SIZE_SET_MODULATION_PARAMS_LORA\] = {

SX126X_SET_MODULATION_PARAMS, // Opcode 0x8B

( uint8_t )( params-\>sf ), // Byte 1: SF (0x05-0x0C)

( uint8_t )( params-\>bw ), // Byte 2: BW (0-10)

( uint8_t )( params-\>cr ), // Byte 3: CR (0x01-0x04)

params-\>ldro & 0x01, // Byte 4: LDRO (0 ó 1)

};

sx126x_status_t status =

( sx126x_status_t ) sx126x_hal_write( context, buf,

SX126X_SIZE_SET_MODULATION_PARAMS_LORA, 0, 0 );

if( status == SX126X_STATUS_OK )

{

// WORKAROUND - Modulation Quality with 500 kHz LoRa Bandwidth

// ver datasheet DS_SX1261-2_V1.2 §15.1

status = sx126x_tx_modulation_workaround( context, SX126X_PKT_TYPE_LORA,
params-\>bw );

}

return status;

}

**El workaround que se ejecuta automáticamente:**

// Función interna (estática) en sx126x.c

static sx126x_status_t sx126x_tx_modulation_workaround( const void\*
context,

sx126x_pkt_type_t pkt_type,

sx126x_lora_bw_t bw )

{

uint8_t reg_value = 0;

sx126x_status_t status = sx126x_read_register( context,
SX126X_REG_TX_MODULATION, &reg_value, 1 );

// Registro 0x0889

if( status == SX126X_STATUS_OK )

{

if( pkt_type == SX126X_PKT_TYPE_LORA )

{

if( bw == SX126X_LORA_BW_500 )

reg_value &= \~( 1 \<\< 2 ); // Bit 2 = 0 para BW500

else

reg_value \|= ( 1 \<\< 2 ); // Bit 2 = 1 para otros BW

}

status = sx126x_write_register( context, SX126X_REG_TX_MODULATION,
&reg_value, 1 );

}

return status;

}

***¿Por qué?** El chip tiene un bug de fábrica: con BW=500kHz la calidad
de modulación se degrada si no se limpia el bit 2 del registro 0x0889.*

**La conversión BW enum → Hz:**

uint32_t sx126x_get_lora_bw_in_hz( sx126x_lora_bw_t bw )

{

uint32_t bw_in_hz = 0;

switch( bw )

{

case SX126X_LORA_BW_007: bw_in_hz = 7812UL; break;

case SX126X_LORA_BW_010: bw_in_hz = 10417UL; break;

case SX126X_LORA_BW_015: bw_in_hz = 15625UL; break;

case SX126X_LORA_BW_020: bw_in_hz = 20833UL; break;

case SX126X_LORA_BW_031: bw_in_hz = 31250UL; break;

case SX126X_LORA_BW_041: bw_in_hz = 41667UL; break;

case SX126X_LORA_BW_062: bw_in_hz = 62500UL; break;

case SX126X_LORA_BW_125: bw_in_hz = 125000UL; break;

case SX126X_LORA_BW_250: bw_in_hz = 250000UL; break;

case SX126X_LORA_BW_500: bw_in_hz = 500000UL; break;

}

return bw_in_hz;

}

**3. EJEMPLO COMPLETO: Secuencia de configuración para tu tesis**

Así quedaría la configuración completa de banda + SF + BW para un nodo
LoRa en 915 MHz:

// ═══ PASO 1: Configurar frecuencia (banda) ═══

sx126x_set_rf_freq( context, 915000000 ); // 915 MHz

sx126x_cal_img_in_mhz( context, 902, 928 ); // Calibrar imagen para
banda US

// ═══ PASO 2: Configurar modulación (SF + BW + CR) ═══

sx126x_mod_params_lora_t mod_params = {

.sf = SX126X_LORA_SF7, // SF7: balance velocidad/alcance

.bw = SX126X_LORA_BW_125, // 125 kHz: estándar LoRaWAN

.cr = SX126X_LORA_CR_4_5, // CR 4/5: mínimo overhead

.ldro = 0, // LDRO off (no necesario para SF7+BW125)

};

sx126x_set_lora_mod_params( context, &mod_params );

// ═══ PASO 3: Configurar potencia TX ═══

sx126x_pa_cfg_params_t pa_cfg = {

.pa_duty_cycle = 0x04, // Duty cycle estándar

.hp_max = 0x07, // Máxima potencia HP

.device_sel = 0x00, // 0x00 = SX1262

.pa_lut = 0x01, // Siempre 0x01

};

sx126x_set_pa_cfg( context, &pa_cfg );

sx126x_set_tx_params( context, 14, SX126X_RAMP_40_US ); // 14 dBm, rampa
40µs

**Trama SPI resultante (lo que viaja por el bus):**

Frecuencia: \[0x86\] \[0x39\] \[0x30\] \[0x00\] \[0x00\] ← 915 MHz en
PLL steps

Calibración: \[0x98\] \[0xE1\] \[0xE9\] ← 902-928 MHz / 4

Modulación: \[0x8B\] \[0x07\] \[0x04\] \[0x01\] \[0x00\] ← SF7, BW125,
CR4/5, LDRO=off

PA Config: \[0x95\] \[0x04\] \[0x07\] \[0x00\] \[0x01\] ← SX1262 full
power

TX Params: \[0x8E\] \[0x0E\] \[0x02\] ← 14dBm, ramp 40µs

Cada una de estas líneas es una transacción SPI que pasa por tu
función sx126x_hal_write(). ¿Quieres que ahora veamos cómo implementar
el HAL para el ESP32-S3 de tu placa WiFi LoRa 32?

dame todo esto en un archivo

Voy a crear un archivo completo y bien organizado con toda esta
información.

El documento contiene **9 partes**:

  -----------------------------------------------------------------------------
  **Parte**   **Contenido**
  ----------- -----------------------------------------------------------------
  **1**       Mapa completo de registros internos (direcciones, bits,
              funciones)

  **2**       Todos los opcodes SPI (37 comandos)

  **3**       Todas las enumeraciones (SF, BW, CR, IRQ, CAD, errores, etc.)

  **4**       Todas las estructuras de datos (12 structs)

  **5**       Constantes y macros (PLL, timeouts, cristal)

  **6**       Código fuente C completo de \~20 funciones clave

  **7**       Las 4 funciones HAL que debes implementar

  **8**       Secuencia completa de inicialización + TX + RX con ejemplo

  **9**       Tabla de tramas SPI reales por función
  -----------------------------------------------------------------------------

Puedes abrirlo con cualquier visor Markdown o directamente en VS Code
para tenerlo como referencia mientras implementas el HAL para tu
ESP32-S3.
