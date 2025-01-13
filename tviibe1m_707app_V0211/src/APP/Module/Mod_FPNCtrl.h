/**
* @file Mod_FPNCtrl.h
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-07-20
* 
* @copyright Copyright (c) 2021
* 
*/

#ifndef MOD_FPNCTRL_H_
#define MOD_FPNCTRL_H_

/* include library */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
/* include driver */
#include "gPinDef.h"
/* Declare global value */
#define FPN_SIZE 25U
#define FPN_DISP_ID_SIZE 2U

#define FPN_WRITE_SUCCESS 1U
#define FPN_WRITE_FAIL 2U
#define FPN_READ_SUCCESS 3U
#define FPN_CHECKSUM_ERR 4U

#define FPN_CHECKSUM_INDEX 25U
#define FPN_CHECKSUM_DISP_ID_INDEX 2U
#define FPN_CHECK_MASK 0xFFU

typedef struct
{
    uint8_t FPNCtrlFlag : 1 ;
    uint8_t FPNDataFlag : 1 ;
    uint8_t RESERVED : 6 ;
}__attribute__((__packed__)) FPN_FLAG_T;

typedef struct
{
    uint8_t FPN_WRT_ST : 1;
    uint8_t FPN_INT_WRT : 1;
    uint8_t CKSUM_ERR : 1;
    uint8_t RESERVED : 5;
}__attribute__((__packed__)) FPN_STATUS_T;

typedef struct
{
    FPN_FLAG_T FPN_Flag_t;
    FPN_STATUS_T FPN_Status_t;
    uint8_t *pFPNDataBuff;
    uint8_t FPNDataLength;
    uint8_t ReadFPNBuff[FPN_SIZE];
}FPN_T;

typedef struct
{
    FPN_FLAG_T FPN_Flag_t;
    FPN_STATUS_T FPN_Status_t;
    uint8_t *pFPNDataBuff;
    uint8_t FPNDataLength;
    uint8_t ReadFPNBuff[FPN_DISP_ID_SIZE];
}FPN_DISPLAY_ID_T;
/* Start program */

void MFPNC_FPNDelAssmToFlash_Write(uint8_t *pFPN,uint8_t u8Size);
void MFPNC_FPNDelAssmFromFlash_Read(uint8_t *pFPNBuff,uint8_t u8Size);
uint8_t MFPNC_FPNDelAssmControl_Process(FPN_T *fpn_t);

void MFPNC_FPNCoreAssmToFlash_Write(uint8_t *pFPN,uint8_t u8Size);
void MFPNC_FPNCoreAssmFromFlash_Read(uint8_t *pFPNBuff,uint8_t u8Size);
uint8_t MFPNC_FPNCoreAssmControl_Process(FPN_T *fpn_t);

void MFPNC_FPNMainCalibToFlash_Write(uint8_t *pFPN,uint8_t u8Size);
void MFPNC_FPNMainCalibFromFlash_Read(uint8_t *pFPNBuff,uint8_t u8Size);
uint8_t MFPNC_FPNMainCalibControl_Process(FPN_T *fpn_t);

void MFPNC_FPNDispIDToFlash_Write(uint8_t *pFPN,uint8_t u8Size);
void MFPNC_FPNDispIDFromFlash_Read(uint8_t *pFPNBuff,uint8_t u8Size);
uint8_t MFPNC_FPNDispIDControl_Process(FPN_DISPLAY_ID_T *fpn_t);


void MFPNC_FPNSoftwareToFlash_Write(uint8_t *pFPN,uint8_t u8Size);
void MFPNC_FPNSoftwareFromFlash_Read(uint8_t *pFPNBuff,uint8_t u8Size);
uint8_t MFPNC_FPNSoftwareControl_Process(FPN_T *fpn_t);

/* leo 20220621 */
void MFPNC_FPNSerialToFlash_Write(uint8_t *pFPN,uint8_t u8Size);
void MFPNC_FPNSerialFromFlash_Read(uint8_t *pFPNBuff,uint8_t u8Size);
uint8_t MFPNC_FPNSerialControl_Process(FPN_T *fpn_t); 

#endif /* MOD_FPNCTRL_H_ */