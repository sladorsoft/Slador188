#include "MultiIO.h"
#include "bus_ctrl.pio.h"

#include "MultiIODevice.h"
#include "DiskCtrlDevice.h"


MultiIO MultiIO::s_Instance;


void MultiIO::InitDevices()
{
    s_Instance.InstallDevice(1, std::make_unique<DiskCtrlDevice>());
    s_Instance.InstallDevice(0, std::make_unique<MultiIODevice>());
}

MultiIO::MultiIO()
{
    uint offset = pio_add_program(Config::BusCtrlPIO, &bus_ctrl_program);
    bus_ctrl_program_init(Config::BusCtrlPIO, Config::BusCtrlSM, offset);

    pio_set_irq0_source_enabled(Config::BusCtrlPIO, pis_interrupt0, true);
    irq_set_exclusive_handler(Config::BusCtrlWriteIRQ, WriteIO);
    irq_set_priority(Config::BusCtrlWriteIRQ, PICO_HIGHEST_IRQ_PRIORITY);
    irq_set_enabled(Config::BusCtrlWriteIRQ, true);

    pio_set_irq1_source_enabled(Config::BusCtrlPIO, pis_interrupt1, true);
    irq_set_exclusive_handler(Config::BusCtrlReadIRQ, ReadIO);
    irq_set_priority(Config::BusCtrlReadIRQ, PICO_HIGHEST_IRQ_PRIORITY);
    irq_set_enabled(Config::BusCtrlReadIRQ, true);

    pio_sm_set_enabled(Config::BusCtrlPIO, Config::BusCtrlSM, true);
}

void __time_critical_func(MultiIO::ReadIO)()
{
    while (pio_sm_get_rx_fifo_level(Config::BusCtrlPIO, Config::BusCtrlSM) > 0)
    {
        const uint8_t addr = ((pio_sm_get_blocking(Config::BusCtrlPIO, Config::BusCtrlSM) >> 2) & 0x0000007f);
        pio_sm_put_blocking(Config::BusCtrlPIO, Config::BusCtrlSM, s_Instance.OnReadIO(addr));
    }

    pio_interrupt_clear(Config::BusCtrlPIO, 1);
}

void __time_critical_func(MultiIO::WriteIO)()
{
    while (pio_sm_get_rx_fifo_level(Config::BusCtrlPIO, Config::BusCtrlSM) > 0)
    {
        const uint32_t data = pio_sm_get_blocking(Config::BusCtrlPIO, Config::BusCtrlSM);
        const uint8_t addr = ((data >> 12) & 0x0000007f);
        const uint8_t val = ((data >> 2) & 0x000000ff);
        s_Instance.OnWriteIO(addr, val);
    }

    pio_interrupt_clear(Config::BusCtrlPIO, 0);
}

uint8_t __time_critical_func(MultiIO::OnReadIO)(uint8_t addr)
{
    std::unique_ptr<IODevice>& device = m_Devices[addr >> AddrBitsPerEntry];
    if (device)
    {
        return device->ReadPort(addr & AddrOffsetMask);
    }

    return 0xff;
}

void __time_critical_func(MultiIO::OnWriteIO)(uint8_t addr, uint8_t val)
{
    std::unique_ptr<IODevice>& device = m_Devices[addr >> AddrBitsPerEntry];
    if (device)
    {
        device->WritePort(addr & AddrOffsetMask, val);
    }
}

void MultiIO::InstallDevice(size_t index, std::unique_ptr<IODevice>&& device)
{
    m_Devices[index] = std::move(device);
}
