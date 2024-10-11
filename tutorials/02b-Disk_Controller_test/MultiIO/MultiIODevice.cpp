#include "MultiIODevice.h"


uint8_t __time_critical_func(MultiIODevice::ReadPort)(uint8_t addr)
{
    // Quick and dirty way of checking if MultiIO has started and is ready to operate.
    // Intel CPU writes random values to this port (offset 15) and then reads 
    // them back until the value read back has all bits negated
    if (addr == 15)
    {
        return m_ReadyCheck ^ 0xff;
    }

    return 0xff;
}

void __time_critical_func(MultiIODevice::WritePort)(uint8_t addr, uint8_t val)
{
    // Quick and dirty way of checking if MultiIO has started and is ready to operate.
    // Intel CPU writes random values to this port (offset 15) and then reads 
    // them back until the value read back has all bits negated
    if (addr == 15)
    {
        m_ReadyCheck = val;
    }
}
