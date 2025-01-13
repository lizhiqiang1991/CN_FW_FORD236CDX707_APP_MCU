/*
* Mod_DetectI2C.c
*
*  Created on: 2020�~11��16��
*      Author: DerekWen
*/
/* include global */
#include "gPinDef.h"
#include "UartApp.h"

/* include module */
#include "Mod_DetectI2C.h"
#include "Mod_DetectGPIO.h"
#include "Mod_DataManagement.h"
#include "Mod_BLBrightness_Ctrl.h"

/* include APP */
#include "I2CMasterApp.h"

static DISPLAY_STATUS_T MDI2C_ErrorState_t;
static uint8_t SourceIC58Faultcount = NUM_ZERO;
static uint8_t SourceIC59Faultcount = NUM_ZERO;
static uint8_t SourceIC5AFaultcount = NUM_ZERO;
static uint8_t SourceIC5BFaultcount = NUM_ZERO;
uint8_t u8Reset_Requst_flag = false; 

diag_byte *tmp_Diag;

/* only detect LED fault byte2~6 */
static diag_byte LED_L_LED1_B2 = 
{
	.effective_bit_mask = 0x01,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_L_LED1_B3 = 
{
	.effective_bit_mask = 0xFD,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_L_LED1_B4 = 
{
	.effective_bit_mask = 0xE0,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_L_LED1_B5 = 
{
	.effective_bit_mask = 0xBC,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_L_LED1_B6 = 
{
	.effective_bit_mask = 0x02,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_L_LED2_B2 = 
{
	.effective_bit_mask = 0x01,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_L_LED2_B3 = 
{
	.effective_bit_mask = 0xFD,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_L_LED2_B4 = 
{
	.effective_bit_mask = 0xE0,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_L_LED2_B5 = 
{
	.effective_bit_mask = 0xBC,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_L_LED2_B6 = 
{
	.effective_bit_mask = 0x02,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_L_LED3_B2 = 
{
	.effective_bit_mask = 0x01,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_L_LED3_B3 = 
{
	.effective_bit_mask = 0xFD,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_L_LED3_B4 = 
{
	.effective_bit_mask = 0xE0,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_L_LED3_B5 = 
{
	.effective_bit_mask = 0xBC,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_L_LED3_B6 = 
{
	.effective_bit_mask = 0x02,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_R_LED1_B2 = 
{
	.effective_bit_mask = 0x01,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_R_LED1_B3 = 
{
	.effective_bit_mask = 0xFD,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_R_LED1_B4 = 
{
	.effective_bit_mask = 0xE0,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_R_LED1_B5 = 
{
	.effective_bit_mask = 0xBC,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_R_LED1_B6 = 
{
	.effective_bit_mask = 0x02,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_R_LED2_B2 = 
{
	.effective_bit_mask = 0x01,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_R_LED2_B3 = 
{
	.effective_bit_mask = 0xFD,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_R_LED2_B4 = 
{
	.effective_bit_mask = 0xE0,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_R_LED2_B5 = 
{
	.effective_bit_mask = 0xBC,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_R_LED2_B6 = 
{
	.effective_bit_mask = 0x02,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_R_LED3_B2 = 
{
	.effective_bit_mask = 0x01,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_R_LED3_B3 = 
{
	.effective_bit_mask = 0xFD,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_R_LED3_B4 = 
{
	.effective_bit_mask = 0xE0,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_R_LED3_B5 = 
{
	.effective_bit_mask = 0xBC,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte LED_R_LED3_B6 = 
{
	.effective_bit_mask = 0x02,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte TCON_B1 = 
{
	.effective_bit_mask = 0x22,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte TCON_B2 = 
{
	.effective_bit_mask = 0x01,
	.timeout = {4,4,4,4,4,4,4,4},
};

static diag_byte SOURCE_IC_58H_B1 = 
{
	.effective_bit_mask = 0x74,
	.timeout = {30,30,30,30,30,30,30,30},
};

static diag_byte SOURCE_IC_59H_B1 = 
{
	.effective_bit_mask = 0x74,
	.timeout = {30,30,30,30,30,30,30,30},
};

static diag_byte SOURCE_IC_5AH_B1 = 
{
	.effective_bit_mask = 0x74,
	.timeout = {30,30,30,30,30,30,30,30},
};

static diag_byte SOURCE_IC_5BH_B1 = 
{
	.effective_bit_mask = 0x74,
	.timeout = {30,30,30,30,30,30,30,30},
};

/* IC總成 */
diag_byte *HX8880_L_LED1_Diag[5] = {&LED_L_LED1_B2, &LED_L_LED1_B3, &LED_L_LED1_B4, &LED_L_LED1_B5, &LED_L_LED1_B6};
diag_byte *HX8880_L_LED2_Diag[5] = {&LED_L_LED2_B2, &LED_L_LED2_B3, &LED_L_LED2_B4, &LED_L_LED2_B5, &LED_L_LED2_B6};
diag_byte *HX8880_L_LED3_Diag[5] = {&LED_L_LED3_B2, &LED_L_LED3_B3, &LED_L_LED3_B4, &LED_L_LED3_B5, &LED_L_LED3_B6};
diag_byte *HX8880_R_LED1_Diag[5] = {&LED_R_LED1_B2, &LED_R_LED1_B3, &LED_R_LED1_B4, &LED_R_LED1_B5, &LED_R_LED1_B6};
diag_byte *HX8880_R_LED2_Diag[5] = {&LED_R_LED2_B2, &LED_R_LED2_B3, &LED_R_LED2_B4, &LED_R_LED2_B5, &LED_R_LED2_B6};
diag_byte *HX8880_R_LED3_Diag[5] = {&LED_R_LED3_B2, &LED_R_LED3_B3, &LED_R_LED3_B4, &LED_R_LED3_B5, &LED_R_LED3_B6};
diag_byte *HX8880_TCON_Diag[2] = {&TCON_B1, &TCON_B2};
diag_byte *HS82105_SOURCE_58H_Diag[1] = {&SOURCE_IC_58H_B1};
diag_byte *HS82105_SOURCE_59H_Diag[1] = {&SOURCE_IC_59H_B1};
diag_byte *HS82105_SOURCE_5AH_Diag[1] = {&SOURCE_IC_5AH_B1};
diag_byte *HS82105_SOURCE_5BH_Diag[1] = {&SOURCE_IC_5BH_B1};

//static uint8_t HX82105_Display_Status = NUM_ZERO;


static uint8_t I2cMasterBuffer[I2SM_RBUFFER] = {NUM_ZERO};

/* for debug */
//static uint8_t I2cStaCheck = NUM_ZERO;

const uint8_t Led_timeout_max = 5U;

/**
* @brief MDI2C_ErrorCounter_Maintain
* 
* @param ErrorCounterIndex 
* @param LastState 
* @param CurrentState 
* @return true 
* @return false 
*/
bool MDI2C_ErrorCounter_Maintain(DisplayReg0x00_E ErrorCounterIndex, bool LastState, bool CurrentState)
{
	/*Boundary check*/
	if (ErrorCounterIndex >= ErrorNum)
	{
		return LastState;
	}
	else
	{
		;
	}
    
	/*Judge state of Error changed */
	if((CurrentState == true) && (LastState != CurrentState))
	{
		MMIM_ErrorCount_Add(ErrorCounterIndex);
	}
	else
	{
		;
	}
    
	LastState = CurrentState;
	return LastState;
}


/******************************************************************************
;       Function Name              :       uint8_t TconApp_Magic_Code_Write(uint8_t u8TconAddr)
;       Function Description       :       Set Magic Code
;       Parameters                 :       [u8TconAddr] - Cmd Address
;       Return Values              :       I2C
******************************************************************************/
uint8_t MDI2C_TconApp_Magic_Code_Write(uint8_t u8TconAddr)
{
    uint8_t u8Status= false;
    uint8_t u8Wr_Data = TCON_MAGIC_CODE;
    
    //TCON_ADDRESS_ID
    if (DRIVER_TRUE == I2CM_Write(TCON_ADDRESS_ID, u8TconAddr, &u8Wr_Data, sizeof(u8Wr_Data)))
    {
        u8Status = true;
    }
    else
    {
        u8Status = false;
    }
    
    return u8Status;
}
    
/* ComPare and Record Backlight Register */
uint8_t MDI2C_CPNR_BL_Reg(uint8_t *pu8ErrFlag, ERR_DataType_E u8LedStartIndex)
{
    uint8_t u8Result = NUM_ONE;
    //uint8_t u8LedFaultToA3[1] = {0x00};
    
    uint8_t loop, checkloop;
	uint8_t checkbit;
	uint8_t bitmask;
    uint8_t offset;
    uint8_t loop_max;
    //diag_byte *tmp_Diag;
    uint8_t source_ic_adrr;

    if(u8LedStartIndex == TCON_LED_L_LED1_B2)
    {
        tmp_Diag = *HX8880_L_LED1_Diag;
        offset = 1U;
        loop_max = 5U;
    }
    else if(u8LedStartIndex == TCON_LED_L_LED2_B2)
    {
        tmp_Diag = *HX8880_L_LED2_Diag;
        offset = 1U;
        loop_max = 5U;
    }
    else if(u8LedStartIndex == TCON_LED_L_LED3_B2)
    {
        tmp_Diag = *HX8880_L_LED3_Diag;
        offset = 1U;
        loop_max = 5U;
    }
    else if(u8LedStartIndex == TCON_LED_R_LED1_B2)
    {
        tmp_Diag = *HX8880_R_LED1_Diag;
        offset = 1U;
        loop_max = 5U;
    }
    else if(u8LedStartIndex == TCON_LED_R_LED2_B2)
    {
        tmp_Diag = *HX8880_R_LED2_Diag;
        offset = 1U;
        loop_max = 5U;
    }

    else if(u8LedStartIndex == TCON_LED_R_LED3_B2)
    {
        tmp_Diag = *HX8880_R_LED3_Diag;
        offset = 1U;
        loop_max = 5U;
    }
    else if(u8LedStartIndex == TCON_FAIL_B1)
    {
        tmp_Diag = *HX8880_TCON_Diag;
        offset = 0U;
        loop_max = 2U;
    }
    else if(u8LedStartIndex == SOURCE_IC_58H)
    {
        tmp_Diag = *HS82105_SOURCE_58H_Diag;
        offset = 0U;
        loop_max = 1U;
        source_ic_adrr = SOURCE_IC_ADDRESS_58H;
    }
    else if(u8LedStartIndex == SOURCE_IC_59H)
    {
        tmp_Diag = *HS82105_SOURCE_59H_Diag;
        offset = 0U;
        loop_max = 1U;
        source_ic_adrr = SOURCE_IC_ADDRESS_59H;
    }
    else if(u8LedStartIndex == SOURCE_IC_5AH)
    {
        tmp_Diag = *HS82105_SOURCE_5AH_Diag;
        offset = 0U;
        loop_max = 1U;
        source_ic_adrr = SOURCE_IC_ADDRESS_5AH;
    }
    else if(u8LedStartIndex == SOURCE_IC_5BH)
    {
        tmp_Diag = *HS82105_SOURCE_5BH_Diag;
        offset = 0U;
        loop_max = 1U;
        source_ic_adrr = SOURCE_IC_ADDRESS_5BH;
    }

	for(loop = 0;loop < loop_max;loop++)
	{
         /*根據offest設定*/
        tmp_Diag[loop].byte.byte = pu8ErrFlag[loop + offset];
		checkbit = tmp_Diag[loop].effective_bit_mask;
		bitmask = 0x01;
		//DEBUG_PF("======Loop: %d, checkbit:%x\r\n", loop, checkbit);
		for(checkloop = 0;checkloop < 8;checkloop++)
		{
			//DEBUG_PF("raw data:%x\r\n",tmp_Diag[loop].byte.byte);
			//DEBUG_PF("check:%x\r\n",bitmask);
			if( checkbit & bitmask)
			{
				//DEBUG_PF("effect bit:%d\r\n",checkloop);
                /*LED byte6 bit1 normal=1, falut=0, change bitmask=0x00*/
				if(tmp_Diag[loop].byte.byte & bitmask)
				{
					/* 有效bit發生錯誤 */
					//DEBUG_PF("===>error:%d\r\n",checkloop);
					if(tmp_Diag[loop].cnt[checkloop] >= tmp_Diag[loop].timeout[checkloop])
					{
                        //DEBUG_PF("error\r\n");
						/* True Fault */
                        #if 0
						DEBUG_PF("Diag[%d], bit:%d happen fault\r\n", loop, checkloop);
						DEBUG_PF("Diag[%d] byte data:%x\r\n", loop, tmp_Diag[loop].byte.byte);
						DEBUG_PF("Diag[%d] cnt:%d\r\n", loop, tmp_Diag[loop].cnt[checkloop]);
						DEBUG_PF("Diag[%d] timout:%d\r\n", loop, tmp_Diag[loop].timeout[checkloop]);
                        #endif
     
                        tmp_Diag[loop].cnt[checkloop] = 0;
                        tmp_Diag[loop].report = tmp_Diag[loop].report | bitmask;
						//tmp_Diag[loop].report[checkloop] = 1;
                        u8Result = NUM_ZERO;
					}
					else
					{
						/* 錯誤持續 */
						tmp_Diag[loop].cnt[checkloop]++;
                        //DEBUG_PF("byte =  %d, bit =  %d ,cnt = %d\r\n",loop, checkloop,tmp_Diag[loop].cnt[checkloop]);
					}
                    /*write source IC fault pin*/
                    if(u8LedStartIndex == SOURCE_IC_58H || u8LedStartIndex == SOURCE_IC_59H || u8LedStartIndex == SOURCE_IC_5AH || u8LedStartIndex == SOURCE_IC_5BH)
                    {
                        //DEBUG_PF("error data = %d, ~bitmask = %d\r\n",u8SourceICErrReg[0],~bitmask);
                        tmp_Diag[loop].byte.byte &= (~bitmask);
                        //DEBUG_PF("conbin data = %d\r\n",u8SourceICErrReg[0]);
                        I2CM_Write(source_ic_adrr, 0x60U, &tmp_Diag[loop].byte.byte, SOURCELENGTH);
                    }
				}
				else
				{
					tmp_Diag[loop].cnt[checkloop] = 0;
                    /*reset bitmask*/
					//tmp_Diag[loop].report[checkloop] = 0;
				}
			}
			else
			{
				//DEBUG_PF("ignore bit %d\r\n",checkloop);//ignore
			}
			bitmask <<= 1;
		}
	}
    return u8Result;
}

BL_Result_E MDI2C_New_BL_Detect(void)
{
    static BL_Detect_I2C_State_E eState = eBL_STATUS1_Wr;    /* state  */
    BL_Result_E eReault                 = eProcessing;       /* result */
    static uint8_t Left_LED_Status      = NUM_ONE;         
    static uint8_t Right_LED_Status     = NUM_ONE;
    static uint8_t Fail_Flag            = NUM_ONE;
    uint8_t I2cStaCheck = NUM_ZERO;
    
    uint8_t u8Length = TCON_LED_STATUS_LENGTH + TCON_MAGICCODE_LENGTH + TCON_CHECKSUM_LENGTH;
    uint8_t u8Length2 = TCON_FAIL_FLAG_LENGTH + TCON_MAGICCODE_LENGTH + TCON_CHECKSUM_LENGTH;
    
    uint8_t u8LedErrReg[6]     = {NUM_ZERO};
    uint8_t u8BufferIndex      = NUM_ZERO;
    uint8_t u8LEDIndicator     = NUM_ZERO;
    uint8_t u8LeftLEDNumber    = 3;
    uint8_t u8RightLEDNumber   = 3;
    uint16_t u16CheckSum       = NUM_ZERO;
    uint8_t temp_result        = NUM_ONE;
    uint8_t loop;
    
    switch(eState)
    {
        case eBL_STATUS1_Wr:
          
              if( MDI2C_TconApp_Magic_Code_Write(TCON_LED_STATUS1) == true)
              {
                  MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) & (~TCON_LED_L_NACK)));
                  //MMIM_Client_Diag_Set(TCON_LED_L_NACK , 0x00);  
                  eState = eBL_STATUS1_Rd;   /*go to eBL_STATUS1_Rd state */  
              }
              else
              {
                  MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) | TCON_LED_L_NACK));
                  //MMIM_Client_Diag_Set(TCON_LED_L_NACK , 0x01);
                  eState = eBL_STATUS2_Wr;
                  Left_LED_Status = NUM_ZERO;
              }
              break;
          
        case eBL_STATUS1_Rd:
          
              I2cStaCheck = I2CM_Read(TCON_ADDRESS_ID, TCON_LED_STATUS1, &I2cMasterBuffer[0], u8Length);
              if(I2cStaCheck == DRIVER_TRUE)
              {
                  MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) & (~TCON_LED_L_NACK)));
                  //MMIM_Client_Diag_Set(TCON_LED_L_NACK , 0x00);
              }
              else
              {
                  MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) | TCON_LED_L_NACK));
                  //MMIM_Client_Diag_Set(TCON_LED_L_NACK , 0x01);
                  eState = eBL_STATUS2_Wr;
                  Left_LED_Status = NUM_ZERO;
                  break;
              }
              
              
              if(I2cMasterBuffer[0] == TCON_MAGIC_CODE) /* confirm magic code */
              {
                  for(u8BufferIndex = 0; u8BufferIndex < u8Length - 1; u8BufferIndex++)  
                  {
                      u16CheckSum = u16CheckSum + (uint16_t)I2cMasterBuffer[u8BufferIndex];
                  }
                  u16CheckSum = u16CheckSum & 0x00FFU;
                  
                  if(I2cMasterBuffer[u8Length - 1] == (uint8_t)u16CheckSum) /* confirm checksum */
                  {
                      for(u8LEDIndicator = 0; u8LEDIndicator < u8LeftLEDNumber; u8LEDIndicator++)
                      {
                          u8LedErrReg[0] = I2cMasterBuffer[u8LEDIndicator*6 + 1];  /* 0x01 byte1 */
                          u8LedErrReg[1] = I2cMasterBuffer[u8LEDIndicator*6 + 2];  /* 0x01 byte2 */
                          u8LedErrReg[2] = I2cMasterBuffer[u8LEDIndicator*6 + 3];  /* 0x02 byte1 */
                          u8LedErrReg[3] = I2cMasterBuffer[u8LEDIndicator*6 + 4];  /* 0x02 byte2 */
                          u8LedErrReg[4] = I2cMasterBuffer[u8LEDIndicator*6 + 5];  /* 0x03 byte1 */
                          u8LedErrReg[5] = ~(I2cMasterBuffer[u8LEDIndicator*6 + 6]);  /* 0x03 byte2 */

                          //DEBUG_PF("diagnostic LED_L_%d input data= %x %x %x %x %x %x\r\n",u8LEDIndicator, u8LedErrReg[0],u8LedErrReg[1],u8LedErrReg[2],u8LedErrReg[3],u8LedErrReg[4],u8LedErrReg[5]);

                          if(u8LEDIndicator == 0)
                          {
                              temp_result = MDI2C_CPNR_BL_Reg(u8LedErrReg, TCON_LED_L_LED1_B2);
                              Left_LED_Status = Left_LED_Status & temp_result;
                              /*detect LED fault, write to A3*/
                              if( temp_result == NUM_ZERO)
                              {
                                u8LedErrReg[1] = tmp_Diag[0].report;
                                u8LedErrReg[2] = tmp_Diag[1].report;
                                u8LedErrReg[3] = tmp_Diag[2].report;
                                u8LedErrReg[4] = tmp_Diag[3].report;
                                u8LedErrReg[5] = tmp_Diag[4].report;

                                //DEBUG_PF("output data3 = %d\r\n",u8LedErrReg[3]);
                                //DEBUG_PF("output data5 = %d\r\n",u8LedErrReg[5]);

                                for(loop = 0; loop < 5; loop++)
                                {
                                    MMIM_Client_Diag_Set((ERR_DataType_E)(TCON_LED_L_LED1_B2+loop) ,u8LedErrReg[loop+1]);
                                }     
                              } 
                          }
                          else if(u8LEDIndicator == 1)
                          {
                              temp_result = MDI2C_CPNR_BL_Reg(u8LedErrReg, TCON_LED_L_LED2_B2);
                              Left_LED_Status = Left_LED_Status & temp_result;
                              /*detect LED fault, write to A3*/
                              if( temp_result == NUM_ZERO)
                              {
                                u8LedErrReg[1] = tmp_Diag[0].report;
                                u8LedErrReg[2] = tmp_Diag[1].report;
                                u8LedErrReg[3] = tmp_Diag[2].report;
                                u8LedErrReg[4] = tmp_Diag[3].report;
                                u8LedErrReg[5] = tmp_Diag[4].report;

                                for(loop = 0; loop < 5; loop++)
                                {
                                    MMIM_Client_Diag_Set((ERR_DataType_E)(TCON_LED_L_LED2_B2+loop) ,u8LedErrReg[loop+1]);
                                }     
                              }
                          }
                          else if(u8LEDIndicator == 2)
                          {
                              temp_result = MDI2C_CPNR_BL_Reg(u8LedErrReg, TCON_LED_L_LED3_B2);
                              Left_LED_Status = Left_LED_Status & temp_result;
                              /*detect LED fault, write to A3*/
                              if( temp_result == NUM_ZERO)
                              {
                                u8LedErrReg[1] = tmp_Diag[0].report;
                                u8LedErrReg[2] = tmp_Diag[1].report;
                                u8LedErrReg[3] = tmp_Diag[2].report;
                                u8LedErrReg[4] = tmp_Diag[3].report;
                                u8LedErrReg[5] = tmp_Diag[4].report;

                                for(loop = 0; loop < 5; loop++)
                                {
                                    MMIM_Client_Diag_Set((ERR_DataType_E)(TCON_LED_L_LED3_B2+loop) ,u8LedErrReg[loop+1]);
                                }     
                              }
                          }
                                                                                                                                                   
                      }
                  }
                  else
                  {
                      /* Invalid data, mcu do nothing */
                  }
              }
              else
              {
                  /* Invalid data, mcu do nothing */
              }             
              
              eState = eBL_STATUS2_Wr;   /*go to eBL_STATUS2_Wr state */
              break;
          
        case eBL_STATUS2_Wr:
              
              if( MDI2C_TconApp_Magic_Code_Write(TCON_LED_STATUS2) == true)
              {
                  MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) & (~TCON_LED_R_NACK)));
                  //MMIM_Client_Diag_Set(TCON_LED_R_NACK , 0x00);  
                  eState = eBL_STATUS2_Rd;   /*go to eBL_STATUS2_Rd state */
              }
              else
              {
                  MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) | TCON_LED_R_NACK));
                  //MMIM_Client_Diag_Set(TCON_LED_R_NACK , 0x01);
                  eState = eBL_FailFlag_Wr;
                  Right_LED_Status = NUM_ZERO;
              }
              break;
        
        case eBL_STATUS2_Rd:
          
              I2cStaCheck = I2CM_Read(TCON_ADDRESS_ID, TCON_LED_STATUS2, &I2cMasterBuffer[0], u8Length);
              if(I2cStaCheck == DRIVER_TRUE)
              {
                  MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) & (~TCON_LED_R_NACK)));
                  //MMIM_Client_Diag_Set(TCON_LED_R_NACK , 0x00);
              }
              else
              {
                  MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) | TCON_LED_R_NACK));
                  //MMIM_Client_Diag_Set(TCON_LED_R_NACK , 0x01);
                  eState = eBL_FailFlag_Wr;
                  Right_LED_Status = NUM_ZERO;
                  break;
              }
              
              
              if(I2cMasterBuffer[0] == TCON_MAGIC_CODE) /* confirm magic code */
              {
                  for(u8BufferIndex = 0; u8BufferIndex < u8Length - 1; u8BufferIndex++)  
                  {
                      u16CheckSum = u16CheckSum + (uint16_t)I2cMasterBuffer[u8BufferIndex];
                  }
                  u16CheckSum = u16CheckSum & 0x00FFU;
                  
                  if(I2cMasterBuffer[u8Length - 1] == (uint8_t)u16CheckSum) /* confirm checksum */
                  {
                      for(u8LEDIndicator = 0; u8LEDIndicator < u8RightLEDNumber; u8LEDIndicator++)
                      {
                          u8LedErrReg[0] = I2cMasterBuffer[u8LEDIndicator*6 + 1];  /* 0x01 byte1 */
                          u8LedErrReg[1] = I2cMasterBuffer[u8LEDIndicator*6 + 2];  /* 0x01 byte2 */
                          u8LedErrReg[2] = I2cMasterBuffer[u8LEDIndicator*6 + 3];  /* 0x02 byte1 */
                          u8LedErrReg[3] = I2cMasterBuffer[u8LEDIndicator*6 + 4];  /* 0x02 byte2 */
                          u8LedErrReg[4] = I2cMasterBuffer[u8LEDIndicator*6 + 5];  /* 0x03 byte1 */
                          u8LedErrReg[5] = ~(I2cMasterBuffer[u8LEDIndicator*6 + 6]);  /* 0x03 byte2 */

                          //DEBUG_PF("diagnostic LED_R_%d input data= %x %x %x %x %x %x\r\n",u8LEDIndicator, u8LedErrReg[0],u8LedErrReg[1],u8LedErrReg[2],u8LedErrReg[3],u8LedErrReg[4],u8LedErrReg[5]);

                          if(u8LEDIndicator == 0)
                          {
                              //DEBUG_PF("input data3 = %d\r\n",u8LedErrReg[3]);
                              //Right_LED_Status = Right_LED_Status & MDI2C_CPNR_BL_Reg(u8LedErrReg, TCON_LED_R_LED1_B1);
                              temp_result = MDI2C_CPNR_BL_Reg(u8LedErrReg, TCON_LED_R_LED1_B2);
                              Right_LED_Status = Right_LED_Status & temp_result;
                              /*detect LED fault, write to A3*/
                              if( temp_result == NUM_ZERO)
                              {
                                u8LedErrReg[1] = tmp_Diag[0].report;
                                u8LedErrReg[2] = tmp_Diag[1].report;
                                u8LedErrReg[3] = tmp_Diag[2].report;
                                u8LedErrReg[4] = tmp_Diag[3].report;
                                u8LedErrReg[5] = tmp_Diag[4].report;

                                //DEBUG_PF("output data3 = %d\r\n",u8LedErrReg[3]);

                                for(loop = 0; loop < 5; loop++)
                                {
                                    MMIM_Client_Diag_Set((ERR_DataType_E)(TCON_LED_R_LED1_B2+loop) ,u8LedErrReg[loop+1]);
                                }     
                              }
                          }
                          else if(u8LEDIndicator == 1)
                          {
                              //Right_LED_Status = Right_LED_Status & MDI2C_CPNR_BL_Reg(u8LedErrReg, TCON_LED_R_LED2_B1);
                              temp_result = MDI2C_CPNR_BL_Reg(u8LedErrReg, TCON_LED_R_LED2_B2);
                              Right_LED_Status = Right_LED_Status & temp_result;
                              /*detect LED fault, write to A3*/
                              if( temp_result == NUM_ZERO)
                              {
                                u8LedErrReg[1] = tmp_Diag[0].report;
                                u8LedErrReg[2] = tmp_Diag[1].report;
                                u8LedErrReg[3] = tmp_Diag[2].report;
                                u8LedErrReg[4] = tmp_Diag[3].report;
                                u8LedErrReg[5] = tmp_Diag[4].report;
                                
                                for(loop = 0; loop < 5; loop++)
                                {
                                    MMIM_Client_Diag_Set((ERR_DataType_E)(TCON_LED_R_LED2_B2+loop) ,u8LedErrReg[loop+1]);
                                }     
                              }
                          }
                          else if(u8LEDIndicator == 2)
                          {
                              //Right_LED_Status = Right_LED_Status & MDI2C_CPNR_BL_Reg(u8LedErrReg, TCON_LED_R_LED3_B1);
                              temp_result = MDI2C_CPNR_BL_Reg(u8LedErrReg, TCON_LED_R_LED3_B2);
                              Right_LED_Status = Right_LED_Status & temp_result;
                              /*detect LED fault, write to A3*/
                              if( temp_result == NUM_ZERO)
                              {
                                u8LedErrReg[1] = tmp_Diag[0].report;
                                u8LedErrReg[2] = tmp_Diag[1].report;
                                u8LedErrReg[3] = tmp_Diag[2].report;
                                u8LedErrReg[4] = tmp_Diag[3].report;
                                u8LedErrReg[5] = tmp_Diag[4].report;

                                for(loop = 0; loop < 5; loop++)
                                {
                                    MMIM_Client_Diag_Set((ERR_DataType_E)(TCON_LED_R_LED3_B2+loop) ,u8LedErrReg[loop+1]);
                                }     
                              }
                          }                                                                                                                      
                      }
                  }
                  else
                  {
                      /* Invalid data, mcu do nothing */
                  }
              }
              else
              {
                   /* Invalid data, mcu do nothing */
              }       
          
              eState = eBL_FailFlag_Wr;  /*go to eBL_FailFlag_Wr state */
              break;
          
        case eBL_FailFlag_Wr:
          
              if(MDI2C_TconApp_Magic_Code_Write(TCON_FAIL_FLAG) == true)
              {
                  MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) & (~TCON_FAIL_NACK)));
                  //MMIM_Client_Diag_Set(TCON_FAIL_NACK , 0x00);
              }
              else
              {
                  MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) | TCON_FAIL_NACK));
                  //MMIM_Client_Diag_Set(TCON_FAIL_NACK , 0x01);
              }
          
              eState = eBL_FailFlag_Rd;  /*go to eBL_FailFlag_Rd state */
              break;
          
        case eBL_FailFlag_Rd:
          
              I2cStaCheck = I2CM_Read(TCON_ADDRESS_ID, TCON_FAIL_FLAG, &I2cMasterBuffer[0], u8Length2);
              if(I2cStaCheck == DRIVER_TRUE)
              {
                  MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) & (~TCON_FAIL_NACK)));
                  //MMIM_Client_Diag_Set(TCON_FAIL_NACK , 0x00);
              }
              else
              {
                  MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) | TCON_FAIL_NACK));
                  //MMIM_Client_Diag_Set(TCON_FAIL_NACK , 0x01);
                  Fail_Flag = NUM_ZERO;
              }
          
              if(I2cMasterBuffer[0] == TCON_MAGIC_CODE)
              {
                  for(u8BufferIndex = 0; u8BufferIndex < u8Length2 - 1; u8BufferIndex++)
                  {
                      u16CheckSum = u16CheckSum + (uint16_t)I2cMasterBuffer[u8BufferIndex];
                  }
                  u16CheckSum = u16CheckSum & 0x00FFU;
                  
                  if(I2cMasterBuffer[u8Length2 - 1] == (uint8_t)u16CheckSum)
                  {  
                    
                        u8LedErrReg[0] = I2cMasterBuffer[1];  
                        u8LedErrReg[1] = I2cMasterBuffer[2];  
                        temp_result = MDI2C_CPNR_BL_Reg(u8LedErrReg, TCON_FAIL_B1);
                        Fail_Flag &= temp_result;
                        if( temp_result == NUM_ZERO)
                        {
                            u8LedErrReg[1] = tmp_Diag[0].report;
                            u8LedErrReg[2] = tmp_Diag[1].report;

                            for(loop = 0; loop < 2; loop++)
                            {
                                MMIM_Client_Diag_Set((ERR_DataType_E)(TCON_FAIL_B1+loop) ,u8LedErrReg[loop+1]);
                            }     
                        }
                  }
                  else
                  {     
                      /* Invalud data, mcu do nothing */
                  }
                  
              }
              else
              {
                /* Invalud data, mcu do nothing */
              }
                        
              if((Left_LED_Status && Right_LED_Status && Fail_Flag) == NUM_ONE)
              {
                  eReault = eBLERR_Pass;
              }
              else
              {
                  eReault = eBLERR_Fail;
              }
              
              /* After register judgement, restore below value to default value */
              Right_LED_Status = NUM_ONE;
              Left_LED_Status  = NUM_ONE;
              Fail_Flag        = NUM_ONE;
              
              eState = eBL_STATUS1_Wr;   /*go to eBL_STATUS1_Wr state */
              break;
          
        default:
              eState = eBL_STATUS1_Wr;
              break;         
    }
    
    return eReault;
}

