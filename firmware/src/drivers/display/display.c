#include "display.h"
#include "../../config/system_config.h"
#include "../../config/pins.h"

bool display_init(void)
{
    /* TODO: initialize SSD1306 OLED via I2C */
    return false;
}

void display_clear(void)
{
    /* TODO: clear display buffer */
}

void display_print(uint8_t col, uint8_t row, const char *text)
{
    /* TODO: draw text at (col, row) in the buffer */
    (void)col; (void)row; (void)text;
}

void display_update(void)
{
    /* TODO: flush buffer to display */
}
