/* Linker script to configure memory regions. */
MEMORY
{
  /* We're using the s110 soft device (and the bootloader) */
  FLASH (rx) : ORIGIN = 0x00018000, LENGTH = 0x1FC00
  STORAGE_REGION (r) : ORIGIN = 0x3F000, LENGTH = 0x400
  RAM (rwx) : ORIGIN = 0x20002C00, LENGTH = 0x5380
}
INCLUDE "gcc_nrf51_common.ld"
