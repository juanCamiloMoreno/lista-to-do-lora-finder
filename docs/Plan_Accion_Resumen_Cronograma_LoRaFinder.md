# Plan de Acción — Versión 2.0 (Actualizado)

## casos
                                    Caso 0 — Operación normal (Heartbeat OK cada 5 min)                               
                                                                                                                      
         ┌─┐                                                                                                   ┌─┐    
         ║"│                                                                                                   ║"│    
         └┬┘                                                                                                   └┬┘    
         ┌┼┐                                                                                                   ┌┼┐    
          │               ┌────────────────┐           ┌────────────┐           ┌───────────────┐               │     
         ┌┴┐              │Nodo A (Maestro)│           │LoRa (canal)│           │Nodo B (Remoto)│              ┌┴┐    
      Usuario A           └────────┬───────┘           └──────┬─────┘           └───────┬───────┘           Usuario B 
          │                        │                          │                         │                       │     
          │     ╔═══════╤══════════╪══════════════════════════╪═════════════════════════╪═════════════════╗     │     
          │     ║ LOOP  │  Cada 5 minutos                     │                         │                 ║     │     
          │     ╟───────┘          │                          │                         │                 ║     │     
          │     ║                  │      HEARTBEAT_OK        │                         │                 ║     │     
          │     ║                  │─────────────────────────>│                         │                 ║     │     
          │     ║                  │                          │                         │                 ║     │     
          │     ║                  │                          │      HEARTBEAT_OK       │                 ║     │     
          │     ║                  │                          │────────────────────────>│                 ║     │     
          │     ║                  │                          │                         │                 ║     │     
          │     ║                  │                          │         ACK_OK          │                 ║     │     
          │     ║                  │                          │<────────────────────────│                 ║     │     
          │     ║                  │                          │                         │                 ║     │     
          │     ║                  │         ACK_OK           │                         │                 ║     │     
          │     ║                  │<─────────────────────────│                         │                 ║     │     
          │     ║                  │                          │                         │                 ║     │     
          │     ║                  │       ACK_CONFIRM        │                         │                 ║     │     
          │     ║                  │─────────────────────────>│                         │                 ║     │     
          │     ║                  │                          │                         │                 ║     │     
          │     ║                  │                          │      ACK_CONFIRM        │                 ║     │     
          │     ║                  │                          │────────────────────────>│                 ║     │     
          │     ╚══════════════════╪══════════════════════════╪═════════════════════════╪═════════════════╝     │     
      Usuario A           ┌────────┴───────┐           ┌──────┴─────┐           ┌───────┴───────┐           Usuario B 
         ┌─┐              │Nodo A (Maestro)│           │LoRa (canal)│           │Nodo B (Remoto)│              ┌─┐    
         ║"│              └────────────────┘           └────────────┘           └───────────────┘              ║"│    
         └┬┘                                                                                                   └┬┘    
         ┌┼┐                                                                                                   ┌┼┐    
          │                                                                                                     │     
         ┌┴┐                                                                                                   ┌┴┐    

## Sistema de Búsqueda LoRa — Trabajo de Grado

---

**Estudiante:** Juan Camilo Moreno Zornosa  
**Director:** Ing. Gustavo Adolfo Ramírez Espinosa, PhD  
**Período:** Febrero — Mayo 2026 (18 semanas)  
**Dedicación:** 6 créditos (~18 horas/semana)

---

## Cambio Metodológico v2.0

### Enfoque Anterior vs Nuevo Enfoque

```
(MAL) ENFOQUE ANTERIOR (v1.0):
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Diseño    │    │   Código    │    │  Pruebas    │    │   Tesis     │
│  Protocolo  │───►│   LoRa      │───►│   Campo     │───►│             │
│             │    │             │    │             │    │             │
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘

NUEVO ENFOQUE (v2.0):
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│  Casos de   │    │  Revisión   │    │   Diseño    │    │   Código    │    │  Pruebas    │
│    Uso      │───►│  Director   │───►│  Técnico    │───►│             │───►│  + Tesis    │
│             │    │             │    │             │    │             │    │             │
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘
   Semana 1-2        Semana 2          Semana 3-4        Semana 5-8         Semana 9-18
```

### Justificación del Cambio