BL_Result_E MDI2C_TCON_Fail_Detect(void)
{
    static BL_Detect_I2C_State_E eTconState = eBL_FailFlag_Wr;    /* state  */
    BL_Result_E eReault                 = eProcessing;       /* result */
    static uint8_t Fail_Flag            = NUM_ONE;
    static uint8_t Wait_Count           = NUM_ONE;
    uint8_t I2cStaCheck = NUM_ZERO;
    uint8_t u8Length2 = TCON_FAIL_FLAG_LENGTH + TCON_MAGICCODE_LENGTH + TCON_CHECKSUM_LENGTH;
    
    uint8_t u8TconErrReg[2]     = {NUM_ZERO};
    uint8_t u8BufferIndex      = NUM_ZERO;
    uint16_t u16CheckSum       = NUM_ZERO;

    switch(eTconState)
    {
        case eBL_FailFlag_Wr:
              if(MDI2C_TconApp_Magic_Code_Write(TCON_FAIL_FLAG) == true)
              {
                  MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) & (~TCON_FAIL_NACK)));
                  //MMIM_Client_Diag_Set(TCON_FAIL_NACK , 0x00);
              }
              else
              {
                  MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) | TCON_FAIL_NACK));
                  //MMIM_Client_Diag_Set(TCON_FAIL_NACK , 0x01);
              }
          
              eTconState = eBL_Wait;  /*go to eBL_FailFlag_Rd state */
        break;

        case eBL_Wait:
            if(16 > Wait_Count)
            {
                Wait_Count++;
            }
            else if(16 == Wait_Count)
            {
                eTconState = eBL_FailFlag_Rd;
                Wait_Count = 0;
            }
            else
            {
                Wait_Count = 0;
            }
        break;

        case eBL_FailFlag_Rd:
            I2cStaCheck = I2CM_Read(TCON_ADDRESS_ID, TCON_FAIL_FLAG, &I2cMasterBuffer[0], u8Length2);
            if(I2cStaCheck == DRIVER_TRUE)
            {
                MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) & (~TCON_FAIL_NACK)));
                //MMIM_Client_Diag_Set(TCON_FAIL_NACK , 0x00);
            }
            else
            {
                MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) | TCON_FAIL_NACK));
                //MMIM_Client_Diag_Set(TCON_FAIL_NACK , 0x01);
                Fail_Flag = NUM_ZERO;
            }
        
            if(I2cMasterBuffer[0] == TCON_MAGIC_CODE)
            {
                for(u8BufferIndex = 0; u8BufferIndex < u8Length2 - 1; u8BufferIndex++)
                {
                    u16CheckSum = u16CheckSum + (uint16_t)I2cMasterBuffer[u8BufferIndex];
                }
                u16CheckSum = u16CheckSum & 0x00FFU;
                
                if(I2cMasterBuffer[u8Length2 - 1] == (uint8_t)u16CheckSum)
                {  
                
                    u8TconErrReg[0] = I2cMasterBuffer[1];  
                    u8TconErrReg[1] = I2cMasterBuffer[2];

                    if(true == (u8TconErrReg[0] && 0x80))
                    {
                        MMIM_Client_Diag_Set((ERR_DataType_E)(TCON_FAIL_B1) ,u8TconErrReg[0]);
                        Fail_Flag = NUM_ZERO;
                    } 
                }
                else
                {     
                    /* Invalud data, mcu do nothing */
                }
                
            }
            else
            {
            /* Invalud data, mcu do nothing */
            }

              if(NUM_ONE == Fail_Flag)
              {
                  eReault = eBLERR_Pass;
              }
              else
              {
                  eReault = eBLERR_Fail;
              }
        break;

        default:
            eTconState = eBL_FailFlag_Wr;
        break;
    }
    return eReault;    
}


