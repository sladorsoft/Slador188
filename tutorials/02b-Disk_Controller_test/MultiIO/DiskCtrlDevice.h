#pragma once

#include "Config.h"
#include "OSTask.h"
#include "IODevice.h"
#include "SDCard.h"
#include "DiskCtrlStatus.h"


/**
 * @brief Disk Controller device
 * 
 * Implements the disk controller functionality.
 * Visible to the host via a range of I/O ports.
 */
class DiskCtrlDevice : public OSTask, public IODevice
{
public:
    /**
     * @brief DiskCtrlDevice constructor
     */
    DiskCtrlDevice();

    uint8_t ReadPort(uint8_t addr) override;
    void WritePort(uint8_t addr, uint8_t val) override;

protected:
    void Run() override;

private:
    enum class Command : uint8_t
    {
        ReadSector = 'R',
        WriteSector = 'W'
    };

    DiskCtrl::Status RunCommand(Command cmd);
    DiskCtrl::Status ReadSectorCommand();
    DiskCtrl::Status WriteSectorCommand();

    void Reset();

    union Ports
    {
        Ports() : Regs{} {}

        constexpr bool IsBusy() const { return ((Regs.Status & DiskCtrl::StatusBusy) != 0); }

        uint8_t Bytes[8];

        struct Registers
        {
            Registers() : Status{ 0 }, DataLen{ 0 }, LBA { 0xffffffff } {}
            uint8_t Status;
            uint8_t DataNotUsed;
            uint16_t DataLen;
            uint32_t LBA;
        } __attribute__((packed)) Regs;

        static const uint8_t StatusOffset = offsetof(Registers, Status);
        static const uint8_t CommandOffset = StatusOffset;
        static const uint8_t DataOffset = offsetof(Registers, DataNotUsed);
        static const uint8_t DataLenOffset = offsetof(Registers, DataLen);
        static const uint8_t LBAOffset = offsetof(Registers, LBA);
    };

    static_assert(sizeof(Ports) == 8);

    SDCard m_Card;
    SDCard::SectorData m_Buffer;
    Ports m_Ports;
    uint16_t m_DataReadOffset = 0;
};
