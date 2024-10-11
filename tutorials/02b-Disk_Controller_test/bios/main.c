#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "text_lcd.h"
#include "system.h"


extern uint8_t disk_buffer[];


void lcd_init()
{
    text_lcd_init();
    text_lcd_function_set();
    text_lcd_display_ctrl(false, false, false);
    text_lcd_clear_display();
    text_lcd_return_home();
    text_lcd_entry_mode_set(true, false);
    text_lcd_display_ctrl(true, false, false);
}

void main()
{
    int bufferOffset = 0;
    for (int y = 0; y < 4; y++)
    {
        char buff[21];
        for (int x = 0; x < 20; x += 2)
        {
            byte_to_hex_str(buff + x, disk_buffer[bufferOffset++]);
        }

        buff[20] = 0;
        text_lcd_send_text_at(0, y, buff);
    }
}
