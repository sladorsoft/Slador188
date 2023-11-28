#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "text_lcd.h"
#include "system.h"


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
    uint32_t lastSysTick = 0;
    while (1)
    {
        char buff[8];
        utoa((uint16_t)(lastSysTick / 100), buff, 10);
        text_lcd_send_text_at(9, 1, buff);

        while (1)
        {
            uint32_t currSysTick = get_sys_ticks();
            if ((currSysTick - lastSysTick) >= 100)
            {
                lastSysTick = currSysTick;
                break;
            }
        }
    }
}
