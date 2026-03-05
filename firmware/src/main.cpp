/**
 * main.cpp — Entry point del sistema LoRa Finder
 * 
 * Implementa la máquina de estados principal que coordina
 * todos los módulos del sistema: comunicación LoRa, GPS,
 * brújula, interfaz OLED y gestión energética.
 * 
 * @author  Juan Camilo Moreno Zornosa
 * @date    2026
 * @version 0.1.0
 */

#include <Arduino.h>
#include "config.h"

// ── Estados del sistema ─────────────────────────────────
enum SystemState {
    STATE_IDLE,         // Reposo / esperando acción
    STATE_ACTIVATING,   // Enviando o recibiendo comando de activación
    STATE_SEARCHING,    // Búsqueda activa (GPS + brújula + OLED)
    STATE_PROXIMITY,    // Alerta de proximidad al objetivo
    STATE_LINK_LOST,    // Enlace LoRa perdido
    STATE_LOW_BATTERY   // Batería baja, servicios reducidos
};

SystemState currentState = STATE_IDLE;

// ── Prototipos ──────────────────────────────────────────
void initHardware();
void handleIdleState();
void handleActivatingState();
void handleSearchingState();
void handleProximityState();
void handleLinkLostState();
void handleLowBatteryState();

// ── Setup ───────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("========================================");
    Serial.println("  LoRa Finder — Sistema SAR P2P");
    Serial.printf("  Node ID: 0x%02X | Peer: 0x%02X\n", NODE_ID, PEER_NODE_ID);
    Serial.println("========================================");
    
    initHardware();
    
    currentState = STATE_IDLE;
    Serial.println("[FSM] Estado inicial: IDLE");
}

// ── Loop principal (máquina de estados) ─────────────────
void loop() {
    switch (currentState) {
        case STATE_IDLE:
            handleIdleState();
            break;
        case STATE_ACTIVATING:
            handleActivatingState();
            break;
        case STATE_SEARCHING:
            handleSearchingState();
            break;
        case STATE_PROXIMITY:
            handleProximityState();
            break;
        case STATE_LINK_LOST:
            handleLinkLostState();
            break;
        case STATE_LOW_BATTERY:
            handleLowBatteryState();
            break;
    }
}

// ── Implementaciones placeholder ────────────────────────

void initHardware() {
    // TODO: Inicializar LoRa (SX1262)
    // TODO: Inicializar GPS (NEO-M8N por UART)
    // TODO: Inicializar brújula (QMC5883L por I2C)
    // TODO: Inicializar OLED (SSD1306)
    // TODO: Inicializar buzzer y motor de vibración
    // TODO: Inicializar lectura de batería (ADC)
    Serial.println("[INIT] Hardware inicializado (placeholder)");
}

void handleIdleState() {
    // TODO: Nodo A — esperar pulsación de botón para iniciar búsqueda
    // TODO: Nodo B — entrar en deep sleep con despertar periódico
    //                 escuchar comandos de activación durante ventana
}

void handleActivatingState() {
    // TODO: Nodo A — enviar comando ALERT, esperar ACK
    // TODO: Nodo B — recibir ALERT, enviar ACK + posición inicial
}

void handleSearchingState() {
    // TODO: Leer GPS → calcular distancia y bearing al peer
    // TODO: Leer brújula → calcular dirección relativa
    // TODO: Transmitir posición por LoRa
    // TODO: Recibir posición del peer
    // TODO: Actualizar OLED (flecha, distancia, RSSI, estado)
    // TODO: Verificar proximidad → transición a STATE_PROXIMITY
    // TODO: Verificar timeout enlace → transición a STATE_LINK_LOST
    // TODO: Verificar batería → transición a STATE_LOW_BATTERY
}

void handleProximityState() {
    // TODO: Aumentar frecuencia de transmisión
    // TODO: Activar buzzer y vibración
    // TODO: Mostrar alerta de proximidad en OLED
}

void handleLinkLostState() {
    // TODO: Mostrar última posición conocida
    // TODO: Incrementar SF para mejorar sensibilidad
    // TODO: Alerta periódica de vibración
    // TODO: Si se recupera enlace → volver a STATE_SEARCHING
}

void handleLowBatteryState() {
    // TODO: Reducir frecuencia de Tx
    // TODO: Apagar buzzer
    // TODO: En nivel crítico: apagar OLED, solo baliza LoRa mínima
}
