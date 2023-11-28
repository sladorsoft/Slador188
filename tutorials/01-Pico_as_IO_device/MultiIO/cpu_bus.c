#include "cpu_bus.h"
#include "config.h"

#include <pico/stdlib.h>
#include <hardware/pio.h>

#include "bus_ctrl.pio.h"


static uint16_t prevFibNo = 1;
static uint16_t currFibNo = 1;

uint8_t __time_critical_func(cpu_bus_read_io)(uint8_t addr)
{
    if (addr == 0x00)
    {
        const uint16_t tmp = prevFibNo;
        prevFibNo = currFibNo;
        currFibNo += tmp;
        return (currFibNo & 0xff);
    }
    else if (addr == 0x01)
    {
        return (currFibNo >> 8);
    }
    else if (addr == 0x04)
    {
        return 0x45;
    }
    else if (addr == 0x05)
    {
        return 0x23;
    }

    return 0xff;
}

void __time_critical_func(cpu_bus_write_io)(uint8_t addr, uint8_t val)
{
    // if port == 0, Fibonacci sequence reset
    if (addr == 0)
    {
        prevFibNo = 1;
        currFibNo = 1;
    }
}

void __time_critical_func(bus_write_handler)()
{
    while (pio_sm_get_rx_fifo_level(config::BusCtrlPIO, config::BusCtrlSM) > 0)
    {
        const uint32_t data = pio_sm_get_blocking(config::BusCtrlPIO, config::BusCtrlSM);
        const uint8_t addr = ((data >> 12) & 0x0000007f);
        const uint8_t val = ((data >> 2) & 0x000000ff);
        cpu_bus_write_io(addr, val);
    }

    pio_interrupt_clear(config::BusCtrlPIO, 0);
}

void __time_critical_func(bus_read_handler)()
{
    while (pio_sm_get_rx_fifo_level(config::BusCtrlPIO, config::BusCtrlSM) > 0)
    {
        const uint8_t addr = ((pio_sm_get_blocking(config::BusCtrlPIO, config::BusCtrlSM) >> 2) & 0x0000007f);
        pio_sm_put_blocking(config::BusCtrlPIO, config::BusCtrlSM, cpu_bus_read_io(addr));
    }

    pio_interrupt_clear(config::BusCtrlPIO, 1);
}

void cpu_bus_init()
{
    uint offset = pio_add_program(config::BusCtrlPIO, &bus_ctrl_program);
    bus_ctrl_program_init(config::BusCtrlPIO, config::BusCtrlSM, offset);

    pio_set_irq0_source_enabled(config::BusCtrlPIO, pis_interrupt0, true);
    irq_set_exclusive_handler(config::WriteIRQ, bus_write_handler);
    irq_set_priority(config::WriteIRQ, PICO_HIGHEST_IRQ_PRIORITY);
    irq_set_enabled(config::WriteIRQ, true);

    pio_set_irq1_source_enabled(config::BusCtrlPIO, pis_interrupt1, true);
    irq_set_exclusive_handler(config::ReadIRQ, bus_read_handler);
    irq_set_priority(config::ReadIRQ, PICO_HIGHEST_IRQ_PRIORITY);
    irq_set_enabled(config::ReadIRQ, true);

    pio_sm_set_enabled(config::BusCtrlPIO, config::BusCtrlSM, true);
}
