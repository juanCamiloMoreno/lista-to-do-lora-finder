#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Utility macros */
#define ARRAY_SIZE(x)   (sizeof(x) / sizeof((x)[0]))
#define CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : ((v) > (hi) ? (hi) : (v)))

void     delay_ms(uint32_t ms);
uint32_t millis_now(void);

#endif /* COMMON_H */
