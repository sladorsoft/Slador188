#include "DiskCtrlDevice.h"


DiskCtrlDevice::DiskCtrlDevice()
: OSTask{ "DiskController", Config::TASK_PRIORITY_NORMAL, 512 }, m_Card{ 0 }
{
    Reset();
}

uint8_t __time_critical_func(DiskCtrlDevice::ReadPort)(uint8_t addr)
{
    addr &= 0x07;
    if (addr == Ports::DataOffset)
    {
        if (!m_Ports.IsBusy())
        {
            if (m_DataReadOffset < std::min<uint16_t>(m_Ports.Regs.DataLen, m_Buffer.size()))
            {
                return m_Buffer[m_DataReadOffset++];
            }
        }

        return 0xff;
    }
    else
    {
        return m_Ports.Bytes[addr];
    }
}

void __time_critical_func(DiskCtrlDevice::WritePort)(uint8_t addr, uint8_t val)
{
    addr &= 0x07;
    if (!m_Ports.IsBusy())
    {
        if (addr == Ports::CommandOffset)
        {
            m_DataReadOffset = 0;
            m_Ports.Regs.Status = DiskCtrl::StatusBusy;
            BaseType_t higherPriorityTaskWoken = pdFALSE;
            xTaskNotifyFromISR(GetHandle(), val, eSetValueWithoutOverwrite, &higherPriorityTaskWoken);
            portYIELD_FROM_ISR(higherPriorityTaskWoken);
        }
        else if (addr == Ports::DataOffset)
        {
            if (m_Ports.Regs.DataLen < m_Buffer.size())
            {
                m_Buffer[m_Ports.Regs.DataLen++] = val;
            }
        }
        else if (addr < Ports::LBAOffset)
        {
            m_DataReadOffset = 0;
            m_Ports.Regs.DataLen = 0;
        }
        else
        {
            m_Ports.Bytes[addr] = val;
        }
    }
}

void DiskCtrlDevice::Run()
{
    while (true)
    {
        uint32_t cmd;
        xTaskNotifyWait(0, -1, &cmd, portMAX_DELAY);
        DiskCtrl::Status status = RunCommand(static_cast<Command>(cmd));
        m_Ports.Regs.Status = status & (DiskCtrl::StatusBusy ^ 0xff);
    }
}

DiskCtrl::Status DiskCtrlDevice::RunCommand(Command cmd)
{
    switch (cmd)
    {
        case Command::ReadSector:
            return ReadSectorCommand();

        case Command::WriteSector:
            return WriteSectorCommand();
    }

    return DiskCtrl::StatusUnknownCmd;
}

DiskCtrl::Status DiskCtrlDevice::ReadSectorCommand()
{
    DiskCtrl::Status status = m_Card.ReadSector(m_Ports.Regs.LBA, m_Buffer);
    m_Ports.Regs.DataLen = (status == DiskCtrl::StatusSuccess) ? m_Buffer.size() : 0;
    return status;
}

DiskCtrl::Status DiskCtrlDevice::WriteSectorCommand()
{
    m_Ports.Regs.DataLen = 0;
    return m_Card.WriteSector(m_Ports.Regs.LBA, m_Buffer);
}

void DiskCtrlDevice::Reset()
{
    m_Ports.Regs = Ports::Registers{};
    m_DataReadOffset = 0;
}
