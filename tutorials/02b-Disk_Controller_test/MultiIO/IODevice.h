#pragma once

#include <cstdint>


/**
 * @brief I/O device interface
 * 
 * Represents an I/O device implemented by MultiIO
 * which can be accessed by the host by a range of I/O ports
 */
class IODevice
{
public:
    virtual ~IODevice() = default;
    
    /**
     * @brief Reads a value from the I/O port
     * 
     * @param addr I/O port of the device to read from
     * @return uint8_t Value read from the I/O port
     */
    virtual uint8_t ReadPort(uint8_t addr) = 0;

    /**
     * @brief Writes a value to the I/O port
     * 
     * @param addr I/O port of the device to write to
     * @param val Value to write to the I/O port
     */
    virtual void WritePort(uint8_t addr, uint8_t val) = 0;
};
