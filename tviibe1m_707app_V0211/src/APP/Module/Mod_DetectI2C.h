/*
* Mod_DetectI2C.h
*
*  Created on: 2020�~11��16��
*      Author: DerekWen
*/

#ifndef MOD_DETECTI2C_H_
#define MOD_DETECTI2C_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define DISSCONNECTCHECKLENGTH 0x1U
#define DISSCONNECTCHECKDATA 0x00U
#define DISPSTATUSCHECKDATA 0x1FU
#define LCDERRLENGTH       0x01U
#define SOURCELENGTH       0x01U

#define HX82105_ERRPR_MASK 0x7FU

typedef struct{
	unsigned bit0:1;
	unsigned bit1:1;
	unsigned bit2:1;
	unsigned bit3:1;
	unsigned bit4:1;
	unsigned bit5:1;
	unsigned bit6:1;
	unsigned bit7:1;
}bit;

union ic_byte{
	uint8_t	byte;
	bit		bit;
};

typedef struct{
	union ic_byte byte;			/* 從IC讀回來的資料以byte為單位 */
	uint8_t	effective_bit_mask; /* 需要偵測的bit */
	uint8_t	cnt[8U];			/* debounce counter */
	const uint8_t timeout[8U];	/* ms base */
    uint8_t report;
	//uint8_t report[8U];
}diag_byte;

#if 0
typedef struct
{
    uint8_t vsync_timeout_err;
    uint8_t tshdn;
    uint8_t sled;
    uint8_t oled;
    uint8_t adj_ssh;
    uint8_t src_sh_vled;
    uint8_t src_sh_gnd;
    uint8_t biasp_uvlo;
    uint8_t v18_ov;
    uint8_t vled_uvlo;
    uint8_t iset_under;
    uint8_t iset_over;
    uint8_t biasp_ov;
    uint8_t vled_0v;
    uint8_t sg;

}TCON_LED_FAULT_COUNT;
#endif

typedef enum
{
	eTCON_Magic_Wr = 0U,
	eTCON_Reg_Read
}BLStrategyState_E;

typedef enum
{
    eBL_STATUS1_Wr    = 0U,
    eBL_STATUS1_Rd    = 1U,
    eBL_STATUS2_Wr    = 2U,
    eBL_STATUS2_Rd    = 3U,
    eBL_FailFlag_Wr   = 4U,
    eBL_FailFlag_Rd   = 5U,
    eBL_Wait          = 6U
}BL_Detect_I2C_State_E;

typedef enum
{
    eBLERR_Pass  = 0U,
    eBLERR_Fail  = 1U,
    eProcessing  = 2U,
}BL_Result_E;

uint8_t MDI2C_DispStatus_Detect(void);
uint8_t MDI2C_LCD_BL_Detect(void);
BL_Result_E MDI2C_New_BL_Detect(void);
uint8_t MDI2C_TconApp_Magic_Code_Write(uint8_t u8TconAddr);

/* leo 20211117 */
uint8_t MDI2C_VCOM_Detect(void);
uint8_t MDI2C_25221_Fault1(void);
void MDI2C_25221_Revision(void);
void MDI2C_Clear_Leddriver_faultpin_cnt(void);
void MDI2C_Clear_TCON_faultpin_cnt(void);
void MDI2C_Clear_SourceIC_faultpin_cnt(void);
void MDI2C_TCON_VER_DETECT(void);
uint8_t MDI2C_ResetRequstFlag_Ctrl(uint8_t u8FlagState);
/*michale add 20230517*/
uint8_t MDI2C_Set_TCON_BlackPattern(uint8_t);
BL_Result_E MDI2C_TCON_Fail_Detect(void);
#endif /* MOD_DETECTI2C_H_ */
