#pragma once

#include "Config.h"
#include "OSTask.h"
#include "IODevice.h"
#include "DiskCtrlStatus.h"

#include <array>


struct sd_card_t;

/**
 * @brief SD card implementation
 * 
 * C++ wrapper for the "no-OS-FatFS-SD-SDIO-SPI-RPi-Pico" library
 */
class SDCard
{
public:
    using SectorData = std::array<uint8_t, 512>;

    /**
     * @brief SDCard constructor
     * @param cardIdx SD card index to be used
     */
    SDCard(size_t cardIdx);

    /**
     * @brief Reads a sector from the SD card
     * @param sector Sector number to read from
     * @param data Buffer to store the sector data to
     * @return Status of the operation
     */
    DiskCtrl::Status ReadSector(uint32_t sector, SectorData& data);

    /**
     * @brief Writes a sector to the SD card
     * @param sector Sector number to write to
     * @param data Buffer containing the sector data
     * @return Status of the operation
     */
    DiskCtrl::Status WriteSector(uint32_t sector, const SectorData& data);

private:
    sd_card_t* const m_Card;
};
