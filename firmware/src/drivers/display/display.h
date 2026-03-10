#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

bool display_init(void);
void display_clear(void);
void display_print(uint8_t col, uint8_t row, const char *text);
void display_update(void);

#endif /* DISPLAY_H */
