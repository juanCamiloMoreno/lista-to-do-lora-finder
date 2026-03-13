#ifndef DIAG_H
#define DIAG_H

/*
 * diag — Diagnóstico completo del sistema por Serial.
 * Llama a diag_report() una vez desde app_init(), después de inicializar
 * todos los drivers.
 */
void diag_report(void);

#endif /* DIAG_H */