| Aspecto | Beneficio |
|---------|-----------|
| **Validación temprana** | Los casos de uso se revisan ANTES de escribir código |
| **Menos retrabajo** | Se identifican escenarios de falla desde el inicio |
| **Mejor documentación** | El documento de tesis tiene base sólida de requisitos |
| **Comunicación clara** | Director y estudiante alineados en el alcance |

---

## 1. Resumen de Fases (Actualizado)

| Fase | Mes | Semanas | Objetivo Principal | Entregable Clave |
|------|-----|---------|-------------------|------------------|
| **Casos de Uso y Diseño** | Febrero | 1-4 | Casos de uso + Revisión + Protocolo | Documento aprobado |
| **Implementación** | Marzo | 5-8 | Código completo del firmware | Prototipo funcional |
| **Validación** | Abril | 9-13 | Pruebas de campo + Métricas | Informe de resultados |
| **Documentación** | Mayo | 14-18 | Redacción + Sustentación | Tesis aprobada |

---

## 2. Cronograma Gantt Actualizado (18 Semanas)

```
2026            FEBRERO              MARZO              ABRIL              MAYO
Semana    1   2   3   4      5   6   7   8      9  10  11  12  13     14  15  16  17  18
          │   │   │   │      │   │   │   │      │   │   │   │   │      │   │   │   │   │
          ▼   ▼   ▼   ▼      ▼   ▼   ▼   ▼      ▼   ▼   ▼   ▼   ▼      ▼   ▼   ▼   ▼   ▼
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│ FASE 1: CASOS DE USO Y DISEÑO                                                           │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│ Casos de uso principales  ████████                                                      │
│ Escenarios de falla           ████████                                                  │
│ 📋 REVISIÓN CON DIRECTOR          ██                                                    │
│ Diseño protocolo P2P              ████████                                              │
│ Diseño máquina estados                ████████                                          │
│                                           │                                             │
│  HITO 1: Casos de uso aprobados ────────┘                                             │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│ FASE 2: IMPLEMENTACIÓN                                                                  │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│ Código LoRa TX/RX básico                  ████████                                      │
│ Integración GPS+LoRa                          ████████                                  │
│ Máquina de estados                                ████████                              │
│ Integración completa                                  ████████                          │
│                                                           │                             │
│  HITO 2: Prototipo funcional ───────────────────────────┘                             │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│ FASE 3: VALIDACIÓN                                                                      │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│ Pruebas laboratorio                                       ████████                      │
│ Pruebas campo semi-rural                                      ████████                  │
│ Pruebas campo urbano                                              ████████              │
│ Análisis de resultados                                                ████████          │
│                                                                           │             │
│  HITO 3: Validación completa ───────────────────────────────────────────┘             │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│ FASE 4: DOCUMENTACIÓN                                                                   │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│ Redacción capítulos                                                       ████████████ │
│ Revisión director                                                               ██████ │
│ Sustentación                                                                       ████│
│                                                                                      │ │
│  HITO 4: Tesis aprobada ───────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────────────────────────┘

LEYENDA:
████  Actividad programada
📋    Punto de revisión con director
🎯    Hito principal
```

---

## 3. Detalle de Actividades por Semana

### Fase 1: Casos de Uso y Diseño (Febrero — Semanas 1-4)

#### Semana 1: Casos de Uso Principales

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Definir actores del sistema | Diagrama de actores | 4 |
| Documentar CU-01 a CU-07 (casos principales) | Fichas de casos de uso | 8 |
| Crear diagramas de secuencia | Diagramas UML | 4 |
| Diseño inicial de pantallas OLED | Mockups | 2 |
| **Total Semana 1** | | **18** |

#### Semana 2: Escenarios de Falla + Revisión

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Documentar FA-01 a FA-06 (escenarios de falla) | Fichas de escenarios | 6 |
| Crear matriz de respuestas del sistema | Tabla de respuestas | 4 |
| Derivar requisitos funcionales y no funcionales | Lista de requisitos | 3 |
| **📋 REVISIÓN CON DIRECTOR** | Documento revisado | 3 |
| Incorporar feedback del director | Documento actualizado | 2 |
| **Total Semana 2** | | **18** |

