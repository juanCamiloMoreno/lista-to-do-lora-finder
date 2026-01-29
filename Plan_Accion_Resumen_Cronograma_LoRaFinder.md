# Plan de Acción — Resumen Ejecutivo

## Sistema de Búsqueda LoRa — Trabajo de Grado

**Estudiante:** Juan Camilo Moreno Zornosa  
**Director:** Ing. Gustavo Adolfo Ramírez Espinosa, PhD  
**Período:** Febrero — Mayo 2026 (18 semanas)  
**Dedicación:** 6 créditos (~18 horas/semana)

---

## 1. Resumen de Fases CDIO

| Fase | Mes | Semanas | Objetivo Principal | Entregable Clave |
|------|-----|---------|-------------------|------------------|
| **Diseño Detallado** | Febrero | 1-4 | Protocolo P2P + Código LoRa básico | Comunicación LoRa funcional |
| **Implementación** | Marzo | 5-8 | Integración de módulos + Firmware | Prototipo funcional completo |
| **Operación/Validación** | Abril | 9-13 | Pruebas de campo + Métricas | Informe técnico de resultados |
| **Documentación** | Mayo | 14-18 | Redacción + Sustentación | Tesis aprobada |

---

## 2. Cronograma Gantt (18 Semanas)

```
2026            FEBRERO           MARZO             ABRIL             MAYO
Semana    1   2   3   4     5   6   7   8     9  10  11  12  13    14  15  16  17  18
          │   │   │   │     │   │   │   │     │   │   │   │   │     │   │   │   │   │
          ▼   ▼   ▼   ▼     ▼   ▼   ▼   ▼     ▼   ▼   ▼   ▼   ▼     ▼   ▼   ▼   ▼   ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│ FASE 1: DISEÑO DETALLADO                                                            │
├─────────────────────────────────────────────────────────────────────────────────────┤
│ Protocolo P2P        ████████                                                       │
│ Análisis LoRa            ████████                                                   │
│ Código LoRa básico           ████████                                               │
│ Power Budget                     ████████                                           │
│                                       │                                             │
│ 🎯 HITO 1: LoRa P2P funcionando ──────┘                                             │
├─────────────────────────────────────────────────────────────────────────────────────┤
│ FASE 2: IMPLEMENTACIÓN                                                              │
├─────────────────────────────────────────────────────────────────────────────────────┤
│ Integración GPS+LoRa                  ████████                                      │
│ Integración OLED+Brújula                  ████████                                  │
│ Máquina de estados                            ████████                              │
│ Pruebas integración                               ████████                          │
│                                                       │                             │
│ 🎯 HITO 2: Prototipo funcional ───────────────────────┘                             │
├─────────────────────────────────────────────────────────────────────────────────────┤
│ FASE 3: OPERACIÓN Y VALIDACIÓN                                                      │
├─────────────────────────────────────────────────────────────────────────────────────┤
│ Protocolo pruebas                                     ████████                      │
│ Pruebas semi-rural                                        ████████                  │
│ Pruebas urbano                                                ████████              │
│ Autonomía + Optimización                                          ████████         │
│ Análisis estadístico                                                  ████████     │
│                                                                           │         │
│ 🎯 HITO 3: Validación completa ───────────────────────────────────────────┘         │
├─────────────────────────────────────────────────────────────────────────────────────┤
│ FASE 4: DOCUMENTACIÓN Y CIERRE                                                      │
├─────────────────────────────────────────────────────────────────────────────────────┤
│ Redacción Cap. 3-5                                                    ████████     │
│ Redacción Cap. 6-7                                                        ████████ │
│ Revisión director                                                             █████│
│ Sustentación                                                                    ███│
│                                                                                   │ │
│ 🎯 HITO 4: Tesis aprobada ────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

---

## 3. Tabla de Actividades por Semana

### Mes 1: Febrero 2026 — Diseño Detallado

| Sem | Actividad | Entregable | Hrs |
|:---:|-----------|------------|:---:|
| 1 | Diseño protocolo P2P (Node IDs, tipos de mensaje) | Documento de especificación | 18 |
| 1 | Diseño estructura payload GPS (codificación escalada) | Diagrama de payload | — |
| 2 | Análisis parámetros LoRa (SF, BW, CR vs ToA) | Tabla comparativa | 18 |
| 2 | Desarrollo código LoRa TX/RX básico (ping-pong) | Código funcional | — |
| 3 | Pruebas comunicación LoRa en laboratorio | Registro RSSI/SNR | 18 |
| 3 | Ajuste de parámetros según pruebas | Config. optimizada | — |
| 4 | Diseño power budget (consumo estimado) | Análisis energético | 18 |
| 4 | Diseño máquina de estados (UML) | Diagrama de estados | — |

**🎯 Hito Mes 1:** Comunicación LoRa P2P funcional entre 2 Heltec V4

---

### Mes 2: Marzo 2026 — Implementación

| Sem | Actividad | Entregable | Hrs |
|:---:|-----------|------------|:---:|
| 5 | Integración GPS + LoRa (transmisión coords) | Código GPS+LoRa | 18 |
| 5 | Implementación payload binario optimizado | Funciones encode/decode | — |
| 6 | Integración OLED (estados, distancia) | Interfaz OLED | 18 |
| 6 | Integración brújula GY-271 | Código brújula | — |
| 7 | Implementación máquina de estados | Firmware estados | 18 |
| 7 | Implementación activación remota | Protocolo activación | — |
| 8 | Implementación lógica botón (clic/doble) | Interfaz usuario | 18 |
| 8 | Pruebas integración laboratorio | Registro pruebas | — |

**🎯 Hito Mes 2:** Firmware integrado + Prueba de concepto funcional

---

### Mes 3: Abril 2026 — Validación

| Sem | Actividad | Entregable | Hrs |
|:---:|-----------|------------|:---:|
| 9 | Diseño protocolo de pruebas (métricas) | Protocolo documentado | 18 |
| 9 | Ensamble carcasas 3D + baterías | Dispositivos listos | — |
| 10 | Pruebas campo semi-rural (línea base) | Datos alcance/RSSI | 18 |
| 10 | Caracterización SF7-SF12 vs alcance | Tabla SF | — |
| 11 | Pruebas campo urbano denso | Datos PDR urbano | 18 |
| 11 | Medición precisión GPS en movimiento | Error localización | — |
| 12 | Pruebas autonomía (standby/búsqueda) | Duración batería | 18 |
| 12 | Optimización configuración final | Config. validada | — |
| 13 | Pruebas casos límite | Registro extremos | 18 |
| 13 | Análisis estadístico de datos | Informe técnico | — |

**🎯 Hito Mes 3:** Validación completa + Informe de resultados

---

### Mes 4: Mayo 2026 — Documentación

| Sem | Actividad | Entregable | Hrs |
|:---:|-----------|------------|:---:|
| 14 | Redacción Cap. Metodología y Diseño | Cap. 3-4 borrador | 18 |
| 14 | Generación figuras y tablas finales | Material gráfico | — |
| 15 | Redacción Cap. Implementación | Cap. 5 borrador | 18 |
| 15 | Documentación código (README) | Repositorio docs | — |
| 16 | Redacción Cap. Resultados | Cap. 6 borrador | 18 |
| 16 | Redacción Conclusiones | Cap. 7 borrador | — |
| 17 | Revisión con director | Doc. corregido | 18 |
| 17 | Preparación presentación | PowerPoint | — |
| 18 | Ajustes finales | Documento final | 18 |
| 18 | Sustentación | ✅ Aprobación | — |

**🎯 Hito Mes 4:** Tesis aprobada + Sustentación exitosa

---

## 4. Resumen de Hitos Principales

| # | Hito | Fecha Objetivo | Criterio de Éxito |
|:-:|------|----------------|-------------------|
| 1 | LoRa P2P funcionando | Fin Febrero | Ping-pong exitoso entre dispositivos |
| 2 | Prototipo funcional | Fin Marzo | Sistema completo operando en lab |
| 3 | Validación completa | Fin Abril | Métricas cumplidas (PDR>80%, 300m+) |
| 4 | Tesis aprobada | Fin Mayo | Sustentación exitosa |

---

## 5. Métricas de Éxito del Proyecto

| Métrica | Mínimo | Deseado | Método |
|---------|:------:|:-------:|--------|
| Alcance urbano | 300 m | 500 m | GPS + conteo paquetes |
| PDR (Packet Delivery Ratio) | 80% | 95% | RX/TX × 100 |
| Tiempo respuesta | 30 s | 15 s | Timestamp TX→ACK |
| Precisión GPS | 10 m | 5 m | vs GPS referencia |
| Autonomía standby | 4 h | 8 h | Medición directa |
| Autonomía búsqueda | 3 h | 4 h | Medición directa |

---

## 6. Estado Actual vs Plan

| Componente | Estado | Responsable | Semana |
|------------|:------:|-------------|:------:|
| ✅ Hardware Heltec V4 (×2) | Listo | — | — |
| ✅ Módulos GPS | Listo | — | — |
| ✅ Brújula GY-271 | Listo | — | — |
| ✅ Código GPS | Listo | — | — |
| ✅ Código Brújula | Listo | — | — |
| ✅ Código OLED | Listo | — | — |
| ⬜ Código LoRa P2P | Pendiente | Juan | 2-3 |
| ⬜ Integración firmware | Pendiente | Juan | 5-8 |
| ⬜ Carcasa 3D | Pendiente | Juan | 9 |
| ⬜ Pruebas campo | Pendiente | Juan | 10-13 |
| ⬜ Documento tesis | Pendiente | Juan | 14-18 |

---

## 7. Cronograma Visual Simplificado

```
         FEB 2026              MAR 2026              ABR 2026              MAY 2026
    ┌─────────────────┐   ┌─────────────────┐   ┌─────────────────┐   ┌─────────────────┐
    │                 │   │                 │   │                 │   │                 │
    │  📐 DISEÑO      │   │  🔧 IMPLEMENTAR │   │  🧪 VALIDAR     │   │  📝 DOCUMENTAR  │
    │                 │   │                 │   │                 │   │                 │
    │  • Protocolo    │   │  • Integrar     │   │  • Pruebas      │   │  • Redactar     │
    │  • LoRa básico  │   │    GPS+LoRa     │   │    campo        │   │    tesis        │
    │  • Power budget │   │  • Estados      │   │  • Métricas     │   │  • Presentación │
    │                 │   │  • UI botón     │   │  • Optimizar    │   │  • Sustentar    │
    │                 │   │                 │   │                 │   │                 │
    └────────┬────────┘   └────────┬────────┘   └────────┬────────┘   └────────┬────────┘
             │                     │                     │                     │
             ▼                     ▼                     ▼                     ▼
        🎯 HITO 1            🎯 HITO 2            🎯 HITO 3            🎯 HITO 4
       LoRa P2P OK         Prototipo OK        Validación OK        Tesis OK
