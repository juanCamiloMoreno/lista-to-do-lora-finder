#pragma once
#include <stdint.h>
#include <stdbool.h>

/* Puerto UDP para ping-pong */
#define WIFI_UDP_PORT    4210
#define WIFI_UDP_SSID    "LF-UDP"
#define WIFI_UDP_PASS    "lorafinder"

typedef enum {
    WIFI_UDP_IDLE = 0,
    WIFI_UDP_AP,        /* Este nodo es AP + servidor UDP */
    WIFI_UDP_STA,       /* Este nodo es STA + cliente UDP */
} wifi_udp_role_t;

void          wifi_udp_start_ap(void);    /* Ser AP */
void          wifi_udp_start_sta(void);   /* Ser STA (conectar al AP) */
void          wifi_udp_stop(void);
void          wifi_udp_tick(void);        /* Llamar en loop */
bool          wifi_udp_connected(void);   /* STA conectado al AP */
void          wifi_udp_send_ping(void);   /* Enviar ping */

uint32_t      wifi_udp_tx(void);
uint32_t      wifi_udp_rx(void);
int16_t       wifi_udp_rssi(void);        /* WiFi RSSI de la conexion */
uint32_t      wifi_udp_rtt_last_ms(void);
uint32_t      wifi_udp_rtt_avg_ms(void);
wifi_udp_role_t wifi_udp_role(void);