#### Semana 3: Diseño del Protocolo P2P

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Definir estructura del paquete (header + payload) | Diagrama de paquete | 4 |
| Definir tipos de mensaje (ALERT, ACK, LOCATION, etc.) | Tabla de mensajes | 4 |
| Diseñar codificación binaria del payload GPS | Especificación de encoding | 4 |
| Definir parámetros LoRa (SF, BW, CR) | Tabla de configuración | 3 |
| Calcular Time on Air (ToA) | Análisis de rendimiento | 3 |
| **Total Semana 3** | | **18** |

#### Semana 4: Máquina de Estados + Cierre Fase 1

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Diseñar máquina de estados completa | Diagrama de estados UML | 6 |
| Definir tabla de transiciones | Tabla de transiciones | 4 |
| Diseñar manejo de errores y timeouts | Especificación de errores | 4 |
| Consolidar documento de especificación | Documento final fase 1 | 4 |
| **Total Semana 4** | | **18** |

**🎯 HITO 1 (Fin de Semana 4):** Documento de Casos de Uso y Protocolo aprobado

---

### Fase 2: Implementación (Marzo — Semanas 5-8)

#### Semana 5: Código LoRa Básico

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Configurar proyecto PlatformIO para Heltec V4 | platformio.ini | 2 |
| Implementar código LoRa TX básico | lora_tx.cpp | 6 |
| Implementar código LoRa RX básico | lora_rx.cpp | 6 |
| Prueba ping-pong entre dispositivos | Log de pruebas | 4 |
| **Total Semana 5** | | **18** |

#### Semana 6: Integración GPS + LoRa

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Integrar lectura GPS existente | gps_handler.cpp | 4 |
| Implementar codificación binaria de coordenadas | location_codec.cpp | 4 |
| Transmitir coordenadas GPS vía LoRa | Código integrado | 6 |
| Pruebas de transmisión de ubicación | Log de pruebas | 4 |
| **Total Semana 6** | | **18** |

#### Semana 7: Máquina de Estados

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Implementar estados (STANDBY, SEEKING, SENDING) | state_machine.cpp | 8 |
| Implementar transiciones y eventos | Código de transiciones | 6 |
| Implementar manejo de botón (clic, doble clic) | button_handler.cpp | 4 |
| **Total Semana 7** | | **18** |

#### Semana 8: Integración Completa

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Integrar OLED con estados y mensajes | display_handler.cpp | 4 |
| Integrar brújula para dirección | compass_handler.cpp | 4 |
| Implementar cálculo de distancia (Haversine) | distance.cpp | 3 |
| Pruebas de integración en laboratorio | Log de pruebas | 4 |
| Depuración y corrección de errores | Código corregido | 3 |
| **Total Semana 8** | | **18** |

**🎯 HITO 2 (Fin de Semana 8):** Prototipo funcional completo

---

### Fase 3: Validación (Abril — Semanas 9-13)

#### Semana 9: Preparación y Pruebas Laboratorio

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Diseñar protocolo de pruebas | Documento de pruebas | 4 |
| Preparar equipos (carcasas, baterías) | Dispositivos listos | 4 |
| Pruebas de todos los casos de uso en lab | Checklist completado | 6 |
| Verificar escenarios de falla | Log de verificación | 4 |
| **Total Semana 9** | | **18** |

#### Semana 10: Pruebas Campo Semi-Rural

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Pruebas de alcance máximo (línea de vista) | Datos de alcance | 6 |
| Caracterización SF7 vs SF12 | Tabla comparativa SF | 6 |
| Medición RSSI y SNR vs distancia | Gráficas de señal | 6 |
| **Total Semana 10** | | **18** |

#### Semana 11: Pruebas Campo Urbano

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Pruebas en ambiente urbano denso | Datos de alcance urbano | 6 |
| Medición de PDR (Packet Delivery Ratio) | Estadísticas PDR | 6 |
| Pruebas con obstáculos (edificios, multitud) | Log de condiciones | 6 |
| **Total Semana 11** | | **18** |

#### Semana 12: Pruebas de Rendimiento

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Medición de precisión GPS en movimiento | Error de localización | 6 |
| Pruebas de autonomía (standby) | Duración en standby | 4 |
| Pruebas de autonomía (búsqueda activa) | Duración en búsqueda | 4 |
| Pruebas de casos extremos | Log de extremos | 4 |
| **Total Semana 12** | | **18** |