/**
* @brief 
* 
*/

uint8_t MDI2C_DispStatus_Detect(void)
{
    //uint8_t u8Temp;
	uint8_t HX82105_58H_Status = NUM_ONE;
    uint8_t HX82105_59H_Status = NUM_ONE;
    uint8_t HX82105_5AH_Status = NUM_ONE;
    uint8_t HX82105_5BH_Status = NUM_ONE;
    uint8_t u8SourceICErrReg[1]= {NUM_ZERO};
    uint8_t I2cStaCheck = NUM_ZERO;
    uint8_t HX82105_Display_Status = NUM_ZERO;
    //uint8_t loop;
    //uint8_t bitmask;

	if (I2cMasterBuffer != NULL)
	{
		(void)memset(I2cMasterBuffer, 0, sizeof(I2cMasterBuffer));
	}

	/* HX82105 IC 	Source diver fault address data */
    /*detect address = 0x58*/
	I2cStaCheck = I2CM_Read(0x58U, 0x60U, &I2cMasterBuffer[0], LCDERRLENGTH);
    if(I2cStaCheck == DRIVER_TRUE)
	{
        SourceIC58Faultcount = 0;
        MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) & (~SOURCE_IC_58H_NACK)));
        //MMIM_Client_Diag_Set(SOURCE_IC_58H_NACK , 0x00);
        /*test*/
        u8SourceICErrReg[0] =  I2cMasterBuffer[0];
        //u8SourceICErrReg[0] = 0x04;
        HX82105_58H_Status = MDI2C_CPNR_BL_Reg(u8SourceICErrReg,SOURCE_IC_58H);
        //DEBUG_PF("check HX82105_58H_Status  = %d,\r\n",HX82105_58H_Status);
        if(HX82105_58H_Status == NUM_ZERO)
        {
            /*detect source IC fault, write to A3*/
            MMIM_Client_Diag_Set(SOURCE_IC_58H , u8SourceICErrReg[0]);
        }
	}
	else
	{
        SourceIC58Faultcount++;
        if(SourceIC58Faultcount >= 30)
        {
            SourceIC58Faultcount = 0;
            MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) | SOURCE_IC_58H_NACK));
            //MMIM_Client_Diag_Set(SOURCE_IC_58H_NACK , 0x01);
            HX82105_58H_Status = NUM_ZERO;
            MDI2C_ResetRequstFlag_Ctrl(true);
        }
    }

    /*detect address = 0x59*/
	I2cStaCheck = I2CM_Read(0x59U, 0x60U, &I2cMasterBuffer[1], LCDERRLENGTH);
    if(I2cStaCheck == DRIVER_TRUE)
	{
        SourceIC59Faultcount = 0;
        MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) & (~SOURCE_IC_59H_NACK)));
        //MMIM_Client_Diag_Set(SOURCE_IC_59H_NACK , 0x00);
        u8SourceICErrReg[0] =  I2cMasterBuffer[1];
        HX82105_59H_Status = MDI2C_CPNR_BL_Reg(u8SourceICErrReg,SOURCE_IC_59H);
        if(HX82105_59H_Status == NUM_ZERO)
        {
            /*detect source IC fault, write to A3*/
            MMIM_Client_Diag_Set(SOURCE_IC_59H , u8SourceICErrReg[0]);
        }
	}
	else
	{
        SourceIC59Faultcount++;
        if(SourceIC59Faultcount >= 30)
        {
            SourceIC59Faultcount = 0;
            MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) | SOURCE_IC_59H_NACK));
            //MMIM_Client_Diag_Set(SOURCE_IC_59H_NACK , 0x01);
            HX82105_59H_Status = NUM_ZERO;
            MDI2C_ResetRequstFlag_Ctrl(true);
        }

    }

	/*detect address = 0x5A*/
	I2cStaCheck = I2CM_Read(0x5AU, 0x60U, &I2cMasterBuffer[2], LCDERRLENGTH);
    if(I2cStaCheck == DRIVER_TRUE)
	{
        SourceIC5AFaultcount = 0;
        MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) & (~SOURCE_IC_5AH_NACK)));
        //MMIM_Client_Diag_Set(SOURCE_IC_5AH_NACK , 0x00);
        u8SourceICErrReg[0] =  I2cMasterBuffer[2];
        HX82105_5AH_Status = MDI2C_CPNR_BL_Reg(u8SourceICErrReg,SOURCE_IC_5AH);
        if(HX82105_5AH_Status == NUM_ZERO)
        {
            /*detect source IC fault, write to A3*/
            MMIM_Client_Diag_Set(SOURCE_IC_5AH , u8SourceICErrReg[0]);
        }
	}
	else
	{
        SourceIC5AFaultcount++;
        if(SourceIC5AFaultcount >=30)
        {
            SourceIC5AFaultcount = 0;
            MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) | SOURCE_IC_5AH_NACK));
            //MMIM_Client_Diag_Set(SOURCE_IC_5AH_NACK , 0x01);
            HX82105_5AH_Status = NUM_ZERO;
            MDI2C_ResetRequstFlag_Ctrl(true);
        }

    }
    
	/*detect address = 0x5B*/
	I2cStaCheck = I2CM_Read(0x5BU, 0x60U, &I2cMasterBuffer[3], LCDERRLENGTH);
    if(I2cStaCheck == DRIVER_TRUE)
	{
        SourceIC5BFaultcount = 0;
        MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) & (~SOURCE_IC_5BH_NACK)));
        //MMIM_Client_Diag_Set(SOURCE_IC_5BH_NACK , 0x00);
        u8SourceICErrReg[0] =  I2cMasterBuffer[3];
        HX82105_5BH_Status = MDI2C_CPNR_BL_Reg(u8SourceICErrReg,SOURCE_IC_5BH);
        if(HX82105_5BH_Status == NUM_ZERO)
        {
            /*detect source IC fault, write to A3*/
            MMIM_Client_Diag_Set(SOURCE_IC_5BH , u8SourceICErrReg[0]);
        }
	}
	else
	{
        SourceIC5BFaultcount++;
        if(SourceIC5BFaultcount >= 30)
        {
            SourceIC5BFaultcount = 0;
            MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) | SOURCE_IC_5BH_NACK));
            //MMIM_Client_Diag_Set(SOURCE_IC_5BH_NACK , 0x01);
            HX82105_5BH_Status = NUM_ZERO;
            MDI2C_ResetRequstFlag_Ctrl(true);
        }
    }
    
    //DEBUG_PF("58H_Status=%d 59H_Status=%d 5AH_Status=%d 5BH_Status=%d\r\n",HX82105_58H_Status,HX82105_59H_Status,HX82105_5AH_Status,HX82105_5BH_Status);
	if((HX82105_58H_Status && HX82105_59H_Status && HX82105_5AH_Status && HX82105_5BH_Status) ==  NUM_ONE)
	{
		HX82105_Display_Status = NUM_ONE;
		MDI2C_ErrorState_t.DISP_ST = MDI2C_ErrorCounter_Maintain(eDisplayStatus,MDI2C_ErrorState_t.DISP_ST,true );
	}
	else
	{
		HX82105_Display_Status = NUM_ZERO;
		MDI2C_ErrorState_t.DISP_ST = MDI2C_ErrorCounter_Maintain(eDisplayStatus,MDI2C_ErrorState_t.DISP_ST,false );
	}
	return HX82105_Display_Status;
}

