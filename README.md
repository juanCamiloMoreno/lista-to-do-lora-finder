# PLAN DE ACCIÓN — Trabajo de Grado

**Maestría en Ingeniería del Internet de las Cosas**  
*Sistema de búsqueda basado en LoRa donde la conectividad celular no esté disponible*

---

**Estudiante:** Juan Camilo Moreno Zornosa  
**Director:** Ing. Gustavo Adolfo Ramírez Espinosa, PhD  
**Universidad:** Pontificia Universidad Javeriana  
**Período:** Febrero — Mayo 2026 (18 semanas)  
**Dedicación:** 6 créditos (~18 horas/semana)

---

## 1. Resumen Ejecutivo

Este plan de acción detalla las actividades para el desarrollo de la tesis durante 4 meses, siguiendo la metodología CDIO. El plan considera los avances previos (códigos de GPS, brújula y OLED ya desarrollados) y establece un cronograma realista con hitos verificables.

---

## 2. Estado Actual del Proyecto

| Componente | Estado | Plataforma |
|------------|--------|------------|
| Código GPS (lectura coordenadas) | ✅ Completado | PlatformIO |
| Código Brújula Digital (GY-271) | ✅ Completado | PlatformIO |
| Código Pantalla OLED | ✅ Completado | PlatformIO |
| Hardware: 2x Heltec WiFi LoRa 32 V4 | ✅ Adquirido | ESP32-S3 |
| Módulos GPS + Antenas | ✅ Adquirido | — |
| **Código Transmisión LoRa P2P** | ⬜ Pendiente | — |
| **Integración de módulos** | ⬜ Pendiente | — |
| **Gestión de estados** | ⬜ Pendiente | — |
| **Pruebas de campo** | ⬜ Pendiente | — |

---

## 3. Cronograma Detallado por Mes

---

### 📅 MES 1: FEBRERO 2026 — Fase de Diseño Detallado

**Objetivo:** Completar el diseño técnico del sistema, definir el protocolo de comunicación P2P, y desarrollar el módulo de transmisión LoRa básico.

| Semana | Actividad | Entregable |
|--------|-----------|------------|
| **1** | Diseño del protocolo P2P: definición de Node IDs, estructura de mensajes, tipos de paquetes (ALERT, ACK, LOCATION, PING) | Documento de especificación del protocolo |
| **1** | Diseño de la estructura binaria del payload (codificación escalada de coordenadas GPS) | Diagrama de estructura del payload |
| **2** | Análisis de parámetros LoRa: SF, BW, CR y su impacto en ToA y alcance | Tabla comparativa de configuraciones |
| **2** | Desarrollo código LoRa TX/RX básico (sin integrar GPS aún) | Código LoRa funcional (ping-pong) |
| **3** | Pruebas de comunicación LoRa punto a punto en laboratorio | Registro de pruebas: RSSI, SNR |
| **3** | Ajuste de parámetros LoRa según resultados de pruebas | Configuración LoRa optimizada |
| **4** | Diseño del presupuesto de potencia (power budget) | Documento de análisis energético |
| **4** | Diseño de máquina de estados: Standby ↔ Búsqueda | Diagrama de estados UML |

> 🎯 **Hito Mes 1:** Comunicación LoRa P2P funcional entre los dos dispositivos Heltec V4 + Documento de diseño técnico completo

---

### 📅 MES 2: MARZO 2026 — Fase de Implementación

**Objetivo:** Integrar todos los módulos (GPS, brújula, OLED, LoRa) en un firmware unificado, implementar la lógica de estados y el mecanismo de activación remota.

| Semana | Actividad | Entregable |
|--------|-----------|------------|
| **5** | Integración módulo GPS con LoRa: transmisión de coordenadas | Código integrado GPS+LoRa |
| **5** | Implementación del payload binario optimizado | Funciones de encode/decode |
| **6** | Integración pantalla OLED: visualización de estados y distancia | Interfaz OLED funcional |
| **6** | Integración brújula digital para función de orientación | Código brújula integrado |
| **7** | Implementación máquina de estados (Standby/Búsqueda) | Firmware con gestión de estados |
| **7** | Implementación mecanismo de activación remota (mensaje clave) | Protocolo de activación funcional |
| **8** | Implementación lógica del botón (clic, doble clic) | Interfaz de usuario completa |
| **8** | Pruebas de integración en laboratorio | Registro de pruebas unitarias |