#### Semana 13: Análisis y Optimización

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Análisis estadístico de datos | Tablas y gráficas | 8 |
| Optimización de configuración según resultados | Config. optimizada | 4 |
| Redacción de informe técnico de resultados | Informe de validación | 6 |
| **Total Semana 13** | | **18** |

**🎯 HITO 3 (Fin de Semana 13):** Validación completa con datos de campo

---

### Fase 4: Documentación (Mayo — Semanas 14-18)

#### Semana 14: Capítulos de Metodología y Diseño

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Redacción Cap. 3: Metodología | Borrador Cap. 3 | 9 |
| Redacción Cap. 4: Diseño del Sistema | Borrador Cap. 4 | 9 |
| **Total Semana 14** | | **18** |

#### Semana 15: Capítulo de Implementación

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Redacción Cap. 5: Implementación | Borrador Cap. 5 | 10 |
| Documentación del código (README, comentarios) | Código documentado | 4 |
| Generación de figuras y diagramas | Material gráfico | 4 |
| **Total Semana 15** | | **18** |

#### Semana 16: Capítulos de Resultados y Conclusiones

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Redacción Cap. 6: Resultados | Borrador Cap. 6 | 10 |
| Redacción Cap. 7: Conclusiones | Borrador Cap. 7 | 6 |
| Generación de tablas de resultados | Tablas finales | 2 |
| **Total Semana 16** | | **18** |

#### Semana 17: Revisión y Preparación

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| **📋 REVISIÓN INTEGRAL CON DIRECTOR** | Feedback recibido | 6 |
| Incorporar correcciones | Documento corregido | 6 |
| Preparar presentación de sustentación | PowerPoint | 6 |
| **Total Semana 17** | | **18** |

#### Semana 18: Entrega y Sustentación

| Actividad | Entregable | Horas |
|-----------|------------|:-----:|
| Ajustes finales al documento | Documento final | 6 |
| Ensayo de sustentación | — | 4 |
| **ENTREGA DOCUMENTO FINAL** | Tesis entregada | 2 |
| **SUSTENTACIÓN** | ✅ Aprobación | 6 |
| **Total Semana 18** | | **18** |

**🎯 HITO 4 (Fin de Semana 18):** Tesis aprobada y sustentación exitosa

---

## 4. Hitos y Puntos de Control

| # | Hito | Semana | Fecha Estimada | Criterio de Éxito |
|:-:|------|:------:|----------------|-------------------|
| 1 | Casos de Uso Aprobados | 4 | Fin Febrero | Documento revisado y aprobado por director |
| 2 | Prototipo Funcional | 8 | Fin Marzo | Sistema funcionando en laboratorio |
| 3 | Validación Completa | 13 | Fin Abril | Métricas cumplidas (PDR>80%, 300m+) |
| 4 | Tesis Aprobada | 18 | Fin Mayo | Sustentación exitosa |

### Puntos de Revisión con Director

| Semana | Tipo | Objetivo |
|:------:|------|----------|
| 2 | 📋 Revisión | Aprobar casos de uso y escenarios de falla |
| 4 | 📋 Revisión | Aprobar diseño de protocolo y máquina de estados |
| 8 | 📋 Demo | Mostrar prototipo funcional |
| 13 | 📋 Revisión | Presentar resultados de validación |
| 17 | 📋 Revisión | Revisar documento de tesis completo |

---

## 5. Métricas de Éxito

| Métrica | Mínimo | Deseado | Método |
|---------|:------:|:-------:|--------|
| Alcance efectivo (urbano) | 300 m | 500 m | GPS + conteo paquetes |
| PDR (Packet Delivery Ratio) | 80% | 95% | RX/TX × 100 |
| Tiempo de respuesta | 30 s | 15 s | Timestamp TX→ACK |
| Precisión GPS (CEP50) | 10 m | 5 m | vs GPS referencia |
| Autonomía standby | 4 h | 8 h | Medición directa |
| Autonomía búsqueda | 3 h | 4 h | Medición directa |

---

## 6. Estado Actual del Proyecto

### Completado ✅

| Componente | Estado | Notas |
|------------|:------:|-------|
| Hardware Heltec V4 (×2) | ✅ | Adquirido |
| Módulos GPS | ✅ | Adquirido |
| Brújula GY-271 | ✅ | Adquirido |
| Código GPS (lectura) | ✅ | PlatformIO |
| Código Brújula | ✅ | PlatformIO |
| Código OLED | ✅ | PlatformIO |