uint8_t MDI2C_VCOM_Detect(void)
{
    static uint8_t u8VCOMFlag = 1;
    
    if(u8VCOMFlag == 1)
    {
        if (I2cMasterBuffer != NULL)
        {
            (void)memset(I2cMasterBuffer,0,sizeof(I2cMasterBuffer));
        }
    
        I2CM_Read(TFTBIAS_ADDRESS_ID, 0x09, &I2cMasterBuffer[0], 1);
        MMIM_VCOM_Set(I2cMasterBuffer[0]);
        u8VCOMFlag = 0;
        //i2c_messages.vcom = I2cMasterBuffer[0];
    }
    else
    {       
        ;
    }
    return 1;
}

void MDI2C_TCON_VER_DETECT(void)
{
    static uint8_t u8READ_TCON_VER_Flag = 1;
    uint8_t u8Length = TCON_VERSION_LENGTH + TCON_MAGICCODE_LENGTH + TCON_CHECKSUM_LENGTH;
    
    if(u8READ_TCON_VER_Flag == 1)
    {
        if (I2cMasterBuffer != NULL)
        {
            (void)memset(I2cMasterBuffer,0,sizeof(I2cMasterBuffer));
        }

        MDI2C_TconApp_Magic_Code_Write(TCON_VERSION);
        Cy_SysLib_Delay(5);
        I2CM_Read(TCON_ADDRESS_ID, TCON_VERSION, &I2cMasterBuffer[0], u8Length);
        
        MMIM_TCON_VER_Set(I2cMasterBuffer[1]);
        
        u8READ_TCON_VER_Flag = 0;
    }
    
}