```

---

## 8. Distribución de Horas (Total: 324 horas)

| Fase | Semanas | Horas/Sem | Total Hrs | % |
|------|:-------:|:---------:|:---------:|:-:|
| Diseño | 4 | 18 | 72 | 22% |
| Implementación | 4 | 18 | 72 | 22% |
| Validación | 5 | 18 | 90 | 28% |
| Documentación | 5 | 18 | 90 | 28% |
| **TOTAL** | **18** | **18** | **324** | **100%** |

```
  Diseño        ██████████████████████ 22%
  Implementación██████████████████████ 22%
  Validación    ████████████████████████████ 28%
  Documentación ████████████████████████████ 28%
```

---

## 9. Próximos Pasos (Semana 1)

| # | Tarea | Prioridad | Duración Est. |
|:-:|-------|:---------:|:-------------:|
| 1 | Diseñar tipos de mensaje del protocolo | 🔴 Alta | 4 h |
| 2 | Definir estructura del header (4 bytes) | 🔴 Alta | 2 h |
| 3 | Diseñar payload LOCATION (12 bytes) | 🔴 Alta | 3 h |
| 4 | Crear archivo `protocol.h` | 🟡 Media | 3 h |
| 5 | Documentar protocolo en Markdown | 🟡 Media | 4 h |
| 6 | Configurar repositorio Git | 🟢 Baja | 2 h |

---

## 10. Checklist Semanal

### ☐ Semana 1 (Febrero)
- [ ] Protocolo P2P documentado
- [ ] Estructura payload definida
- [ ] Archivo `protocol.h` creado
- [ ] Repositorio Git configurado

### ☐ Semana 2 (Febrero)
- [ ] Análisis SF vs ToA completado
- [ ] Código LoRa TX básico funcionando
- [ ] Código LoRa RX básico funcionando
- [ ] Primera transmisión exitosa

### ☐ Semana 3 (Febrero)
- [ ] Pruebas RSSI/SNR en lab
- [ ] Parámetros LoRa ajustados
- [ ] Ping-pong funcionando estable
- [ ] Documentación de pruebas

### ☐ Semana 4 (Febrero)
- [ ] Power budget calculado
- [ ] Máquina de estados diseñada (UML)
- [ ] Diagrama de arquitectura final
- [ ] **🎯 HITO 1 completado**

---

*Documento generado: Enero 2026 | Versión: 1.0*
