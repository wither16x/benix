/*
    ata.c

    Implementations of the functions in ata.h.
*/

#include "drivers/ata.h"
#include "klib/asm.h"
#include "klib/logging.h"

static struct DriverATA ata;

static void wait_busy(void) {
    while (__asm_inb(ATA_STATUS) & ATA_BUSY);
}

static void wait_data_request(void) {
    int timeout = 1000000;
    while (!( __asm_inb(ATA_STATUS) & ATA_DATA_REQUEST )) {
        if (--timeout == 0) {
            error("ATA DRQ timeout");
            u8 status = __asm_inb(ATA_STATUS);
            if (status & ATA_ERROR) {
                u8 err = __asm_inb(ATA_ERROR);
                error("ATA error: status=0x%x, error=0x%x", status, err);
            }
            return;
        }
    }
}

static void reset(void) {
    OUTB(ATA_CONTROL, 0x04 | 0x02)
    iowait();
    iowait();

    OUTB(ATA_CONTROL, 0x02)
    iowait();
    iowait();

    for (i32 i = 0; i < 100000; i++) {
        // TODO: check errors
    }
}

static void identify(u16* buffer) {
    OUTB(ATA_DRIVE_HEAD, 0xa0)
    OUTB(ATA_SECTOR_COUNT, 0)
    OUTB(ATA_LBA_LOW, 0)
    OUTB(ATA_LBA_HIGH, 0)
    OUTB(ATA_CMD, ATA_CMD_IDENTIFY)

    ata.wait_data_request();
    for (i32 i = 0; i < SECTOR_SIZE / 2; i++) {
        buffer[i] = __asm_inw(ATA_DATA);
    }
}

static void read(u32 lba, u16* buffer) {
    ata.wait_busy();

    OUTB(ATA_DRIVE_HEAD, 0xe0 | ((lba >> 24) & 0x0f))
    OUTB(ATA_SECTOR_COUNT, 1)
    OUTB(ATA_LBA_LOW, (u8)(lba & 0xff))
    OUTB(ATA_LBA_MIDDLE, (u8)((lba >> 8) & 0xff))
    OUTB(ATA_LBA_HIGH, (u8)((lba >> 16) & 0xff))
    OUTB(ATA_CMD, ATA_CMD_READ)
    ata.wait_busy();

    ata.wait_data_request();
    for (i32 i = 0; i < SECTOR_SIZE / 2; i++) {
        buffer[i] = __asm_inw(ATA_DATA);
    }
}

static void write(u32 lba, const u16* buffer)
{
    debug("ATA write: lba = %d", lba);
    wait_busy();

    OUTB(ATA_DRIVE_HEAD, 0xe0 | ((lba >> 24) & 0x0f))
    OUTB(ATA_SECTOR_COUNT, 1)
    OUTB(ATA_LBA_LOW,  lba & 0xff)
    OUTB(ATA_LBA_MIDDLE,  (lba >> 8) & 0xff)
    OUTB(ATA_LBA_HIGH, (lba >> 16) & 0xff)
    OUTB(ATA_CMD, ATA_CMD_WRITE);
    wait_busy();

    wait_data_request();
    for (int i = 0; i < 256; i++) {
        OUTW(ATA_DATA, buffer[i])
    }
}

void init_driver_ata(void) {
    ata.wait_busy = wait_busy;
    ata.wait_data_request = wait_data_request;
    ata.reset = reset;
    ata.identify = identify;
    ata.read = read;
    ata.write = write;
}

struct DriverATA* get_driver_ata(void) {
    return &ata;
}