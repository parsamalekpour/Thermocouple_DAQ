#ifndef _W25Q_H
#define _W25Q_H

#include <stdint.h>
#include "stm32f1xx_hal.h"
// SPI port (Change to the desired one if necessary)
extern SPI_HandleTypeDef        hspi1;

//Flash
#define FLASH_PAGE_SIZE			256
#define FLASH_SECTOR_SIZE		4096
#define FLASH_SECTOR_COUNT		512
#define FLASH_BLOCK_SIZE		65536
#define FLASH_PAGES_PER_SECTOR	FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE

#define W25Q_SPI                &hspi1

// Dummy
#define W25Q_SPI_NOP            0xFF

// Port on which CS is connected
#define W25Q_PORT     GPIOA

// Pin on which CS is connected
#define W25Q_PIN      GPIO_PIN_2

// Output CS to 0
#define W25Q_CS_LO() HAL_GPIO_WritePin(W25Q_PORT, W25Q_PIN, GPIO_PIN_RESET);
// Output CS to 1
#define W25Q_CS_HI() HAL_GPIO_WritePin(W25Q_PORT, W25Q_PIN, GPIO_PIN_SET);


// Chip memory size
#define W25Q_MAMORY_8           0x4014 // 1МБ
#define W25Q_MAMORY_16          0x4015 // 2МБ
#define W25Q_MAMORY_32          0x4016 // 4МБ

// Company manufacturer
#define W25Q_WINBOND            0xEF // Winbond

// Page size
#define W25Q_PAGE_LEN           256

// Sector size in KB
#define W25Q_SECTOR_4           0x04

// Block size in KB
#define W25Q_BLOCK_64           0x40

// Number of pages
#define W25Q_PAGE_8             0x0FFF
#define W25Q_PAGE_16            0x1FFF
#define W25Q_PAGE_32            0x3FFF


// Commands
#define W25Q_WRITE_ENABLE       0X06
#define W25Q_WRRITE_DISABLE     0X04
#define W25Q_RD_STATUS_1        0x05       
#define W25Q_RD_STATUS_2        0x35
#define W25Q_WR_STATUS          0x01
#define W25Q_PAGE_PROGRAM       0x02
#define W25Q_Q_PAGE_PROGRAM     0x32
#define W25Q_BLOCK_ERASE_64     0xD8
#define W25Q_BLOCK_ERASE_32     0x52
#define W25Q_SECTOR_ERASE_4     0x20
#define W25Q_CHIP_ERASE         0xC7
#define W25Q_ERASE_SUSPEND      0X75
#define W25Q_ERASE_RESUME       0x7A
#define W25Q_POWER_DOWN         0xB9
#define W25Q_HI_PF_MODE         0xA3
#define W25Q_MODE_BIT_RESET     0xFF
#define W25Q_RS_PWR_DOUN        0xAB
#define W25Q_MNF_DEVICE_ID      0x90
#define W25Q_RD_UNIQUE_ID       0x4B
#define W25Q_JEDEC_ID           0x9F
#define W25Q_READ_PAGE          0x0B

#define W25Q_READ_DATA			0x03

// Read the status register
void w25qReadStatus(uint8_t *answer, uint8_t regnum);

// Wait for the busy bit to clear
void w25qWaitForReady(void);

// Set security bits
void w25qSetBlockProtect(uint8_t protect);

// Read the signature.
// Takes an array of 3 cells as an argument
// After calling the function, the array will contain:
// answer[0] = manufacturer index
// answer[1] = high byte of memory size
// answer[2] = low byte of memory size
// Example: 0xEF, 0x40, 0x16 = W25Q32
void w25qReadManuf(uint8_t *answer);

// Read the page
// Takes an array of
// 256 cells and page number to read
// After calling the function, the array will contain the read page
void w25qReadPage(uint8_t *page, uint16_t pagenum);

// Write page
// Takes an array of data as an argument
// 256 cells and page number for recording
void w25qWritePage(uint8_t *page, uint16_t pagenum);

// Clear 4KB sector
void w25qEraseSector(uint16_t sector);
void w25qReadSector(uint32_t nSector, char* pBuffer);
void w25qWriteSector(uint32_t nSector, char* pBuffer);


// Erase all memory
void w25qEraseChip(void);

int32_t w25qDiskInitialize(void);

#endif /* _W25Q_H */