> 🎯 **Hito Mes 2:** Firmware integrado funcional en ambos dispositivos + Prueba de concepto del sistema completo

---

### 📅 MES 3: ABRIL 2026 — Fase de Operación y Validación

**Objetivo:** Realizar pruebas de campo en entornos urbanos y semi-rurales, validar métricas de desempeño (alcance, PDR, precisión), y optimizar configuración.

| Semana | Actividad | Entregable |
|--------|-----------|------------|
| **9** | Diseño del protocolo de pruebas: métricas, ubicaciones, condiciones | Protocolo de pruebas documentado |
| **9** | Preparación de equipos para campo: carcasas 3D, baterías | Dispositivos ensamblados |
| **10** | Pruebas de campo entorno semi-rural (línea base) | Datos: alcance máximo, RSSI, SNR |
| **10** | Caracterización de SF (SF7-SF12) vs alcance/ToA | Tabla de caracterización SF |
| **11** | Pruebas de campo entorno urbano denso (simulando evento) | Datos: alcance urbano, PDR |
| **11** | Medición de precisión GPS en movimiento | Datos: error de localización |
| **12** | Pruebas de autonomía: modo standby y modo búsqueda | Datos: duración de batería |
| **12** | Optimización de configuración según resultados | Configuración final validada |
| **13** | Pruebas adicionales y casos límite | Registro de casos extremos |
| **13** | Análisis estadístico de datos recolectados | Informe técnico de validación |

> 🎯 **Hito Mes 3:** Validación completa del sistema con datos de campo + Informe técnico de resultados

---

### 📅 MES 4: MAYO 2026 — Documentación y Sustentación

**Objetivo:** Completar la redacción del documento de tesis, preparar la sustentación y realizar ajustes finales al prototipo.

| Semana | Actividad | Entregable |
|--------|-----------|------------|
| **14** | Redacción capítulo de Metodología y Diseño | Capítulos 3-4 borrador |
| **14** | Generación de figuras, diagramas y tablas finales | Material gráfico completo |
| **15** | Redacción capítulo de Implementación | Capítulo 5 borrador |
| **15** | Documentación del código fuente (comentarios, README) | Repositorio documentado |
| **16** | Redacción capítulo de Resultados y Análisis | Capítulo 6 borrador |
| **16** | Redacción de Conclusiones y Trabajo Futuro | Capítulo 7 borrador |
| **17** | Revisión integral del documento con el director | Documento con correcciones |
| **17** | Preparación de presentación para sustentación | Presentación PowerPoint |
| **18** | Ajustes finales al documento y prototipo | Documento final |
| **18** | Ensayo de sustentación y entrega final | Sustentación exitosa |

> 🎯 **Hito Mes 4:** Documento de tesis aprobado + Sustentación completada

---

## 4. Métricas de Validación (Objetivo Específico 4)

| Métrica | Objetivo Mínimo | Objetivo Deseado | Método de Medición |
|---------|-----------------|------------------|-------------------|
| Alcance efectivo | 300 m (urbano) | 500 m (urbano) | GPS + conteo de paquetes |
| Tasa de entrega (PDR) | > 80% | > 95% | Paquetes RX / Paquetes TX |
| Tiempo de respuesta | < 30 s (cold start GPS) | < 15 s (hot start) | Timestamp TX → ACK |
| Precisión localización | < 10 m (CEP50) | < 5 m (CEP50) | Comparación GPS referencia |
| Autonomía standby | > 4 horas | > 8 horas | Medición directa |
| Autonomía búsqueda | > 3 horas | > 4 horas | Medición directa |

---

## 5. Riesgos Identificados y Mitigación

| Riesgo | Probabilidad | Estrategia de Mitigación |
|--------|--------------|--------------------------|
| Alcance insuficiente en entorno urbano | Media | Probar múltiples SF; considerar antena externa si es necesario |
| Consumo energético mayor al estimado | Media | Implementar deep sleep agresivo; optimizar ciclo de escucha |
| GPS con fix lento en condiciones adversas | Alta | Usar última ubicación conocida; implementar timeout con fallback |
| Interferencia LoRa en banda ISM | Baja | Implementar retransmisión con backoff; cambiar canal si es necesario |
| Retrasos en importación de componentes | N/A | ✅ Mitigado: hardware ya adquirido |