uint8_t MDI2C_25221_Fault1(void)
{
    /*
    25221 slave address = 0x21
    Reg Fault1          = 0x05
    
    bit0   vgoff_uv
    bit1   vgoff_ov
    bit2   vgon_uv
    bit3   vgon_ov
    bit4   navdd_uv
    bit5   navdd_ov
    bit6   avdd_uv
    bit7   avdd_ov
    */
    uint8_t u8MAX25221_FLT1 = 1U;
    uint8_t u8Return;
    uint8_t u8Temp;

    if(!Get_Dimming_Action_flag())
    {
        u8Return = I2CM_Read(TFTBIAS_ADDRESS_ID, 0x05, &u8MAX25221_FLT1, sizeof(u8MAX25221_FLT1));
        
        if(u8Return == DRIVER_TRUE)
        {
            MMIM_DispVoltError_Set(u8MAX25221_FLT1);
            
            /* record ACK to 0xA3 Joel 20220316 */
                MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) & (~VCOM_FAULT_NACK)));
                //MMIM_Client_Diag_Set(VCOM_FAULT_NACK , 0x00);
            
            /* record error to 0xA3 */
            u8Temp = MMIM_Client_Diag_Get(VCOM_FAULT);
            u8Temp |= u8MAX25221_FLT1;
            MMIM_Client_Diag_Set(VCOM_FAULT , u8Temp);
        }
        else
        {
            /* record NACK to 0xA3 */
            MMIM_Client_Diag_Set(General_Device_NACK , (MMIM_Client_Diag_Get(General_Device_NACK) | VCOM_FAULT_NACK));
            //MMIM_Client_Diag_Set(VCOM_FAULT_NACK , 0x01);
            MDI2C_ResetRequstFlag_Ctrl(true);
            u8MAX25221_FLT1 = 1;
        }
    }
    else
    {
       u8MAX25221_FLT1 = 0;
    }
    return u8MAX25221_FLT1;

}

