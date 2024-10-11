#pragma once

#include "Config.h"
#include "IODevice.h"


/**
 * @brief The main MultiIO device
 * 
 * Implements Multi I/O device functionality.
 * Visible to the host via a range of I/O ports.
 */
class MultiIODevice : public IODevice
{
public:
    uint8_t ReadPort(uint8_t addr) override;
    void WritePort(uint8_t addr, uint8_t val) override;

private:
    uint8_t m_ReadyCheck = 0xff;
};