---

## 6. Próximos Pasos Inmediatos (Semana 1)

Para iniciar el proyecto en febrero:

1. **Diseñar el protocolo de comunicación P2P:** Definir los tipos de mensajes (ALERT, ACK, LOCATION, PING), el formato del header (Node ID origen, destino, tipo, secuencia) y la estructura del payload.

2. **Diseñar la codificación del payload GPS:** Implementar codificación escalada de coordenadas (ej: `lat × 1e6` en 4 bytes) para minimizar el tamaño del paquete y reducir el Time on Air.

3. **Implementar código LoRa básico:** Crear un sketch de prueba TX/RX simple (ping-pong) entre los dos Heltec V4 usando la librería RadioLib o la nativa de Heltec.

4. **Documentar configuración del entorno:** Crear README del proyecto en el repositorio con instrucciones de setup de PlatformIO, librerías requeridas y configuración de los boards.

---

## 7. Estructura de Proyecto Sugerida (PlatformIO)

```
lora-finder/
├── src/
│   ├── main.cpp              # Entry point, state machine
│   ├── lora_handler.cpp      # LoRa TX/RX, protocolo P2P
│   ├── gps_handler.cpp       # Lectura GPS, codificación
│   ├── display_handler.cpp   # Interfaz OLED
│   ├── compass_handler.cpp   # Brújula GY-271
│   ├── button_handler.cpp    # Lógica de botón
│   └── power_manager.cpp     # Gestión energética
├── include/
│   ├── config.h              # Parámetros LoRa, pines, IDs
│   └── protocol.h            # Definición de mensajes
├── platformio.ini
├── docs/                     # Documentación técnica
└── README.md
```

---

## 8. Diseño Propuesto del Protocolo P2P

### 8.1 Tipos de Mensajes

| Tipo | Código | Descripción | Payload |
|------|--------|-------------|---------|
| PING | 0x01 | Verificación de enlace | Ninguno |
| PONG | 0x02 | Respuesta a PING | RSSI recibido |
| ALERT | 0x10 | Solicitud de búsqueda | Ninguno |
| ACK | 0x11 | Confirmación de alerta | Ninguno |
| LOCATION | 0x20 | Envío de coordenadas | Lat, Lon, Heading |

### 8.2 Estructura del Paquete (Propuesta)

```
┌──────────┬──────────┬──────────┬──────────┬─────────────────┐
│ Src ID   │ Dst ID   │ Msg Type │ Seq Num  │ Payload         │
│ (1 byte) │ (1 byte) │ (1 byte) │ (1 byte) │ (0-12 bytes)    │
└──────────┴──────────┴──────────┴──────────┴─────────────────┘
```

### 8.3 Codificación GPS Optimizada (8 bytes total)

```
Latitud:  int32_t lat_scaled = (int32_t)(latitude * 1e6)   → 4 bytes
Longitud: int32_t lon_scaled = (int32_t)(longitude * 1e6)  → 4 bytes
```

**Total payload LOCATION:** 4 (header) + 8 (GPS) = 12 bytes

Con SF7/BW125kHz: ToA ≈ 41 ms (muy eficiente)

---

## 9. Alineación con Objetivos del Anteproyecto

| Objetivo Específico | Mes | Semanas | Actividades Clave |
|---------------------|-----|---------|-------------------|
| **OE1:** Definir y seleccionar dispositivo | — | — | ✅ Ya completado (Heltec V4) |
| **OE2:** Diseñar arquitectura P2P resiliente | 1-2 | 1-8 | Protocolo, payload, máquina de estados |
| **OE3:** Desarrollar mecanismos de activación remota | 2 | 5-8 | Mensaje clave, secuencia ACK, firmware |
| **OE4:** Evaluar desempeño del sistema | 3 | 9-13 | Pruebas de campo, métricas, optimización |

---

## 10. Notas Importantes

- **Reuniones con el director:** Programar al menos cada 2 semanas
- **Revisión semanal:** Evaluar avance vs. plan y ajustar si es necesario
- **Documentación continua:** Mantener registro de decisiones técnicas
- **Control de versiones:** Usar Git desde el inicio del proyecto
- **Respaldos:** Mantener copias de seguridad del código y documentos

---

*Documento generado: Enero 2026*  
*Versión: 1.0*