void MDI2C_25221_Revision(void)
{
    /*
    25221 slave address = 0x21
    Reg_CTRL      : 0x02
    Revision Mask : 0x07   0000 0111
    */
    
    static uint8_t u8TFT_RevID_Flag = 1;
    uint8_t u8TFT_RevID = 0;
    
    if (I2cMasterBuffer != NULL)
    {
        (void)memset(I2cMasterBuffer,0,sizeof(I2cMasterBuffer));
    }
    
    if(u8TFT_RevID_Flag == 1)
    {
        I2CM_Read(TFTBIAS_ADDRESS_ID, 0x02, &I2cMasterBuffer[0], 1);
        
        u8TFT_RevID = I2cMasterBuffer[0] & TFT_REV_ID_MASK;
        
        MMIM_TFT_Revision_Set(u8TFT_RevID);
        
        if(u8TFT_RevID != 0) /* in case that TFT-bias is not ready before mcu asking */
        {
            u8TFT_RevID_Flag = 0;
        }  
    }
    else
    {       
        ;
    }

}

void MDI2C_Clear_Leddriver_faultpin_cnt(void)
{
    uint8_t loop, bit_loop;
    
    for(loop = 0; loop < 5; loop++)
    {
        for(bit_loop = 0; bit_loop < 8; bit_loop++)
        {
            HX8880_L_LED1_Diag[loop]->cnt[bit_loop] = 0;
            HX8880_L_LED2_Diag[loop]->cnt[bit_loop] = 0;
            HX8880_L_LED3_Diag[loop]->cnt[bit_loop] = 0;
            HX8880_R_LED1_Diag[loop]->cnt[bit_loop] = 0;
            HX8880_R_LED2_Diag[loop]->cnt[bit_loop] = 0;
            HX8880_R_LED3_Diag[loop]->cnt[bit_loop] = 0;
        }

    }
}

