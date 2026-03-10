#ifndef LORA_H
#define LORA_H

#include <stdint.h>
#include <stdbool.h>

bool lora_init(void);
bool lora_send(const uint8_t *data, size_t len);
int  lora_receive(uint8_t *buf, size_t max_len);

#endif /* LORA_H */
