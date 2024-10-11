#include "SDCard.h"

#include <sd_card.h>


namespace
{
    static spi_t SPIDevice =
    {
        .hw_inst = Config::SDCardSPI,
        .miso_gpio = PICO_DEFAULT_SPI_RX_PIN,
        .mosi_gpio = PICO_DEFAULT_SPI_TX_PIN,
        .sck_gpio = PICO_DEFAULT_SPI_SCK_PIN,
        .baud_rate = 10 * 1000 * 1000
    };

    static sd_spi_if_t SPIInterface =
    {
        .spi = &SPIDevice,
        .ss_gpio = PICO_DEFAULT_SPI_CSN_PIN
    };

    static sd_card_t SDCards[] =
    {
        {
            .type = SD_IF_SPI,
            .spi_if_p = &SPIInterface,
            .use_card_detect = false,
        }
    };


    static DiskCtrl::Status ConvertStatus(block_dev_err_t err)
    {
        if (err == SD_BLOCK_DEVICE_ERROR_NONE)
        {
            return DiskCtrl::StatusSuccess;
        }

        DiskCtrl::Status status = 0;
        if ((err & (SD_BLOCK_DEVICE_ERROR_WRITE_PROTECTED | SD_BLOCK_DEVICE_ERROR_ERASE | SD_BLOCK_DEVICE_ERROR_WRITE)) != 0)
        {
            status |= DiskCtrl::StatusReadWriteErr;
        }

        if ((err & SD_BLOCK_DEVICE_ERROR_CRC) != 0)
        {
            status |= DiskCtrl::StatusCRCErr;
        }

        if ((err & SD_BLOCK_DEVICE_ERROR_NONE) != 0)
        {
            status |= DiskCtrl::StatusSectorNotFound;
        }

        if ((err & (SD_BLOCK_DEVICE_ERROR_NO_DEVICE | SD_BLOCK_DEVICE_ERROR_NO_RESPONSE)) != 0)
        {
            status |= DiskCtrl::StatusDevMissing;
        }

        if ((err & SD_BLOCK_DEVICE_ERROR_PARAMETER) != 0)
        {
            status |= DiskCtrl::StatusInvalidParam;
        }

        if ((err & (SD_BLOCK_DEVICE_ERROR_UNSUPPORTED | SD_BLOCK_DEVICE_ERROR_WOULD_BLOCK |
                    SD_BLOCK_DEVICE_ERROR_NO_INIT | SD_BLOCK_DEVICE_ERROR_UNUSABLE)) != 0)
        {
            status |= DiskCtrl::StatusOtherErr;
        }

        return status;
    }
}

// "no-OS-FatFS-SD-SDIO-SPI-RPi-Pico" library needs these 2 functions
// to be implemented by us
extern "C" 
{
    /**
     * @brief Returns the number of available SD cards in the system
     * 
     * @return size_t The number of available SD cards in the system
     */
    size_t sd_get_num() { return count_of(SDCards); }

    /**
     * @brief Returns a pointer to the structure defining an SD card
     * 
     * @param idx The index of the SD card
     * @return sd_card_t* Pointer to the structure defining an SD card,
     * or nullptr if the index is out of bounds
     */
    sd_card_t* sd_get_by_num(size_t idx)
    {
        if (idx >= sd_get_num())
        {
            return nullptr;
        }

        return &SDCards[idx];
    }
}


SDCard::SDCard(size_t cardIdx)
: m_Card{ sd_get_by_num(cardIdx) }
{
    sd_init_driver();
    DSTATUS status = m_Card->init(m_Card);
    DBG_PRINT("SDCard::SDCard: Result: %d\n", status);
}

DiskCtrl::Status SDCard::ReadSector(uint32_t sector, SectorData& data)
{
    block_dev_err_t result = m_Card->read_blocks(m_Card, data.data(), sector, 1);
    DBG_PRINT("SDCard::ReadSector: Sector: %d. Result: %d\n", sector, result);
    return ConvertStatus(result);
}

DiskCtrl::Status SDCard::WriteSector(uint32_t sector, const SectorData& data)
{
    block_dev_err_t result = m_Card->write_blocks(m_Card, data.data(), sector, 1);
    DBG_PRINT("SDCard::WriteSector: Sector: %d. Result: %d\n", sector, result);
    return ConvertStatus(result);
}
