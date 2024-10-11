#pragma once

#include "Config.h"
#include "IODevice.h"

#include <memory>
#include <array>


/**
 * @brief MultiIO singleton
 */
class MultiIO
{
public:
    /**
     * @brief Initialises all devices
     * 
     * Creates and initialises all the device implementations
     * and maps their I/O ports int the MultiIO range
     */
    static void InitDevices();

private:
    MultiIO();

    uint8_t OnReadIO(uint8_t addr);
    void OnWriteIO(uint8_t addr, uint8_t val);

    void InstallDevice(size_t index, std::unique_ptr<IODevice>&& device);

    static const int AddrSpaceBits = 7;
    static const int AddrBitsPerEntry = 4;
    static const uint8_t AddrOffsetMask = (1 << AddrBitsPerEntry) - 1;

    using DeviceArray = std::array<std::unique_ptr<IODevice>, (1 << (AddrSpaceBits - AddrBitsPerEntry))>;
    DeviceArray m_Devices;

    static void ReadIO();
    static void WriteIO();

    static MultiIO s_Instance;
};
