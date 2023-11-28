#pragma once

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"


namespace config
{
    static const PIO BusCtrlPIO = pio0;
    static const uint BusCtrlSM = 0;

    static const uint WriteIRQ = PIO0_IRQ_0;
    static const uint ReadIRQ = PIO0_IRQ_1;

    static const uint LEDPin = 22;
    static const uint INTPin = 21;
};
