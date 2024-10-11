#pragma once

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"


#define CONFIG_BUS_CTRL_PIO     pio0
#define CONFIG_BUS_CTRL_SM      0
#define CONFIG_WRITE_IRQ        PIO0_IRQ_0
#define CONFIG_READ_IRQ         PIO0_IRQ_1
#define CONFIG_LED_PIN          22
#define CONFIG_INT_PIN          21