void MDI2C_Clear_TCON_faultpin_cnt(void)
{
    uint8_t loop, bit_loop;
    for(loop = 0; loop < 2; loop++)
    {
        for(bit_loop = 0; bit_loop < 8; bit_loop++)
    {
            HX8880_TCON_Diag[loop]->cnt[bit_loop] = 0;
        }
    }   
}

void MDI2C_Clear_SourceIC_faultpin_cnt(void)
{
    uint8_t bit_loop;
    
    for(bit_loop = 0; bit_loop < 8; bit_loop++)
    {
        HS82105_SOURCE_58H_Diag[0]->cnt[bit_loop] = 0;
        HS82105_SOURCE_59H_Diag[0]->cnt[bit_loop] = 0;
        HS82105_SOURCE_5AH_Diag[0]->cnt[bit_loop] = 0;
        HS82105_SOURCE_5BH_Diag[0]->cnt[bit_loop] = 0;
    }

}

uint8_t MDI2C_ResetRequstFlag_Ctrl(uint8_t u8FlagState)
{
    switch (u8FlagState)
    {
    case GETFLAG:
        break;
        
    default:
        u8Reset_Requst_flag = u8FlagState;
        break;
    }

    return u8Reset_Requst_flag;
}

uint8_t MDI2C_Set_TCON_BlackPattern(uint8_t onoff)
{
    uint8_t bist_mode_addr = 0x84;
	uint8_t buffer_length = 9;
	uint8_t cmd_buffer1[9] = {0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04}; 
	uint8_t cmd_buffer2[9] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x84}; 
    uint8_t u8status = true;

    if(ENABLE == onoff)
    {
        //DEBUG_PF("start black pattern\r\n");
        I2CM_Write(TCON_ADDRESS_ID,bist_mode_addr,cmd_buffer1,buffer_length);
    }
    else if(DISABLE == onoff)
    {
        //DEBUG_PF("end black pattern\r\n");
        I2CM_Write(TCON_ADDRESS_ID,bist_mode_addr,cmd_buffer2,buffer_length);
    }
    else
    {
        ;
    }

    return  u8status;
}