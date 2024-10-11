#pragma once

#include <cstdint>


namespace DiskCtrl
{
    /**
     * @brief Status of the Disk Controller operation reported to the host
     */
    using Status = uint8_t;

    constexpr Status StatusSuccess = 0;

    constexpr Status StatusReadWriteErr = (1 << 0);
    constexpr Status StatusCRCErr = (1 << 1);
    constexpr Status StatusSectorNotFound = (1 << 2);
    constexpr Status StatusDevMissing = (1 << 3);
    constexpr Status StatusInvalidParam = (1 << 4);
    constexpr Status StatusUnknownCmd = (1 << 5);
    constexpr Status StatusOtherErr = (1 << 6);
    constexpr Status StatusBusy = (1 << 7);
};
