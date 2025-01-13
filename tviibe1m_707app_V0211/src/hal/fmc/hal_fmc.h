#ifndef HAL_FMC_H
#define HAL_FMC_H

uint8_t HAL_FMC_Initial(fmc_config_t*  tsConfig);
uint8_t Hal_FMC_Deinit(void);
uint8_t HAL_FMC_Status_Get(uint32_t *pu32Status);

/*Code flash */
uint8_t HAL_FMC_Flash_Safety(uint8_t u8Enable);
uint8_t HAL_FMC_FlashEraseAll(uint8_t u8Verify);
uint8_t HAL_FMC_FlashEraseSector(uint32_t u32SectorAddr ,uint8_t u8Verify);
uint8_t HAL_FMC_FlashWrite (uint8_t *pu8Data,uint32_t u32Address,uint16_t  u16DataLength);
uint8_t HAL_FMC_FlashRead  (uint8_t *pu8Data,uint32_t u32Address,uint16_t  u16DataLength);

/*Working flash*/
uint8_t HAL_FMC_EEPROM_Safety( uint8_t u8Enable);
uint8_t HAL_FMC_EEPROMEraseAll(uint8_t u8Verify);
uint8_t HAL_FMC_EEPROMEraseSector(uint32_t u32SectorAddr ,uint8_t u8Verify);
uint8_t HAL_FMC_EEPROMWrite(uint8_t *pu8Data,uint32_t u32Address,uint16_t  u16DataLength);     
uint8_t HAL_FMC_EEPROMRead (uint8_t *pu8Data,uint32_t u32Address,uint16_t  u16DataLength);

#endif

  