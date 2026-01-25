/*
    ata.h

    Benix ATA driver.
*/

#pragma once

#include "klib/types.h"

#define SECTOR_SIZE                 512

// ATA ports
#define ATA_CMD                     0x1f7
#define ATA_DATA                    0x1f0

#define ATA_ERROR                   0x1f1
#define ATA_SECTOR_COUNT            0x1f2
#define ATA_LBA_LOW                 0x1f3
#define ATA_LBA_MIDDLE              0x1f4
#define ATA_LBA_HIGH                0x1f5
#define ATA_DRIVE_HEAD              0x1f6
#define ATA_STATUS                  0x1f7
#define ATA_CONTROL                 0x3f6

#define ATA_BUSY                    0x80
#define ATA_DRIVE_READY             0x40
#define ATA_DEVICE_FAULT            0x20
#define ATA_DRIVE_SEEK_COMPLETE     0x10
#define ATA_DATA_REQUEST            0x08
#define ATA_CORRECTED_DATA          0x04
#define ATA_INDEX                   0x02
#define ATA_ERR                     0x01

#define ATA_CMD_IDENTIFY            0xec
#define ATA_CMD_READ                0x20
#define ATA_CMD_WRITE               0x30

struct DriverATA {
    void (*wait_busy)(void);
    void (*wait_data_request)(void);
    void (*reset)(void);
    void (*identify)(u16* buffer);
    void (*read)(u32 lba, u16* buffer);
    void (*write)(u32 lba, const u16* buffer);
};

void init_driver_ata(void);
struct DriverATA* get_driver_ata(void);