### En Progreso 🔄

| Componente | Estado | Semana |
|------------|:------:|:------:|
| Documento de Casos de Uso v2 | 🔄 | 1-2 |
| Catálogo exhaustivo de escenarios | 🔄 | 1-2 |

### Pendiente ⬜

| Componente | Estado | Semana |
|------------|:------:|:------:|
| Revisión con director | ⬜ | 2 |
| Diseño protocolo P2P | ⬜ | 3 |
| Máquina de estados (diseño) | ⬜ | 4 |
| Código LoRa TX/RX | ⬜ | 5 |
| Integración completa | ⬜ | 5-8 |
| Pruebas de campo | ⬜ | 9-13 |
| Documento de tesis | ⬜ | 14-18 |

---

## 7. Distribución de Horas

| Fase | Semanas | Horas/Sem | Total | % |
|------|:-------:|:---------:|:-----:|:-:|
| Casos de Uso y Diseño | 4 | 18 | 72 | 22% |
| Implementación | 4 | 18 | 72 | 22% |
| Validación | 5 | 18 | 90 | 28% |
| Documentación | 5 | 18 | 90 | 28% |
| **TOTAL** | **18** | **18** | **324** | **100%** |

```
Casos de Uso+Diseño █████████████████████ 22%
Implementación      █████████████████████ 22%
Validación          ███████████████████████████ 28%
Documentación       ███████████████████████████ 28%
```

---

## 8. Cronograma Visual Simplificado

```
         FEBRERO 2026           MARZO 2026           ABRIL 2026           MAYO 2026
    ┌───────────────────┐   ┌───────────────────┐   ┌───────────────────┐   ┌───────────────────┐
    │                   │   │                   │   │                   │   │                   │
    │  📋 CASOS DE USO  │   │  🔧 IMPLEMENTAR   │   │  🧪 VALIDAR       │   │  📝 DOCUMENTAR    │
    │                   │   │                   │   │                   │   │                   │
    │  • Casos de uso   │   │  • Código LoRa    │   │  • Pruebas lab    │   │  • Redactar       │
    │  • Escenarios     │   │  • Integración    │   │  • Pruebas campo  │   │    tesis          │
    │  • Revisión 📋    │   │  • Estados        │   │  • Métricas       │   │  • Presentación   │
    │  • Protocolo      │   │  • UI completa    │   │  • Optimizar      │   │  • Sustentar      │
    │                   │   │                   │   │                   │   │                   │
    └─────────┬─────────┘   └─────────┬─────────┘   └─────────┬─────────┘   └─────────┬─────────┘
              │                       │                       │                       │
              ▼                       ▼                       ▼                       ▼
         🎯 HITO 1              🎯 HITO 2              🎯 HITO 3              🎯 HITO 4
      Casos de Uso           Prototipo OK          Validación OK          Tesis OK
        Aprobados
```

---

## 9. Próximos Pasos Inmediatos (Semana Actual)

| # | Tarea | Prioridad | Estado |
|:-:|-------|:---------:|:------:|
| 1 | Finalizar documento de Casos de Uso v2 | 🔴 Alta | 🔄 |
| 2 | Completar escenarios de falla (FA-01 a FA-06) | 🔴 Alta | 🔄 |
| 3 | Crear matriz de respuestas del sistema | 🔴 Alta | ⬜ |
| 4 | Preparar preguntas para revisión con director | 🟡 Media | ⬜ |
| 5 | Agendar reunión de revisión (Semana 2) | 🟡 Media | ⬜ |

---

## 10. Herramientas y Recursos

| Herramienta | Uso | Estado |
|-------------|-----|:------:|
| **PlatformIO + VS Code** | Desarrollo de firmware | ✅ Configurado |
| **GitHub** | Control de versiones | ✅ Repositorio creado |
| **Overleaf** | Documento de tesis | ⬜ Director creará proyecto |
| **Draw.io / Mermaid** | Diagramas UML | ✅ Disponible |
| **Excel** | Análisis de datos | ✅ Disponible |

---

*Documento actualizado: Febrero 2026 | Versión: 2.0*  
*Cambio principal: Casos de Uso primero, código después*
