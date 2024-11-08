#include "stm32f1xx_hal.h"

//#include "spi.h"
//#include "gpio.h"
#include "w25q.h"

int32_t w25qDiskInitialize(void)
{
	return 0;
}

// Чтение статусного регистра
void w25qReadStatus(uint8_t *answer, uint8_t regnum)
{  
  W25Q_CS_LO();    
  // Передать команду чтения статусного регистра
  HAL_SPI_Transmit(W25Q_SPI, &regnum, 1, 100);
  // Прочитать статусный регистр
  HAL_SPI_Receive(W25Q_SPI, answer, 1, 100);  
  W25Q_CS_HI();
}

// Чтение бита занятости
void w25qWaitForReady(void)
{
  uint8_t reg = W25Q_RD_STATUS_1;
  uint8_t status = 0;

  W25Q_CS_LO();
    
  do
  {
    status = 0;
    
    W25Q_CS_LO(); 
    // Передать команду чтения статусного регистра
    HAL_SPI_Transmit(W25Q_SPI, &reg, 1, 100);
    // Прочитать статусный регистр
    HAL_SPI_Receive(W25Q_SPI, &status, 1, 100);
    W25Q_CS_HI();
  }
  while (status & 0x01);
    
  
}

// Установка защитных бит
void w25qSetBlockProtect(uint8_t protect)
{
  uint8_t temp[] = {0x50, 0x00};

  W25Q_CS_LO();
  HAL_SPI_Transmit(W25Q_SPI, temp, 1, 100);
  W25Q_CS_HI();
    
  temp[0] = W25Q_WR_STATUS;
  temp[1] = ((protect & 0x0F) << 2);

  W25Q_CS_LO();
  HAL_SPI_Transmit(W25Q_SPI, temp, 2, 100);
  W25Q_CS_HI();
}

// Чтение данных от микросхемы
void w25qReadManuf(uint8_t *answer)
{
  // Массив для записи команды
  uint8_t status = W25Q_JEDEC_ID;

  W25Q_CS_LO();
  // Передать команду
  HAL_SPI_Transmit(W25Q_SPI, &status, 1, 100);
  // Прочитать сигнатуру
  HAL_SPI_Receive(W25Q_SPI, answer, 3, 100);
  W25Q_CS_HI();
}


// Read Sector
void w25qReadSector(uint32_t nSector, char* pBuffer){
	// Create an array for the command
	  uint8_t cmd[5] = {W25Q_READ_DATA,
			  (uint8_t)(nSector >> 16),
	          (uint8_t)(nSector >> 8),
			  (uint8_t) nSector};
	  W25Q_CS_LO();
	  // Pass command and page address
	  HAL_SPI_Transmit(W25Q_SPI, cmd, 4, 100);
	  // Read page
	  HAL_SPI_Receive(W25Q_SPI, pBuffer, FLASH_SECTOR_SIZE, 500); // was 100
	  W25Q_CS_HI();

	  w25qWaitForReady();
}

//Write Sector
void w25qWriteSector(uint32_t nSector, char* pBuffer)
{
	int i,j;

	nSector *= FLASH_SECTOR_SIZE;
	for(j=0;j<FLASH_PAGES_PER_SECTOR;j++)
		{
			w25qWritePage(pBuffer, nSector);
			pBuffer += FLASH_PAGE_SIZE;
			nSector += FLASH_PAGE_SIZE;
			w25qWaitForReady();
		}
}

// Reading the page
void w25qReadPage(uint8_t *page, uint16_t pagenum)
{
  // Create an array for the command
  uint8_t com[5] = {W25Q_READ_PAGE, (uint8_t)(pagenum >> 8), 
                   (uint8_t)(pagenum & 0x00FF), 0x00, 0x00};
  
  W25Q_CS_LO();
  // Pass command and page address
  HAL_SPI_Transmit(W25Q_SPI, com, 5, 100);
  // Read page
  HAL_SPI_Receive(W25Q_SPI, page, W25Q_PAGE_LEN, 10); // было 100
  W25Q_CS_HI();
}

// Page entry
// Takes an array of data as an argument
// a page long and the page number for the entry
void w25qWritePage(uint8_t *page, uint16_t pagenum)
{  
  uint8_t temp[4] = {W25Q_WRITE_ENABLE, 0x00, 0x00, 0x00};
  
  w25qSetBlockProtect(0x00);

  W25Q_CS_LO();
  HAL_SPI_Transmit(W25Q_SPI, temp, 1, 100); // было 100
  W25Q_CS_HI();
    
  temp[0] = W25Q_PAGE_PROGRAM;
  temp[1] = (uint8_t)(pagenum >> 8);
  temp[2] = (uint8_t)(pagenum & 0x00FF);
  temp[3] = 0x00;

  W25Q_CS_LO();
  HAL_SPI_Transmit(W25Q_SPI, temp, 4, 100);
  HAL_SPI_Transmit(W25Q_SPI, page, W25Q_PAGE_LEN, 100); // было 100
  W25Q_CS_HI();
    
  w25qWaitForReady();
    
  temp[0] = W25Q_WRRITE_DISABLE;

  W25Q_CS_LO();
  HAL_SPI_Transmit(W25Q_SPI, temp, 1, 100); // было 100
  W25Q_CS_HI();
    
  w25qSetBlockProtect(0x0F);
}

// Очистка сектора
void w25qEraseSector(uint16_t sector)
{
  w25qSetBlockProtect(0x00);
  
  sector = (sector << 1);  
  uint8_t com = W25Q_WRITE_ENABLE;
  uint8_t temp[4] = {W25Q_SECTOR_ERASE_4, (uint8_t)(sector >> 8), 
                    (uint8_t)(sector & 0x00FF), 0x00};
  W25Q_CS_LO();
  // Передать команду и адрес страницы
  HAL_SPI_Transmit(W25Q_SPI, &com, 1, 100);
  W25Q_CS_HI();

  W25Q_CS_LO();
  // Передать команду и адрес страницы
  HAL_SPI_Transmit(W25Q_SPI, temp, 4, 100);
  W25Q_CS_HI();

  w25qWaitForReady();
   
  temp[0] = W25Q_WRRITE_DISABLE;
   
  W25Q_CS_LO();
  HAL_SPI_Transmit(W25Q_SPI, temp, 1, 100);
  W25Q_CS_HI();

  w25qSetBlockProtect(0x0F);
}

// Стереть всю память
void w25qEraseChip(void)
{
  w25qSetBlockProtect(0x00);
    
  uint8_t temp[3] = {W25Q_WRITE_ENABLE, W25Q_CHIP_ERASE, W25Q_WRRITE_DISABLE};

  W25Q_CS_LO();
  // Передать команду и адрес страницы
  HAL_SPI_Transmit(W25Q_SPI, &temp[0], 1, 100);
  W25Q_CS_HI();
    
  W25Q_CS_LO();
  // Передать команду и адрес страницы
  HAL_SPI_Transmit(W25Q_SPI, &temp[1], 1, 100);
  W25Q_CS_HI();

  w25qWaitForReady();
   
  W25Q_CS_LO();
  // Передать команду и адрес страницы
  HAL_SPI_Transmit(W25Q_SPI, &temp[2], 1, 100);
  W25Q_CS_HI();

  w25qSetBlockProtect(0x0F);
}
