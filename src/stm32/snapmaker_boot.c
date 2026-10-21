// Support for the Snapmaker J1 (gd32f305) factory bootloader
//
// Copyright (C) 2026  Nimrasol
//
// This file may be distributed under the terms of the GNU GPLv3 license.

#include <string.h> // memcpy
#include "board/irq.h" // irq_disable
#include "board/misc.h" // bootloader_request
#include "internal.h" // FLASH
#include "sched.h" // DECL_INIT

// The factory bootloader only starts the application if the "update
// info" header stored in flash has the "app normal" status and the
// application checksum it contains matches the flash contents.  After
// an update the bootloader leaves an "update done" status and relies on
// the application to restore "app normal" (as the stock firmware does
// on every boot).  After a reset the bootloader waits a few seconds for
// a new update header (SACP command 0xad/0x01, as the stock display sends
// it) before it starts the application; a bootloader request therefore
// only resets.  The bootloader never erases or writes its own pages.

#define HDR_ADDR 0x080FD000      // Update info header (4KiB flash page)
#define HDR_SIZE 101             // sizeof(update_packet_info_t)
#define HDR_STATUS_OFFSET 81
#define HDR_CHECKSUM_OFFSET 97
#define STATUS_APP_NORMAL 0xAA05

// Pages above 512KiB are in the second flash bank, which has its own
// copy of the key, status, control and address registers
#define FMC_KEY1  (*(volatile uint32_t *)(FLASH_R_BASE + 0x44))
#define FMC_STAT1 (*(volatile uint32_t *)(FLASH_R_BASE + 0x4c))
#define FMC_CTL1  (*(volatile uint32_t *)(FLASH_R_BASE + 0x50))
#define FMC_ADDR1 (*(volatile uint32_t *)(FLASH_R_BASE + 0x54))

// Checksum used by the bootloader (inverted sum of big-endian words)
static uint32_t
snapmaker_checksum(const uint8_t *buf, uint32_t len)
{
    uint32_t sum = 0, i;
    for (i = 0; i + 1 < len; i += 2)
        sum += (buf[i] << 8) | buf[i + 1];
    if (len & 1)
        sum += buf[len - 1];
    return ~sum;
}

static void
flash_wait(void)
{
    while (FMC_STAT1 & FLASH_SR_BSY)
        ;
    FMC_STAT1 = FLASH_SR_EOP | FLASH_SR_PGERR | FLASH_SR_WRPRTERR;
}

// Rewrite the header with a new status (the page holds only the header)
static void
write_header_status(uint16_t status)
{
    uint8_t hdr[HDR_SIZE + 1];
    memcpy(hdr, (void *)HDR_ADDR, HDR_SIZE);
    hdr[HDR_SIZE] = 0xff;
    hdr[HDR_STATUS_OFFSET] = status;
    hdr[HDR_STATUS_OFFSET + 1] = status >> 8;
    uint32_t csum = snapmaker_checksum(hdr, HDR_CHECKSUM_OFFSET);
    memcpy(&hdr[HDR_CHECKSUM_OFFSET], &csum, sizeof(csum));

    irqstatus_t flag = irq_save();
    FMC_KEY1 = FLASH_KEY1;
    FMC_KEY1 = FLASH_KEY2;
    flash_wait();
    FMC_CTL1 = FLASH_CR_PER;
    FMC_ADDR1 = HDR_ADDR;
    FMC_CTL1 = FLASH_CR_PER | FLASH_CR_STRT;
    flash_wait();
    FMC_CTL1 = FLASH_CR_PG;
    volatile uint16_t *dest = (void *)HDR_ADDR;
    uint32_t i;
    for (i = 0; i < HDR_SIZE; i += 2) {
        dest[i / 2] = hdr[i] | (hdr[i + 1] << 8);
        flash_wait();
    }
    FMC_CTL1 = FLASH_CR_LOCK;
    irq_restore(flag);
}

static int
header_is_valid(void)
{
    const uint8_t *hdr = (void *)HDR_ADDR;
    uint32_t csum;
    memcpy(&csum, &hdr[HDR_CHECKSUM_OFFSET], sizeof(csum));
    return csum == snapmaker_checksum(hdr, HDR_CHECKSUM_OFFSET);
}

static uint16_t
header_status(void)
{
    const uint8_t *hdr = (void *)HDR_ADDR;
    return hdr[HDR_STATUS_OFFSET] | (hdr[HDR_STATUS_OFFSET + 1] << 8);
}

void
snapmaker_boot_init(void)
{
    if (header_is_valid() && header_status() != STATUS_APP_NORMAL)
        write_header_status(STATUS_APP_NORMAL);
}
DECL_INIT(snapmaker_boot_init);

// Reboot into the factory bootloader (scripts/snapmaker_j1_flash.py of
// KlipperSnapmakerJ1 then sends the new image)
void
snapmaker_bootloader_request(void)
{
    irq_disable();
    NVIC_SystemReset();
}
