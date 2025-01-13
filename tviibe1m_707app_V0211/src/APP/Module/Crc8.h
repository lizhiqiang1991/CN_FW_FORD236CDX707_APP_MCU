/******************************************************************************
;       Program  : Crc8.h
;       Function : Declare CRC8 Function & Variable
;       Chip     : Cypress CYT2B6
******************************************************************************/
#ifndef __Crc8_H__
#define __Crc8_H__ 

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
//---------------------------- Define Constant ------------------------------//
//---------------------------- Support Function -----------------------------//
//---------------------------- Declare Function -----------------------------// 
uint8_t CRC8_Calculation(uint8_t *pu8data,uint8_t u8Length);
#endif




