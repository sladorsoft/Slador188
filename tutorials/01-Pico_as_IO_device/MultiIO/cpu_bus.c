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
    while (pio_sm_get_rx_fifo_level(CONFIG_BUS_CTRL_PIO, CONFIG_BUS_CTRL_SM) > 0)
    {
        const uint32_t data = pio_sm_get_blocking(CONFIG_BUS_CTRL_PIO, CONFIG_BUS_CTRL_SM);
        const uint8_t addr = ((data >> 12) & 0x0000007f);
        const uint8_t val = ((data >> 2) & 0x000000ff);
        cpu_bus_write_io(addr, val);
    }

    pio_interrupt_clear(CONFIG_BUS_CTRL_PIO, 0);
}

void __time_critical_func(bus_read_handler)()
{
    while (pio_sm_get_rx_fifo_level(CONFIG_BUS_CTRL_PIO, CONFIG_BUS_CTRL_SM) > 0)
    {
        const uint8_t addr = ((pio_sm_get_blocking(CONFIG_BUS_CTRL_PIO, CONFIG_BUS_CTRL_SM) >> 2) & 0x0000007f);
        pio_sm_put_blocking(CONFIG_BUS_CTRL_PIO, CONFIG_BUS_CTRL_SM, cpu_bus_read_io(addr));
    }

    pio_interrupt_clear(CONFIG_BUS_CTRL_PIO, 1);
}

void cpu_bus_init()
{
    uint offset = pio_add_program(CONFIG_BUS_CTRL_PIO, &bus_ctrl_program);
    bus_ctrl_program_init(CONFIG_BUS_CTRL_PIO, CONFIG_BUS_CTRL_SM, offset);

    pio_set_irq0_source_enabled(CONFIG_BUS_CTRL_PIO, pis_interrupt0, true);
    irq_set_exclusive_handler(CONFIG_WRITE_IRQ, bus_write_handler);
    irq_set_priority(CONFIG_WRITE_IRQ, PICO_HIGHEST_IRQ_PRIORITY);
    irq_set_enabled(CONFIG_WRITE_IRQ, true);

    pio_set_irq1_source_enabled(CONFIG_BUS_CTRL_PIO, pis_interrupt1, true);
    irq_set_exclusive_handler(CONFIG_READ_IRQ, bus_read_handler);
    irq_set_priority(CONFIG_READ_IRQ, PICO_HIGHEST_IRQ_PRIORITY);
    irq_set_enabled(CONFIG_READ_IRQ, true);

    pio_sm_set_enabled(CONFIG_BUS_CTRL_PIO, CONFIG_BUS_CTRL_SM, true);
}
