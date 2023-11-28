#include "cpu_bus.h"
#include "config.h"

#include <pico/sync.h>
#include <pico/stdlib.h>


int main()
{
    stdio_init_all();

    cpu_bus_init();

    while (true)
    {
        __wfi();
    }

    return 0;
}
