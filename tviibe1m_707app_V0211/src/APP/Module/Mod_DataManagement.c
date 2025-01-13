/*
* Mod_DataManagement.c
*
*  Created on: 2021-07-18
*      Author: Joshua
*/
/* include global */
#include "gPinDef.h"
#include "Ford_Config.h"

/* include module */
#include "Mod_DataManagement.h"
#include "Mod_BatteryProtection_Ctrl.h"
#include "Crc8.h"

/* Include App */
#include "FlashApp.h"
#include "GpioApp.h"
#include "UartApp.h"
#include "PWMApp.h"
/* Include Backdoor */
#include "ICDiagApp.h"

I2C_MESSAGES_T i2c_messages;
/* for temperature */
NTCTempValStruct NTC_Buffer[2] = {0};
/* For Watch dog switch */
static uint8_t WatchDogToogle_EN = DISABLE;
/* for bootloader */
uint8_t gApMode[] = {0x41, 0x50, 0x50};
/* for USER BIST mode control Read/Write */
static USER_BIST_MODE_CTRL_T u8BIST_DATA_t;

uint16_t gu16CoolFanPWMDuty = 0;

static uint16_t u16BacklightNtcAdcVaules, u16PCBNtcAdcVaules = 0U;

static uint8_t u8LowVolResetReq = 0xFF;

static uint8_t gMcuVerReturn[10] = "F-05.04.13"; // T-客戶baseline_功能更新(只能增加或固定)_小更新(特殊版本或測試版本) 
static uint8_t ucErrorPower = 0;
static uint8_t gReadBackErrorCounter[ErrorNum];
/* for active INTB Flag */
static uint8_t u8INITActiveFlag = DISABLE;
/* for Battery Detect */
static uint16_t u16BatteryVoltageValue = 0U;
static uint16_t uBatteryVoltAdcValue = 0U;

static const uint8_t LatchBitRegShift[DisplayStatusLatchBitSize] = {0U, 1U, 2U, 3U, 5U, 6U, 7U};
static const uint8_t LatchBitRegShift2[DisplayStatusLatchBit2Size] = {3U, 4U, 5U, 7U};

/* bootloader value */
static uint8_t gUserTouchReset = 0;
__no_init volatile uint16_t  u16UpdateKey @0x08001500;
static bool gbUpdateRead = false;

/* Power State */
static Global_PowerState_E MMIM_PowerState_e = ePowerState_Stop;
/* Flag for send event queue */
static uint8_t u8DispENFlag = DISABLE;
static uint8_t u8BLPWMFlag = DISABLE;
static uint8_t u8ScanDirectionFlag = DISABLE;
static uint8_t u8ShutdownFlag = DISABLE;
static uint8_t u8JumpToBootCodeFlag = DISABLE;
static uint8_t u8BISTModeFlag = NUM_ZERO;
static uint8_t u8DiagnosticMessageFlag = NUM_ZERO;

uint8_t sf_status =0;
/* for factory mode */
static uint8_t u8FactoryEn = DISABLE;
/*  for derating switch */
static uint8_t u8DeratingEN = ENABLE;
/*  for gErrorCounter */
static uint8_t gErrorCounter[ErrorNum];
/* bootloader function */

/* Lock pin status */
static uint8_t u8LockPinStatus = NUM_ZERO;
/* Power Initial status */
uint8_t u8PowerInitStatus = DISABLE;
/* P/N Read/Write */
static uint8_t u8DelAssmFPN[DELIVERY_ASSMBLY_FPN_SIZE] = {NUM_ZERO};
static uint8_t u8WriteDelAssmPNStatusRegFlag = NUM_ZERO;
static uint8_t u8DelAssmFPNFlag = NUM_ZERO;

static uint8_t u8CoreAssmFPN[CORE_ASSMBLY_FPN_SIZE] = {NUM_ZERO};
static uint8_t u8WriteCoreAssmPNStatusRegFlag = NUM_ZERO;
static uint8_t u8CoreAssmFPNFlag = NUM_ZERO;

static uint8_t u8MainCalibFPN[MAIN_CALIBRATION_FPN_SIZE] = {NUM_ZERO};
static uint8_t u8WriteMainCalibPNStatusRegFlag = NUM_ZERO;
static uint8_t u8MainCalibFPNFlag = NUM_ZERO;

static uint8_t u8DispIDFPN[DISPLAY_ID_FPN_SIZE] = {NUM_ZERO};
static uint8_t u8WriteDispIDPNStatusRegFlag = NUM_ZERO;
static uint8_t u8DispIDFPNFlag = NUM_ZERO;

static uint8_t u8SoftwareFPN[SOFTWARE_FPN_SIZE] = {NUM_ZERO};
static uint8_t u8WriteSoftwareFPNStatusRegFlag = NUM_ZERO;
static uint8_t u8SoftwareFPNFlag = NUM_ZERO;

/* leo 20220621 */
static uint8_t u8SerialNumFPN[SERIAL_FPN_SIZE] = {NUM_ZERO};
static uint8_t u8WriteSerialNumFPNStatusRegFlag = NUM_ZERO;
static uint8_t u8SerialNumFPNFlag = NUM_ZERO;

static uint8_t u8sourceDriver_rest_flag = DISABLE;

static uint8_t u8FlashBuf[128];
static uint8_t Crc8CheckBuff[32];    /* this buffer is used to calculate the CRC-8 value for diaplay transmit to host */
static uint8_t u8MCUSendRollingCnt = NUM_ZERO;
static uint8_t u8LastHostRollingCnt = NUM_ZERO;
static uint8_t u8LastHostSendsubAddress = NUM_ZERO;
static uint8_t u8CrcRuleAccept_flag = DISABLE;

static uint16_t u10BitPWMValue = NUM_ZERO;

static uint8_t u8VolRestRq_flag = DISABLE;

static bool bVoltageDerating = false;   /* leo 20211125 */

static uint8_t u8DispVoltSatus = NUM_ZERO;    /* leo 2021_12_13, if this byte != 0 indicate error occurs. */
static uint8_t u8LedVoltStatus = NUM_ZERO;     /* leo 2021_12_13, if this byte != 0 indicate error occurs. */

static uint8_t u8VCOM = NUM_ZERO;

static uint8_t u8TFT_Rev_ID = NUM_ZERO;

static uint8_t u8DeratingStatus = NUM_ZERO; /* 0 = normal mode, 1 = derating mode */

static uint8_t u8TCONversion = NUM_ZERO;

#if BACKDOOR_ICDIAG_OPEN
const uint8_t ICDIAG_CMD_ICFETCH_LENGTH = 9; /* this length size not include rc and crc */
const uint8_t ICDIAG_CMD_ICCTRL_LENGTH = 73; /* this length size not include rc and crc */
const uint8_t ICDIAG_CMD_READ_LENGTH = 64;   /* this length size not include rc and crc */

const uint8_t ucMcuVerU[11]  = {0x31, 0x70, 0x30, 0x5F, 0x31, 0x5F, 0x30, 0x33, 0x5F, 0x30, 0x30};	    // ECU Version is 1p0_1_03_00 -------
#endif

#if TCON_DIAG_NEW
/*  
*  Byte 0 : TCON LED L NACK
*  Byte 1 : TCON LED L LED1 Byte1 
*  Byte 2 : TCON LED L LED1 Byte2
*  Byte 3 : TCON LED L LED1 Byte3
*  Byte 4 : TCON LED L LED1 Byte4
*  Byte 5 : TCON LED L LED1 Byte5
*  Byte 6 : TCON LED L LED1 Byte6
*  Byte 7 : TCON LED L LED2 Byte1 
*  Byte 8 : TCON LED L LED2 Byte2
*  Byte 9 : TCON LED L LED2 Byte3
*  Byte 10 : TCON LED L LED2 Byte4
*  Byte 11 : TCON LED L LED2 Byte5
*  Byte 12 : TCON LED L LED2 Byte6
*  Byte 13 : TCON LED L LED3 Byte1 
*  Byte 14 : TCON LED L LED3 Byte2
*  Byte 15 : TCON LED L LED3 Byte3
*  Byte 16 : TCON LED L LED3 Byte4
*  Byte 17 : TCON LED L LED3 Byte5
*  Byte 18 : TCON LED L LED3 Byte6
*  Byte 19 : TCON LED R NACK
*  Byte 20 : TCON LED R LED1 Byte1 
*  Byte 21 : TCON LED R LED1 Byte2
*  Byte 22 : TCON LED R LED1 Byte3
*  Byte 23 : TCON LED R LED1 Byte4
*  Byte 24 : TCON LED R LED1 Byte5
*  Byte 25 : TCON LED R LED1 Byte6
*  Byte 26 : TCON LED R LED2 Byte1 
*  Byte 27 : TCON LED R LED2 Byte2
*  Byte 28 : TCON LED R LED2 Byte3
*  Byte 29 : TCON LED R LED2 Byte4
*  Byte 30 : TCON LED R LED2 Byte5
*  Byte 31 : TCON LED R LED2 Byte6
*  Byte 32 : TCON LED R LED3 Byte1 
*  Byte 33 : TCON LED R LED3 Byte2
*  Byte 34 : TCON LED R LED3 Byte3
*  Byte 35 : TCON LED R LED3 Byte4
*  Byte 36 : TCON LED R LED3 Byte5
*  Byte 37 : TCON LED R LED3 Byte6 
*  Byte 38 : TCON Fail Flag NACK
*  Byte 39 : TCON Fail Flag
*  Byte 40 : Source IC 0x58 NACK
*  Byte 41 : Source IC 0x58
*  Byte 42 : Source IC 0x59 NACK
*  Byte 43 : Source IC 0x59
*  Byte 44 : Source IC 0x5A NACK
*  Byte 45 : Source IC 0x5A
*  Byte 46 : Source IC 0x5B NACK
*  Byte 47 : Source IC 0x5B
*  Byte 48 : VCOM Fault NACK
*  Byte 49 : VCOM Fault
*/
static uint8_t u8ClientDiag[41] = {0};
#else
/*  
*  Byte 0 : TCON status1 NACK
*  Byte 1 : TCON status1 
*  Byte 2 : TCON status2 NACK
*  Byte 3 : TCON status2 
*  Byte 4 : TCON Fail Flag NACK
*  Byte 5 : TCON Fail Flag
*  Byte 6 : Source IC 0x58 NACK
*  Byte 7 : Source IC 0x58
*  Byte 8 : Source IC 0x59 NACK
*  Byte 9 : Source IC 0x59
*  Byte 10 : Source IC 0x5A NACK
*  Byte 11 : Source IC 0x5A
*  Byte 12 : Source IC 0x5B NACK
*  Byte 13 : Source IC 0x5B
*  Byte 14 : VCOM Fault NACK
*  Byte 15 : VCOM Fault
*/
static uint8_t u8ClientDiag[16] = {0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00};
#endif

static uint8_t u8LocalDim_Flag = NUM_ONE;  /* Default Value is Enable */

static uint8_t u8FirstDiag = NUM_ONE;


uint8_t I2c_Protocal_Get_Touch(void)
{
    return gUserTouchReset;
}

void I2c_Protocal_Set_Touch(uint8_t val)
{
    gUserTouchReset = val;
}

/*--------------------- Internal interface ---------------------------- */
void i2cMessagesInit(void)
{
    /* Flag for create other TASK */
    if( (MMIM_PowerState_e != ePowerState_AbnormalRun)&&
       (MMIM_PowerState_e != ePowerState_AbnormalRun_3V3)&&
           (MMIM_PowerState_e != ePowerState_AbnormalRun_5V0))
    {
        (void)memset(&i2c_messages, 0x00U, sizeof(i2c_messages));
    }
    else
    {
        // (void)memset(&i2c_messages.display_status.INIT, 0x00, sizeof(i2c_messages.display_status.INIT));
        i2c_messages.display_status.INIT = NUM_ZERO;
        (void)memset(&i2c_messages.lcd_backlight_pwm_value, 0x00U, sizeof(i2c_messages.lcd_backlight_pwm_value));/* 0x02 */
        (void)memset(&i2c_messages.display_scanning, 0x00U, sizeof(i2c_messages.display_scanning));/* 0x03 */
        (void)memset(&i2c_messages.interrupt_status_message, 0x00U, sizeof(i2c_messages.interrupt_status_message));/* 0x30 */
        (void)memset(&i2c_messages.display_enable, 0x00U, sizeof(i2c_messages.display_enable));/* 0x04 */
        (void)memset(&i2c_messages.display_shutdown, 0x00U, sizeof(i2c_messages.display_shutdown));/* 0x05 */
        
    }
    
    //(void)memset(&i2c_messages, 0x00U, sizeof(i2c_messages));
    
    const char *pCore_assembly = "RU5T-14F180-SA";
    const char *pSoftware_ford_part_number = "RU5T-14D358-SA";
    // const char *pMain_calibration_ford_part_number = "N1NT-14D359-AA";
    const char *pdelivery_assembly = "R2TT-10849-AA";
#if 1  
    i2c_messages.display_identification.Display_ID = 0x27;/*20210429 */
    i2c_messages.display_identification.Subrevision = 0x00;/*20210122*/
    
    (void)strcpy((char *)i2c_messages.core_assembly, pCore_assembly);
    //(void)memset(i2c_messages.delivery_assembly, 0xff, sizeof(i2c_messages.delivery_assembly));
    (void)strcpy((char *)i2c_messages.delivery_assembly, pdelivery_assembly);
    (void)strcpy((char *)i2c_messages.software_ford_part_number, pSoftware_ford_part_number);
    //(void)strncpy((char*)i2c_messages.software_ford_part_number,pSoftware_ford_part_number,sizeof("N1NT-14D358-AA"));
    (void)memset(i2c_messages.serial_number, 0xff, sizeof(i2c_messages.delivery_assembly));
    (void)memset(i2c_messages.main_calibration_ford_part_number, 0xff, sizeof(i2c_messages.main_calibration_ford_part_number));
    (void)memset(i2c_messages.client_specific_diagnostic_message, 0xff, sizeof(i2c_messages.client_specific_diagnostic_message));
    //(void)strcpy((char *)i2c_messages.main_calibration_ford_part_number, pMain_calibration_ford_part_number);
    //(void)strncpy((char*)i2c_messages.main_calibration_ford_part_number,pMain_calibration_ford_part_number,sizeof("N1NT-14D359-AA"));
    /*write error state to work flash when battery voltage get low*/
#endif 
#if 0 //test for disable/enable irq  
   __disable_irq();
#endif 
    
    //(void)memset(&u8FlashBuf[0], 0x00U, sizeof(u8FlashBuf));
    //memcpy(&u8FlashBuf[0],&i2c_messages.delivery_assembly,sizeof(i2c_messages.delivery_assembly));
    //(void)Flash_Work_Sector_Write(DeliveryAssemblyAddr, SIZE_WORK_FLASH_SSECTOR_BYTE,&u8FlashBuf[0], (sizeof(u8FlashBuf)/sizeof(uint8_t)));    
#if 0   //when first using work flash must be done => erase program then read
   Flash_Sector_Erase(DisplayIDAddr);
   Flash_Sector_Erase(CoreAssemblyAddr);
   Flash_Sector_Erase(DeliveryAssemblyAddr);
   Flash_Sector_Erase(MainCalibrationFordPartNumberAddr);
   Flash_Sector_Erase(SerialNumAddr);
   Flash_Sector_Erase(ErrorBackupAddr);
#endif
#if 0  //when first using work flash must be done => write FF
   (void)memset(&u8FlashBuf[0], 0xFFU, sizeof(u8FlashBuf));
   Flash_Work_Sector_Write(DisplayIDAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FlashBuf[0], (sizeof(u8FlashBuf)/sizeof(uint8_t)));
//   memcpy(&u8FlashBuf[0],&ucMcuVerU[0],10);
   Flash_Work_Sector_Write(CoreAssemblyAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FlashBuf[0], (sizeof(u8FlashBuf)/sizeof(uint8_t)));
//   (void)memset(&u8FlashBuf[0], 0x00U, sizeof(u8FlashBuf));
   Flash_Work_Sector_Write(DeliveryAssemblyAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FlashBuf[0], (sizeof(u8FlashBuf)/sizeof(uint8_t)));
   Flash_Work_Sector_Write(MainCalibrationFordPartNumberAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FlashBuf[0], (sizeof(u8FlashBuf)/sizeof(uint8_t)));
   Flash_Work_Sector_Write(SerialNumAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FlashBuf[0], (sizeof(u8FlashBuf)/sizeof(uint8_t)));
   Flash_Work_Sector_Write(ErrorBackupAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FlashBuf[0], (sizeof(u8FlashBuf)/sizeof(uint8_t)));
#endif 
    (void)memset(&u8FlashBuf[0], 0x00U, sizeof(u8FlashBuf));
    Flash_Work_Sector_Read(DisplayIDAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FlashBuf[0], (sizeof(u8FlashBuf)/sizeof(uint8_t)));   
    
    (void)memset(&u8FlashBuf[0], 0x00U, sizeof(u8FlashBuf));
    Flash_Work_Sector_Read(CoreAssemblyAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FlashBuf[0], (sizeof(u8FlashBuf)/sizeof(uint8_t)));
    //memcpy(&u8FlashBuf[0],&i2c_messages.core_assembly,sizeof(i2c_messages.core_assembly));
    //(void)Flash_Work_Sector_Write(CoreAssemblyAddr, SIZE_WORK_FLASH_SSECTOR_BYTE,&u8FlashBuf[0], (sizeof(u8FlashBuf)/sizeof(uint8_t)));
    
    (void)memset(&u8FlashBuf[0], 0x00U, sizeof(u8FlashBuf));
    Flash_Work_Sector_Read(DeliveryAssemblyAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FlashBuf[0], (sizeof(u8FlashBuf)/sizeof(uint8_t)));
    
    (void)memset(&u8FlashBuf[0], 0x00U, sizeof(u8FlashBuf));
    Flash_Work_Sector_Read(MainCalibrationFordPartNumberAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FlashBuf[0], (sizeof(u8FlashBuf)/sizeof(uint8_t)));
    
    /* leo 20220621 */
    (void)memset(&u8FlashBuf[0], 0x00U, sizeof(u8FlashBuf));
    Flash_Work_Sector_Read(SerialNumAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FlashBuf[0], (sizeof(u8FlashBuf)/sizeof(uint8_t)));
    
    (void)memset(&u8FlashBuf[0], 0x00U, sizeof(u8FlashBuf));
    Flash_Work_Sector_Read(ErrorBackupAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FlashBuf[0], (sizeof(u8FlashBuf)/sizeof(uint8_t)));
   
    
#if 0 //test for disable/enable irq  
   __enable_irq();
#endif 
}

int8_t MMIM_DisplayStatusRegister_Set(uint8_t *pReg, uint8_t ucShift, uint8_t ValSorce, uint8_t ucRegVal)
{
    uint32_t ucTempRegVal;
    
    if (ucShift >= DISP_STATUS_DATA1_SIZE)
    {
        return -1;
    }
    
    ucTempRegVal = *pReg;
    
    /*fill up Reg*/
    if (ucRegVal == ENABLE)
    {
        *pReg |= (ucRegVal << ucShift);
    }
    /*if while 0x00 have been read -> Clear Reg. resource must be i2c.*/
    else if (ucRegVal == DISABLE && ValSorce == SOUREFROMI2C)
    {
        *pReg &= ~((uint8_t)orbit1Mask << ucShift);
    }
    else
    {
        /**/
    }
    
    /*if state change, FaultFlag = 1*/
    if ((ucTempRegVal != (*pReg)))
    {
        MMIM_InteruptStutsReg_Ctrl(MMIM_DisplayStateChangeEvent, eINT_ERR, LEVEL_HIGH);
        u8INITActiveFlag = ENABLE;
    }
    
    return 0;
}

int8_t MMIM_InteruptStutsReg_Set(uint8_t *pReg, uint8_t ucShift, uint8_t ValSorce, uint8_t ucRegVal)
{
    
    //uint32_t ucTempRegVal;
    
    /*Register size check*/
    if (ucShift >= InteruptStatusRegistorSize)
    {
        return -1;
    }
    
    //ucTempRegVal = *pReg;
    
    /*fill up Reg*/
    if (ucRegVal == ENABLE)
    {
        *pReg |= (ucRegVal << ucShift);
    }
    /*if while 0x00 have been read -> Clear Reg.*/
    else if (ucRegVal == DISABLE && ValSorce == SOUREFROMI2C)
    {
        *pReg &= ~((uint8_t)orbit1Mask << ucShift);
    }
    else
    {
        /**/
    }
#if 0
    /*michael disable,20220818*/
    /*if state change, u8INITActiveFlag = 1*/
    if (((ucTempRegVal == 0U) && ((*pReg) == 1U)))
    {
        
        u8INITActiveFlag = ENABLE;
    }
    else
    {
        ;
    }
#endif
    return 0;
}

/*---------------- Provided External Interface---------------- */
uint8_t MMIM_DelAssmPNStatusRegFlag_Ctrl (uint8_t u8status)
{
    switch (u8status)
    {
    case GETFLAG:
        
        break;
        
    default:
        u8WriteDelAssmPNStatusRegFlag = u8status;
        break;
    }
    
    return u8WriteDelAssmPNStatusRegFlag;
}

uint8_t MMIM_CoreAssmPNStatusRegFlag_Ctrl (uint8_t u8status)
{
    switch (u8status)
    {
    case GETFLAG:
        
        break;
        
    default:
        u8WriteCoreAssmPNStatusRegFlag = u8status;
        break;
    }
    
    return u8WriteCoreAssmPNStatusRegFlag;
}

uint8_t MMIM_MainCalibPNStatusRegFlag_Ctrl (uint8_t u8status)
{
    switch (u8status)
    {
    case GETFLAG:
        
        break;
        
    default:
        u8WriteMainCalibPNStatusRegFlag = u8status;
        break;
    }
    
    return u8WriteMainCalibPNStatusRegFlag;
}

uint8_t MMIM_DispIDPNStatusRegFlag_Ctrl (uint8_t u8status)
{
    switch (u8status)
    {
    case GETFLAG:
        
        break;
        
    default:
        u8WriteDispIDPNStatusRegFlag = u8status;
        break;
    }
    
    return u8WriteDispIDPNStatusRegFlag;
}

uint8_t MMIM_SoftwarePNStatusRegFlag_Ctrl (uint8_t u8status)
{
    switch (u8status)
    {
    case GETFLAG:
        
        break;
        
    default:
        u8WriteSoftwareFPNStatusRegFlag = u8status;
        break;
    }
    
    return u8WriteSoftwareFPNStatusRegFlag;
}

/* leo 20220621 */
uint8_t MMIM_SerialPNStatusRegFlag_Ctrl (uint8_t u8status)
{
    switch (u8status)
    {
    case GETFLAG:
        
        break;
        
    default:
        u8WriteSerialNumFPNStatusRegFlag = u8status;
        break;
    }
    
    return u8WriteSerialNumFPNStatusRegFlag;
}

uint8_t MMIM_WriteDelAssmFPNFlag_Ctrl (uint8_t u8status)
{
    switch (u8status)
    {
    case GETFLAG:
        
        break;
        
    default:
        u8DelAssmFPNFlag = u8status;
        break;
    }
    
    return u8DelAssmFPNFlag;
}

uint8_t MMIM_WriteCoreAssmFPNFlag_Ctrl (uint8_t u8status)
{
    switch (u8status)
    {
    case GETFLAG:
        
        break;
        
    default:
        u8CoreAssmFPNFlag = u8status;
        break;
    }
    
    return u8CoreAssmFPNFlag;
}

uint8_t MMIM_WriteMainCalibFPNFlag_Ctrl (uint8_t u8status)
{
    switch (u8status)
    {
    case GETFLAG:
        
        break;
        
    default:
        u8MainCalibFPNFlag = u8status;
        break;
    }
    
    return u8MainCalibFPNFlag;
}

uint8_t MMIM_WriteDispIDFPNFlag_Ctrl (uint8_t u8status)
{
    switch (u8status)
    {
    case GETFLAG:
        
        break;
        
    default:
        u8DispIDFPNFlag = u8status;
        break;
    }
    
    return u8DispIDFPNFlag;
}

uint8_t MMIM_WriteSoftwareFPNFlag_Ctrl (uint8_t u8status)
{
    switch (u8status)
    {
    case GETFLAG:
        
        break;
        
    default:
        u8SoftwareFPNFlag = u8status;
        break;
    }
    
    return u8SoftwareFPNFlag;
}

/* leo 20220621 */
uint8_t MMIM_WriteSerialFPNFlag_Ctrl (uint8_t u8status)
{
    switch (u8status)
    {
    case GETFLAG:
        
        break;
        
    default:
        u8SerialNumFPNFlag = u8status;
        break;
    }
    
    return u8SerialNumFPNFlag;
}

uint8_t MMIM_BISTModeFlag_Ctrl (uint8_t u8status)
{
    switch (u8status)
    {
    case GETFLAG:
        
        break;
        
    default:
        u8BISTModeFlag = u8status;
        break;
    }
    
    return u8BISTModeFlag;
}

uint8_t *MMIM_pdelivery_assembly_Get (void)
{
    return u8DelAssmFPN;
}

uint8_t *MMIM_pcore_assembly_Get (void)
{
    return u8CoreAssmFPN;
}

uint8_t *MMIM_pmain_calibration_Get (void)
{
    return u8MainCalibFPN;
}

uint8_t *MMIM_pdisplay_id_Get (void)
{
    return u8DispIDFPN;
}

uint8_t *MMIM_psoftware_fpn_Get (void)
{
    return u8SoftwareFPN;
}

/* leo 20220621 */
uint8_t *MMIM_pserial_number_Get (void)
{
    return u8SerialNumFPN;
}

uint8_t MMIM_PwrInitStatus_Ctrl(uint8_t u8Status)
{
    switch (u8Status)
    {
    case GETFLAG:
        break;
        
    default:
        u8PowerInitStatus = u8Status;
        break;
    }
    
    return u8PowerInitStatus;
}

uint8_t MMIM_JumpToBootCodeFlag_Ctrl(uint8_t u8FlagStatus)
{
    switch (u8FlagStatus)
    {
    case GETFLAG:
        /* code */
        break;
        
    default:
        
        u8JumpToBootCodeFlag = u8FlagStatus;
        
        break;
    }
    
    return u8JumpToBootCodeFlag;
}

uint8_t MMIM_FactoryFlag_Ctrl(uint8_t FlagState)
{
    switch (FlagState)
    {
    case GETFLAG /* constant-expression */:
        /* code */
        break;
        
    default:
        u8FactoryEn = FlagState;
        break;
    }
    
    return u8FactoryEn;
}

uint8_t MMIM_ShutdownFlag_Ctrl(uint8_t FlagState)
{
    switch (FlagState)
    {
    case GETFLAG /* constant-expression */:
        /* code */
        break;
        
    default:
        u8ShutdownFlag = FlagState;
        break;
    }
    
    return u8ShutdownFlag;
}

uint8_t MMIM_DispENFlag_Ctrl(uint8_t FlagState)
{
    switch (FlagState)
    {
    case GETFLAG /* constant-expression */:
        /* code */
        break;
        
    default:
        u8DispENFlag = FlagState;
        break;
    }
    
    return u8DispENFlag;
}

uint8_t MMIM_BLPWMFlag_Ctrl(uint8_t FlagState)
{
    switch (FlagState)
    {
    case GETFLAG /* constant-expression */:
        /* code */
        break;
        
    default:
        u8BLPWMFlag = FlagState;
        break;
    }
    
    return u8BLPWMFlag;
}

uint8_t MMIM_ScanDirectionFlag_Ctrl(uint8_t FlagState)
{
    switch (FlagState)
    {
    case GETFLAG /* constant-expression */:
        /* code */
        break;
        
    default:
        u8ScanDirectionFlag = FlagState;
        break;
    }
    
    return u8ScanDirectionFlag;
}

uint8_t MMIM_DiagnosticMessageFlag_Ctrl(uint8_t FlagState)
{
    switch (FlagState)
    {
    case GETFLAG /* constant-expression */:
        /* code */
        break;
        
    default:
        u8DiagnosticMessageFlag = FlagState;
        break;
    }
    
    return u8DiagnosticMessageFlag;
}

Global_PowerState_E MMIM_PowerState_Ctrl(Global_PowerState_E PowerState_e)
{
    switch (PowerState_e)
    {
    case ePowerstate_Get:
        break;
        
    default:
        
        MMIM_PowerState_e = PowerState_e;
        
        break;
    }
    
    return MMIM_PowerState_e;
}

I2C_MESSAGES_T *MMIM_pI2CMessage_Get(void)
{
    I2C_MESSAGES_T *pi2c_message_t;
    
    pi2c_message_t = &i2c_messages;
    
    return pi2c_message_t;
}

void MMIM_Check_Power_Error(uint8_t status)
{
    switch (status)
    {
    case PIN_LM61460_PG:
        ucErrorPower |= (uint8_t)ERROR_PGOOD_LM61460;
        break;
    case PIN_MAX20419_PG:
        ucErrorPower |= (uint8_t)ERROR_PGOOD_MAX20419;
        break;
    case PIN_MAX25221_FLTB:
        ucErrorPower |= (uint8_t)ERROR_PGOOD_MAX25221_FLTB;
        break;
    case PIN_984_LOCK:
        ucErrorPower |= (uint8_t)ERROR_PIN_984_LOCK;
        break;
    case PIN_PANEL_ABD:
        ucErrorPower |= (uint8_t)ERROR_PIN_PANEL_ABD;
        break;
    case PIN_25210_RESET_R:
        ucErrorPower |= (uint8_t)ERROR_PIN_25210_RESET_R;
        break;
    case PIN_25210_RESET_L:
        ucErrorPower |= (uint8_t)ERROR_PIN_25210_RESET_L;
        break;
    default:
        break;
    }
}

#if 1
/*
uint8_t MMIM_FactoryEn_Get(void)
{
    return u8FactoryEn;
}*/
#endif

uint8_t MMIM_DeratingFlag_Get(void)
{
    return u8DeratingEN;
}

uint8_t MMIM_ErrorCount_Add(uint8_t u8Index)
{
    return gErrorCounter[u8Index]++;
}

uint8_t MMIM_ErrorCount_Get(uint8_t u8Index)
{
    return gErrorCounter[u8Index];
}

void *MMIM_ErrorCountArray_Get(void)
{
    void *pTemp = NULL;
    
    pTemp = gErrorCounter;
    
    return pTemp;
}

uint8_t MMIM_INTBActiveFlag_Ctrl(uint8_t u8FlagState)
{
    switch (u8FlagState)
    {
    case GETFLAG:
        break;
        
    default:
        u8INITActiveFlag = u8FlagState;
        break;
    }
    
    return u8INITActiveFlag;
}

Global_LockState_E MMIM_LockStatus_Get(void)
{
    return (Global_LockState_E)i2c_messages.display_status.LLOSS;
}

uint8_t MMIM_LockPinStatus_Ctrl (uint8_t u8Status)
{
    switch (u8Status)
    {
    case GETFLAG:
        break;
        
    default:
        u8LockPinStatus = u8Status;
        break;
    }
    
    return u8LockPinStatus;
}

uint8_t  MMIM_SourceDriver_Ctrl(uint8_t u8FlagState)
{
    switch (u8FlagState)
    {
    case GETFLAG:
        break;
        
    default:
        u8sourceDriver_rest_flag = u8FlagState;
        break;
    }
    
    return u8sourceDriver_rest_flag;
}

uint16_t  MMIM_PWMTargetValue_Ctrl(uint16_t u16PWMState)
{
    switch (u16PWMState)
    {
    case GETPWMFLAG:
        break;
        
    default:
        u10BitPWMValue = u16PWMState;
        break;
    }
    
    return u10BitPWMValue;
}

uint8_t  MMIM_VolRestRqFlag_Ctrl(uint8_t u8FlagState)
{
    switch (u8FlagState)
    {
    case GETFLAG:
        break;
        
    default:
        u8VolRestRq_flag = u8FlagState;
        break;
    }
    
    return u8VolRestRq_flag;
}

void MMIM_DisplayStatusReg_Ctrl(uint8_t DataSource, DisplayReg0x00_E eReg, uint8_t RegState)
{
    uint8_t *pDisplayStateReg;
    uint8_t *pDisplayStateReg2;
    uint8_t ShiftVal = 0U;
    uint8_t ShiftIdex = 0U;
    
    (void)ShiftVal;
    pDisplayStateReg = ((uint8_t *)&i2c_messages.display_status);
    pDisplayStateReg2 = ((uint8_t *)&i2c_messages.display_status) + 1; 
    switch (eReg)
    {
    case eLCDERR:
        ShiftVal = 0;
        break;
    case eLCD_BL_Fault:
        ShiftVal = 1U;
        break;
    case eTemperatureERR:
        ShiftVal = 2U;
        break;
    case eDisconnecterror:
        ShiftVal = 3U;
        break;
    case eResetRequest:
        ShiftVal = 4U;
        break;
    case elossoflock:
        ShiftVal = 5U;
        break;
    case eTouchScreenControllerError:
        ShiftVal = 6U;
        break;
    case eTouchConnectionError:
        ShiftVal = 7U;
        break;
        
    case eDisplayStatus:
        pDisplayStateReg = ((uint8_t *)&i2c_messages.display_status) + 1;
        ShiftVal = 0U;
        break;
    case eTouchControllerStatus:
        pDisplayStateReg = ((uint8_t *)&i2c_messages.display_status) + 1;
        ShiftVal = 1U;
        break;
    case eDisplayInitialized:
        pDisplayStateReg = ((uint8_t *)&i2c_messages.display_status) + 1;
        ShiftVal = 2U;
        break;
    case eHighPriorityError:
        pDisplayStateReg = ((uint8_t *)&i2c_messages.display_status) + 1;
        ShiftVal = 3U;
        break;
    case eMediumPriorityError:
        pDisplayStateReg = ((uint8_t *)&i2c_messages.display_status) + 1;
        ShiftVal = 4U;
        break;
    case eLowPriorityError:
        pDisplayStateReg = ((uint8_t *)&i2c_messages.display_status) + 1;
        ShiftVal = 5U;
        break;
    case eBacklightStatus:
        pDisplayStateReg = ((uint8_t *)&i2c_messages.display_status) + 1;
        ShiftVal = 6U;
        break;
    case eDisplayError:
        pDisplayStateReg = ((uint8_t *)&i2c_messages.display_status) + 1;
        ShiftVal = 7U;
        break;   
        
    case eALLReg:
        
        ShiftVal = eALLReg;
        
        break;
    default:
        
        ShiftVal = 0U;
        
        break;
    }
    
    if (ShiftVal == eALLReg)
    {
        for (ShiftIdex = 0U; ShiftIdex < DisplayStatusLatchBitSize; ShiftIdex++)
        {
            (void)MMIM_DisplayStatusRegister_Set(pDisplayStateReg, LatchBitRegShift[ShiftIdex], DataSource, RegState);
        }
        for (ShiftIdex = 0U; ShiftIdex < DisplayStatusLatchBit2Size; ShiftIdex++)
        {  
            (void)MMIM_DisplayStatusRegister_Set(pDisplayStateReg2, LatchBitRegShift2[ShiftIdex], DataSource, RegState);
        }
        
    }
    else
    {
        (void)MMIM_DisplayStatusRegister_Set(pDisplayStateReg, ShiftVal, DataSource, RegState);
    }
}

void MMIM_InteruptStutsReg_Ctrl(uint8_t DataSource, interrupt_status_message_E eISR, uint8_t RegState)
{
    uint8_t *pDisplayStateReg;
    uint8_t ShiftVal = 0U;
    
    (void)ShiftVal;
    pDisplayStateReg = ((uint8_t *)&i2c_messages.interrupt_status_message);
    
    switch (eISR)
    {
    case eINT_ERR:
        ShiftVal = 0U;
        break;
    case eINT_BTN:
        ShiftVal = 1U;
        break;
    case eINT_TCH:
        ShiftVal = 2U;
        break;
    case eINT_ROT:
        ShiftVal = 3U;
        break;
    case eINT_ALL:
        ShiftVal = InteruptStatusRegistorSize;
        break;
    default:
        ShiftVal = InteruptStatusRegistorSize;
        break;
    }
    
    if (ShiftVal == InteruptStatusRegistorSize)
    {
        for (ShiftVal = 0; ShiftVal < InteruptStatusRegistorSize; ShiftVal++)
        {
            (void)MMIM_InteruptStutsReg_Set(pDisplayStateReg, ShiftVal, DataSource, RegState);
        }
    }
    else
    {
        (void)MMIM_InteruptStutsReg_Set(pDisplayStateReg, ShiftVal, DataSource, RegState);
    }
}

void MMIM_PCBNTCADC_Set (uint16_t u16NADCValue)
{
    
    u16PCBNtcAdcVaules = u16NADCValue;
}

uint16_t MMIM_PCBNTCADC_Get(void)
{
    return u16PCBNtcAdcVaules;
}

void MMIM_BLNTCADC_Set(uint16_t u16NADCValue)
{
    
    u16BacklightNtcAdcVaules = u16NADCValue;
}

uint16_t MMIM_BLNTCADC_Get(void)
{
    return u16BacklightNtcAdcVaules;
}

void MMIM_PCBTemperatureVal_Set(uint8_t u8Sign ,uint16_t u16Temperature,uint8_t u8DataSource)
{
    NTC_Buffer[BYTE0].ucSign = u8Sign;
    NTC_Buffer[BYTE0].TempVal_H = (u16Temperature >> 8U) & MASK_8_BIT;
    NTC_Buffer[BYTE0].TempVal_L = u16Temperature & MASK_8_BIT ;
    NTC_Buffer[BYTE0].DataSource = u8DataSource ;    
}
void MMIM_BLTemperatureVal_Set(uint8_t u8Sign ,uint16_t u16Temperature,uint8_t u8DataSource)
{
    NTC_Buffer[BYTE1].ucSign = u8Sign;
    NTC_Buffer[BYTE1].TempVal_H = (u16Temperature >> 8U) & MASK_8_BIT;
    NTC_Buffer[BYTE1].TempVal_L = u16Temperature & MASK_8_BIT ;
    NTC_Buffer[BYTE1].DataSource = u8DataSource ;    
}

void MMIM_FactoryFlag_Set(uint8_t u8Flag)
{
    i2c_messages.factory_status.FACTORY_EN = u8Flag;
}

uint8_t MMIM_FactoryFlag_Get(void)
{
    return i2c_messages.factory_status.FACTORY_EN;
}

/* Battery voltage Data ctrl */
void MMIM_BattVolVal_Set(uint16_t u16Val)
{
    u16BatteryVoltageValue = u16Val;
}

uint16_t MMIM_BattVolVal_Get(void)
{
    return u16BatteryVoltageValue;
}

void MMIM_BattVolADCVal_Set(uint16_t u16ADCVal)
{
    uBatteryVoltAdcValue = u16ADCVal;
}

uint16_t MMIM_BattVolADCVal_Get(void)
{
    return uBatteryVoltAdcValue;
}

USER_BIST_MODE_CTRL_T MMIM_BISTData_Get(void)
{
    return u8BIST_DATA_t;
}

uint8_t MMIM_DataLength_Get(uint8_t subAddr)
{
    // 2.3.6.6 IFS-MMI2C-SR-REQ-197875/B-Read from Subaddress Beyond Defined Length
    // --> set the length for Slave TX Buffer. The driver will return 0xFF for the bytes beyoud defined length
    // 2.3.6.7 IFS-MMI2C-SR-REQ-140565/C-Undefined / Unsupported Subaddress
    // --> return length = 0. The driver will return 0xFF for the bytes beyoud defined length.
    uint8_t length = 0;
    switch (subAddr)
    {
    case DISPLAY_STATUS:
        length = (uint8_t)sizeof(i2c_messages.display_status);
        break;
    case DISPLAY_IDENTIFICATION:
        length = (uint8_t)sizeof(i2c_messages.display_identification);
        break;
    case LCD_BACKLIGHT_PWM_VALUE:
        length = (uint8_t)sizeof(i2c_messages.lcd_backlight_pwm_value);
        break;
    case DISPLAY_SCANNING:
        length = (uint8_t)sizeof(i2c_messages.display_scanning);
        break;
    case DISPLAY_ENABLE:
        length = (uint8_t)sizeof(i2c_messages.display_enable);
        break;
    case DISPLAY_SHUTDOWN:
        length = (uint8_t)sizeof(i2c_messages.display_shutdown);
        break;
    case INTERRUPT_STATUS_MESSAGE:
        length = (uint8_t)sizeof(i2c_messages.interrupt_status_message);
        break;
    case CORE_ASSEMBLY:
        length = (uint8_t)sizeof(i2c_messages.core_assembly);
        break;
    case DELIVERY_ASSEMBLY:
        length = (uint8_t)sizeof(i2c_messages.delivery_assembly);
        break;
    case SOFTWARE_FORD_PART_NUMBER:
        length = (uint8_t)sizeof(i2c_messages.software_ford_part_number);
        break;
    case SERIAL_NUMBER:
        length = (uint8_t)sizeof(i2c_messages.serial_number);
        break;
    case MAIN_CALIBRATION_FORD_PART_NUMBER:
        length = (uint8_t)sizeof(i2c_messages.main_calibration_ford_part_number);
        break;
    case CLIENT_SPECIFIC_DIAGNOSTIC_MESSAGE:
        length = (uint8_t)sizeof(i2c_messages.client_specific_diagnostic_message);
        break;  
    case JUMP_BOOTLOADER:    //20211028 Joel
        length = (uint8_t)sizeof(i2c_messages.jump_bootloader);
        break;
    case USER_Temputer:
        length = (uint8_t)sizeof(NTC_Buffer);
        break;
    case USER_Temputer2:
        length = 4;
        break;
    case USER_FACTORYMODE_EN:
        length = (uint8_t)sizeof(i2c_messages.factory_status);
        break;
    case POWER_STATUS:
        length = 1U;
        break;
    case USER_ShowErrorState:
        length = 16;
        break;
    case USER_BatteryVoltage:
        length = 4;
        break;
    case USER_ResetReq:
        length = 1U;
        break;
    case USER_WatchDogToogle_EN:
        length = (uint8_t)sizeof(&WatchDogToogle_EN);
        break;
    case CMD_MODE:
        length = (uint8_t)sizeof(gApMode);
        break;
    case CMD_GET_STATE:
        length = 1;
        break;
    case CMD_GET_VERSION:
        length = 11;
        break;
    case USER_GetNowPWMDuty:
        length = (uint8_t)sizeof(u10BitPWMValue);
        break;
    case USER_BIST_Crtl:
        length = 1;
        break;        
    case PN_DelAssmStatus_Ctrl:
        length = (uint8_t)sizeof(i2c_messages.pn_delivery_assembly_status_ctrl);
        break;
    case PN_DelAssmblyFPN:
        length = (uint8_t)sizeof(u8DelAssmFPN);
        break;
    case PN_CoreAssmStatus_Ctrl:
        length = (uint8_t)sizeof(i2c_messages.pn_core_assembly_status_ctrl);
        break;
    case PN_CoreAssmblyFPN:
        length = (uint8_t)sizeof(u8CoreAssmFPN);
        break;
    case PN_MainCalibStatus_Ctrl:
        length = (uint8_t)sizeof(i2c_messages.pn_main_calibration_status_ctrl);
        break;
    case PN_MainCalibFPN:
        length = (uint8_t)sizeof(u8MainCalibFPN);
        break;
    case  PN_DispIDStatus_Ctrl:
        length = (uint8_t)sizeof(i2c_messages.pn_display_identification_status_ctrl);
        break;
    case PN_DispIDFPN:
        length = (uint8_t)sizeof(u8DispIDFPN);
        break;
    case PN_SwFPNStatus_Ctrl:
        length = (uint8_t)sizeof(i2c_messages.pn_software_ford_part_number_status_ctrl);
        break;
    case PN_SoftwareFPN:
        length = (uint8_t)sizeof(u8SoftwareFPN);
        break;
    case USER_VCOM:
        length = (uint8_t)sizeof(u8VCOM);
        break;
        
        /* leo 2021_12_14 */
    case USER_Get_Disp_Voltage:
        length = (uint8_t)sizeof(u8DispVoltSatus);
        break;
        
        /* leo 2021_12_14 */
    case USER_Get_LED_Voltage:
        length = (uint8_t)sizeof(u8LedVoltStatus);
        break;
        
    case CMD_GET_Bootloader_Status:  /* 20220708 Taylor */
        length = (uint8_t)sizeof(i2c_messages.jump_bootloader);
        break;
        
    default:
        break;
    }
    return length;
}

void MMIM_MessageData_Get(uint8_t subAddr, uint8_t *dataBuffer)
{
    //2.3.6.8 IFS-MMI2C-SR-REQ-140611/C-Reserved Bits --> MASK BIT
    *dataBuffer = subAddr;
    
    switch (subAddr)
    {
    case DISPLAY_STATUS:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.display_status, sizeof(i2c_messages.display_status));
        dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_8_BIT;
        
        /*Clear Reg. 0x30*/
        MMIM_InteruptStutsReg_Ctrl(SOUREFROMI2C, eINT_ERR, NUM_ZERO);
        
        /*Clear Reg. 0x00*/
        
        MMIM_DisplayStatusReg_Ctrl(SOUREFROMI2C, eALLReg, NUM_ZERO);
        
        break;
        /*2.2.7.3 IFS-MMI2C-SR-REQ-140616/I-Display Identification Table*/
    case DISPLAY_IDENTIFICATION:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.display_identification, sizeof(i2c_messages.display_identification));
        break;
    case LCD_BACKLIGHT_PWM_VALUE:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.lcd_backlight_pwm_value, sizeof(i2c_messages.lcd_backlight_pwm_value));
        dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_2_BIT;
        break;
    case DISPLAY_SCANNING:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.display_scanning, sizeof(i2c_messages.display_scanning));
        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_2_BIT;
        break;
    case DISPLAY_ENABLE:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.display_enable, sizeof(i2c_messages.display_enable));
        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;
        break;
    case DISPLAY_SHUTDOWN:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.display_shutdown, sizeof(i2c_messages.display_shutdown));
        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;
        break;
    case INTERRUPT_STATUS_MESSAGE:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.interrupt_status_message, sizeof(i2c_messages.interrupt_status_message));
        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_4_BIT;
        /*CLear u8INITActiveFlag when 0x30 has been read */
        u8INITActiveFlag = DISABLE;
        break;
    case CORE_ASSEMBLY:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.core_assembly, sizeof(i2c_messages.core_assembly));
        break;
    case DELIVERY_ASSEMBLY:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.delivery_assembly, sizeof(i2c_messages.delivery_assembly));
        break;
    case SOFTWARE_FORD_PART_NUMBER:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.software_ford_part_number, sizeof(i2c_messages.software_ford_part_number));
        break;
    case SERIAL_NUMBER:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.serial_number, sizeof(i2c_messages.serial_number));
        break;
    case MAIN_CALIBRATION_FORD_PART_NUMBER:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.main_calibration_ford_part_number, sizeof(i2c_messages.main_calibration_ford_part_number));
        break;    
    case CLIENT_SPECIFIC_DIAGNOSTIC_MESSAGE:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8ClientDiag[0], sizeof(u8ClientDiag));
        break;
    case JUMP_BOOTLOADER:   //20211028 Joel
        (void)memcpy(&dataBuffer[0], &i2c_messages.jump_bootloader, sizeof(i2c_messages.jump_bootloader));
        break;
    case USER_Reset:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.display_status, sizeof(i2c_messages.display_status));
        dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_BIT6_0;
        break;
        /*
        **USER_Temputer 0xB2
        **Length 5
        **Byte1 PCB NTC sign byte 
        **Byte2 PCB NTC temp. high byte 
        **Byte3 PCB NTC temp. low byte 
        
        */
    case USER_Temputer:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &NTC_Buffer, sizeof(NTC_Buffer)); /*copy BL Temperuture*/
        
        break;
        /*
        **USER_Temputer2 0xB3
        **Length 5
        **Byte1 PCB NTC ADC low byte 
        **Byte2 PCB NTC ADC high byte 
        **Byte3 BL NTC ADC low byte 
        **Byte4 BL NTC ADC high byte 
        */
    case USER_Temputer2:
        
        if (u16PCBNtcAdcVaules != NULL)
        {
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u16PCBNtcAdcVaules, sizeof(u16PCBNtcAdcVaules)); /*copy BL Temperuture*/
            (void)memcpy(&dataBuffer[THIR_DATA_BYTE], &u16BacklightNtcAdcVaules, sizeof(u16BacklightNtcAdcVaules)); /*copy BL Temperuture*/
        }
        else
        {
            ;
        }
        break;
    case USER_FACTORYMODE_EN:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.factory_status, sizeof(i2c_messages.factory_status)); /*for Factory Mode*/
        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;
        break;
    case USER_ResetReq:
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8LowVolResetReq, sizeof(u8LowVolResetReq));
        break;
    case CMD_MODE:
        (void)memset(dataBuffer, 0, sizeof(dataBuffer)); /*for bootloader*/
        if (gApMode[0] != NULL)
        {
            (void)memcpy(dataBuffer, &gApMode, sizeof(gApMode));
        }
        else
        {
            ;
        }
        break;
    case CMD_GET_STATE:
        dataBuffer[0] = 0;
        break;
    case CMD_GET_VERSION:
        //        (void)memset(dataBuffer, 0, sizeof(dataBuffer)); /*for bootloader*/
        if (gMcuVerReturn[0] != NULL)
        {
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &gMcuVerReturn, sizeof(gMcuVerReturn));
        }
        else
        {
            ;
        }
        
        break;
    case POWER_STATUS:
        /*for power status*/
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &ucErrorPower, sizeof(ucErrorPower));
        break;
    case USER_ShowErrorState:
        
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], gReadBackErrorCounter, sizeof(gReadBackErrorCounter));
        
        break;
        
    case USER_BatteryVoltage:
        
        dataBuffer[FIRST_DATA_BYTE] = (uint8_t)u16BatteryVoltageValue & 0xFFU;
        dataBuffer[SECOND_DATA_BYTE] =(uint8_t)((u16BatteryVoltageValue >> 8U) & 0xFFU);
        dataBuffer[THIR_DATA_BYTE] = (uint8_t)(uBatteryVoltAdcValue & 0xFFU);
        dataBuffer[FOUR_DATA_BYTE] = (uint8_t)((uBatteryVoltAdcValue >> 8U) & 0xFFU);
        
        break;
    case USER_WatchDogToogle_EN:
        //        (void)memset(dataBuffer, 0, sizeof(dataBuffer)); /*for bootloader*/
        if (WatchDogToogle_EN != NULL)
        {
            
            (void)memcpy(dataBuffer, &WatchDogToogle_EN, sizeof(&WatchDogToogle_EN));
        }
        else
        {
            ;
        }
        
        break;
    case USER_GetNowPWMDuty:
        dataBuffer[FIRST_DATA_BYTE] = (uint8_t)u10BitPWMValue & 0xFFU;
        dataBuffer[SECOND_DATA_BYTE] =(uint8_t)((u10BitPWMValue >> 8U) & MASK_2_BIT);
        
        break;        
    case PN_DelAssmStatus_Ctrl:
        
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.pn_delivery_assembly_status_ctrl, sizeof(i2c_messages.pn_delivery_assembly_status_ctrl));
        
        i2c_messages.pn_delivery_assembly_status_ctrl.INT_WRT = DISABLE;
        i2c_messages.pn_delivery_assembly_status_ctrl.CKSUM_ERR = DISABLE;
        
        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_3_BIT;
        
        break;
    case PN_DelAssmblyFPN:
        
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], u8DelAssmFPN, sizeof(u8DelAssmFPN));
        
        break;
    case PN_CoreAssmStatus_Ctrl:
        
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.pn_core_assembly_status_ctrl, sizeof(i2c_messages.pn_core_assembly_status_ctrl));
        
        i2c_messages.pn_core_assembly_status_ctrl.INT_WRT = DISABLE;
        i2c_messages.pn_core_assembly_status_ctrl.CKSUM_ERR = DISABLE;
        
        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_3_BIT;
        
        break;
    case PN_CoreAssmblyFPN:
        
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], u8CoreAssmFPN, sizeof(u8CoreAssmFPN));
        
        break;   
    case PN_MainCalibStatus_Ctrl:
        
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.pn_main_calibration_status_ctrl, sizeof(i2c_messages.pn_main_calibration_status_ctrl));
        
        i2c_messages.pn_main_calibration_status_ctrl.INT_WRT = DISABLE;
        i2c_messages.pn_main_calibration_status_ctrl.CKSUM_ERR = DISABLE;
        
        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_3_BIT;
        
        break;
    case PN_MainCalibFPN:
        
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], u8MainCalibFPN, sizeof(u8MainCalibFPN));
        
        break;    
    case PN_DispIDStatus_Ctrl:
        
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.pn_display_identification_status_ctrl, sizeof(i2c_messages.pn_display_identification_status_ctrl));
        
        i2c_messages.pn_display_identification_status_ctrl.INT_WRT = DISABLE;
        i2c_messages.pn_display_identification_status_ctrl.CKSUM_ERR = DISABLE;
        
        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_3_BIT;
        
        break;
    case PN_DispIDFPN:
        
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], u8DispIDFPN, sizeof(u8DispIDFPN));
        
        break;
    case PN_SwFPNStatus_Ctrl:
        
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.pn_software_ford_part_number_status_ctrl, sizeof(i2c_messages.pn_software_ford_part_number_status_ctrl));
        
        i2c_messages.pn_software_ford_part_number_status_ctrl.INT_WRT = DISABLE;
        i2c_messages.pn_software_ford_part_number_status_ctrl.CKSUM_ERR = DISABLE;
        
        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_3_BIT;
        
        break;
    case PN_SoftwareFPN:
        
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], u8SoftwareFPN, sizeof(u8SoftwareFPN));
        
        break;   
    case USER_BIST_Crtl:
        
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8BIST_DATA_t, sizeof(u8BIST_DATA_t));
        
        break;
        
        /* leo 2021_12_14 */
    case USER_VCOM:
        
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8VCOM, sizeof(u8VCOM));
        
        break;
        
        /* leo 2021_12_14 */
    case USER_Get_Disp_Voltage:
        
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8DispVoltSatus, sizeof(u8DispVoltSatus));
        
        break;
        
        /* leo 2021_12_14 */
    case USER_Get_LED_Voltage:
        
        (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8LedVoltStatus, sizeof(u8LedVoltStatus));    
        
        break;  
        
    case CMD_GET_Bootloader_Status: /* 20220708 Taylor */
        (void)memcpy(&dataBuffer[0], &i2c_messages.jump_bootloader, sizeof(i2c_messages.jump_bootloader));
        if(0x43 == i2c_messages.jump_bootloader[2])
        {
            DEBUG_PF("Jump Bootloader\r\n");
            MMIM_JumpToBootCodeFlag_Ctrl(ENABLE);
        }
        break;
    default:
        break;
    }
}

void MMIM_I2cMessages_Set(uint8_t subAddr, uint8_t *dataBuffer, uint32_t dataLength)
{
    
    switch (subAddr)
    {
    case LCD_BACKLIGHT_PWM_VALUE:
        if (dataLength == sizeof(i2c_messages.lcd_backlight_pwm_value) + SUB_ADDRESS_SIZE)
        {
            dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_2_BIT;
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                
                (void)memcpy(&i2c_messages.lcd_backlight_pwm_value, &dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.lcd_backlight_pwm_value));
                /* Set Flag to send Event */
                (void)MMIM_BLPWMFlag_Ctrl(ENABLE);
            }
            else
            {
                ;
            }
        }
        break;
    case DISPLAY_SCANNING:
        if (dataLength == sizeof(i2c_messages.display_scanning) + SUB_ADDRESS_SIZE)
        {
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_2_BIT;
            if (dataBuffer != NULL)
            {
                
                (void)memcpy(&i2c_messages.display_scanning, &dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.display_scanning));
                /* Set Flag to send Event */
                (void)MMIM_ScanDirectionFlag_Ctrl(ENABLE);
            }
            else
            {
                ;
            }
        }
        break;
    case DISPLAY_ENABLE:
        if (dataLength == sizeof(i2c_messages.display_enable) + SUB_ADDRESS_SIZE)
        {
            if (dataBuffer != NULL)
            {
                dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;    
                (void)memcpy(&i2c_messages.display_enable, &dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.display_enable));
                /* Set Flag to send Event */
                MMIM_DispENFlag_Ctrl(ENABLE);
            }
            else
            {
                ;
            }
        }
        break;
    case DISPLAY_SHUTDOWN:
        if (dataLength == sizeof(i2c_messages.display_shutdown) + SUB_ADDRESS_SIZE)
        {
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;
            
            if (dataBuffer != NULL)
            {
                
                (void)memcpy(&i2c_messages.display_shutdown, &dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.display_shutdown));
                
                MMIM_ShutdownFlag_Ctrl(ENABLE);
            }
            else
            {
                ;
            }
        }
        break;
    case CLIENT_SPECIFIC_DIAGNOSTIC_MESSAGE:
        if (dataLength == sizeof(i2c_messages.client_specific_diagnostic_message) + SUB_ADDRESS_SIZE)
        {
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;
            
            if (dataBuffer != NULL)
            {
                
                (void)memcpy(&i2c_messages.client_specific_diagnostic_message, &dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.client_specific_diagnostic_message));
                
                MMIM_DiagnosticMessageFlag_Ctrl(ENABLE);
            }
            else
            {
                ;
            }
        }
        break;    
    case JUMP_BOOTLOADER:               //20211028 Joel
        if (dataLength == sizeof(i2c_messages.jump_bootloader))
        {
            if (dataBuffer != NULL)
            {
                if(dataBuffer[2] == 0x03U)
                {
                    uint8_t u8Sum;
                    uint8_t u8Count;
                    for(u8Sum = 0,u8Count = 0; u8Count < 3; u8Count++)
                    {
                        u8Sum = (uint8_t) (u8Sum + *(dataBuffer + u8Count));
                    }
                    u8Sum ++;
                    i2c_messages.jump_bootloader[0] = 0xF4;
                    i2c_messages.jump_bootloader[1] = 0x03;
                    if((u8Sum == dataBuffer[3]) && (MBPC_BattPro_BattVolt_Get() > 95)) /* 20220708 Leo , detect Batt Volt > 9.5 V */
                    {
                        i2c_messages.jump_bootloader[2] = 0x43;
                        u16UpdateKey = BOOT_KEY;
                       //*(uint8_t *)0x08001500== BOOT_KEY;
                        gbUpdateRead = true;
                        //MMIM_JumpToBootCodeFlag_Ctrl(ENABLE);
                    }
                    else
                    {
                        i2c_messages.jump_bootloader[2] = 0x7F;
                        gbUpdateRead = true;
                    }
                    for(u8Sum = 0,u8Count = 0; u8Count < 3; u8Count++)
                    {
                        u8Sum = (uint8_t) (u8Sum + *(i2c_messages.jump_bootloader + u8Count));
                    }
                    u8Sum ++;
                    i2c_messages.jump_bootloader[3] = u8Sum;
                }
            }
            else
            {
                ;
            }
        }
        break;  
    case USER_Reset:
        if (dataLength == sizeof(i2c_messages.tx_cmd_status) + SUB_ADDRESS_SIZE)
        {
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_BIT6_0;
            if (dataBuffer != NULL)
            {
                
                (void)memcpy(&i2c_messages.tx_cmd_status, &dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.tx_cmd_status));
            }
            else
            {
                ;
            }
        }
        break;
    case USER_FACTORYMODE_EN:
        if (dataLength == sizeof(i2c_messages.factory_status) + SUB_ADDRESS_SIZE)
        {
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;
            if (dataBuffer != NULL)
            {
                
                (void)memcpy(&i2c_messages.factory_status, &dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.factory_status));
            }
            else
            {
                ;
            }
            
            if (i2c_messages.factory_status.FACTORY_EN == ENABLE)
            {
                u8FactoryEn = ENABLE;
            }
            else
            {
                u8FactoryEn = DISABLE;
            }
        }
        break;
        
    case USER_ReadEEprom:
        
        break;
        
    case USER_ClearErrorState:
        
        (void)memset(gErrorCounter, 0, sizeof(gErrorCounter)); /*Clear Counter buffer*/
        
        break;
    case USER_Derating_EN:
        
        u8DeratingEN = dataBuffer[FIRST_DATA_BYTE];
        
        break;
    case USER_WatchDogToogle_EN:
        if (dataLength == sizeof(i2c_messages.tx_cmd_status) + SUB_ADDRESS_SIZE)
        {
            if (dataBuffer[FIRST_DATA_BYTE] == 0xBB)
            {
                WatchDogToogle_EN = DISABLE;
            }
            else
            {
                WatchDogToogle_EN = ENABLE;
            }
        }
        break;
    case PN_DelAssmStatus_Ctrl:
        if (dataLength == sizeof(i2c_messages.pn_delivery_assembly_status_ctrl) + SUB_ADDRESS_SIZE)
        {
            dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_6_BIT;
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                //(void)memcpy( &i2c_messages.pn_delivery_assembly_status_ctrl,&dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.pn_delivery_assembly_status_ctrl));
                i2c_messages.pn_delivery_assembly_status_ctrl.WRT_ST = (dataBuffer[FIRST_DATA_BYTE] & BIT1);
                u8WriteDelAssmPNStatusRegFlag = ENABLE;
            }
            else
            {
                ;
            }
        }
        
        break;
    case PN_DelAssmblyFPN:
        
        if (dataLength == sizeof(u8DelAssmFPN) + SUB_ADDRESS_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                (void)memcpy( u8DelAssmFPN,&dataBuffer[FIRST_DATA_BYTE], sizeof(u8DelAssmFPN));
                u8DelAssmFPNFlag = ENABLE;
                u8WriteDelAssmPNStatusRegFlag = ENABLE;
            }
            else
            {
                ;
            }
        }
        break; 
    case PN_CoreAssmStatus_Ctrl:
        if (dataLength == sizeof(i2c_messages.pn_core_assembly_status_ctrl) + SUB_ADDRESS_SIZE)
        {
            dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_6_BIT;
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                //(void)memcpy( &i2c_messages.pn_core_assembly_status_ctrl,&dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.pn_core_assembly_status_ctrl));
                i2c_messages.pn_core_assembly_status_ctrl.WRT_ST = (dataBuffer[FIRST_DATA_BYTE] & BIT1);
                u8WriteCoreAssmPNStatusRegFlag = ENABLE;
            }
            else
            {
                ;
            }
        }
        
        break;
    case PN_CoreAssmblyFPN:
        
        if (dataLength == sizeof(u8CoreAssmFPN) + SUB_ADDRESS_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                (void)memcpy( u8CoreAssmFPN,&dataBuffer[FIRST_DATA_BYTE], sizeof(u8CoreAssmFPN));
                u8CoreAssmFPNFlag = ENABLE;
                u8WriteCoreAssmPNStatusRegFlag = ENABLE;
            }
            else
            {
                ;
            }
        }
        break;         
    case PN_MainCalibStatus_Ctrl:
        if (dataLength == sizeof(i2c_messages.pn_main_calibration_status_ctrl) + SUB_ADDRESS_SIZE)
        {
            dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_6_BIT;
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                //(void)memcpy( &i2c_messages.pn_main_calibration_status_ctrl,&dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.pn_main_calibration_status_ctrl));
                i2c_messages.pn_main_calibration_status_ctrl.WRT_ST = (dataBuffer[FIRST_DATA_BYTE] & BIT1);
                u8WriteMainCalibPNStatusRegFlag = ENABLE;
            }
            else
            {
                ;
            }
        }
        
        break;
    case PN_MainCalibFPN:
        
        if (dataLength == sizeof(u8MainCalibFPN) + SUB_ADDRESS_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                (void)memcpy( u8MainCalibFPN,&dataBuffer[FIRST_DATA_BYTE], sizeof(u8MainCalibFPN));
                u8MainCalibFPNFlag = ENABLE;
                u8WriteMainCalibPNStatusRegFlag = ENABLE;
            }
            else
            {
                ;
            }
        }
        break;       
    case PN_DispIDStatus_Ctrl:
        if (dataLength == sizeof(i2c_messages.pn_display_identification_status_ctrl) + SUB_ADDRESS_SIZE)
        {
            dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_6_BIT;
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                //(void)memcpy( &i2c_messages.pn_display_identification_status_ctrl,&dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.pn_display_identification_status_ctrl));
                i2c_messages.pn_display_identification_status_ctrl.WRT_ST = (dataBuffer[FIRST_DATA_BYTE] & BIT1);
                u8WriteDispIDPNStatusRegFlag = ENABLE;
            }
            else
            {
                ;
            }
        }
        
        break;
    case PN_DispIDFPN:
        
        if (dataLength == sizeof(u8DispIDFPN) + SUB_ADDRESS_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                (void)memcpy( u8DispIDFPN,&dataBuffer[FIRST_DATA_BYTE], sizeof(u8DispIDFPN));
                u8DispIDFPNFlag = ENABLE;
                u8WriteDispIDPNStatusRegFlag = ENABLE;
            }
            else
            {
                ;
            }
        }
        break;
        
    case PN_SwFPNStatus_Ctrl:
        if (dataLength == sizeof(i2c_messages.pn_software_ford_part_number_status_ctrl) + SUB_ADDRESS_SIZE)
        {
            dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_6_BIT;
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                //(void)memcpy( &i2c_messages.pn_software_ford_part_number_status_ctrl,&dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.pn_software_ford_part_number_status_ctrl));
                i2c_messages.pn_software_ford_part_number_status_ctrl.WRT_ST = (dataBuffer[FIRST_DATA_BYTE] & BIT1);
                u8WriteSoftwareFPNStatusRegFlag = ENABLE;
            }
            else
            {
                ;
            }
        }
        
        break;
    case PN_SoftwareFPN:
        
        if (dataLength == sizeof(u8SoftwareFPN) + SUB_ADDRESS_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                (void)memcpy( u8SoftwareFPN,&dataBuffer[FIRST_DATA_BYTE], sizeof(u8SoftwareFPN));
                u8SoftwareFPNFlag = ENABLE;
                u8WriteSoftwareFPNStatusRegFlag = ENABLE;
            }
            else
            {
                ;
            }
        }
        break;
        
        
    case USER_BIST_Crtl:
        if (dataBuffer != NULL)
        {
            MMIM_BISTModeFlag_Ctrl(ENABLE);
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_2_BIT;            
            (void)memcpy(&u8BIST_DATA_t,&dataBuffer[FIRST_DATA_BYTE], sizeof(u8BIST_DATA_t));
        }        
        break;               
        
    default:
        
        break;
    }
}

uint8_t MMIM_DataLength_FunSafe_Get(uint8_t subAddr,uint8_t rollCnt, uint32_t dataByteSize)
{
    // 2.3.6.6 IFS-MMI2C-SR-REQ-197875/B-Read from Subaddress Beyond Defined Length
    // --> set the length for Slave TX Buffer. The driver will return 0xFF for the bytes beyoud defined length
    // 2.3.6.7 IFS-MMI2C-SR-REQ-140565/C-Undefined / Unsupported Subaddress
    // --> return length = 0. The driver will return 0xFF for the bytes beyoud defined length.
    uint8_t length = NUM_ZERO;
    uint8_t subAddress_temp = NUM_ZERO;
    
    if(dataByteSize < (SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE))
    {
        subAddress_temp =  u8LastHostSendsubAddress;    
    }
    else if(dataByteSize == (SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE) && rollCnt == u8LastHostRollingCnt)
    {
        subAddress_temp =  u8LastHostSendsubAddress;
    }
    else
    {
        subAddress_temp = subAddr;
    }
    
    switch (subAddress_temp)
    {
    case DISPLAY_STATUS:
        length = (uint8_t)sizeof(i2c_messages.display_status);
        break;
    case DISPLAY_IDENTIFICATION:
        length = (uint8_t)sizeof(i2c_messages.display_identification);
        break;
    case LCD_BACKLIGHT_PWM_VALUE:
        length = (uint8_t)sizeof(i2c_messages.lcd_backlight_pwm_value);
        break;
    case DISPLAY_SCANNING:
        length = (uint8_t)sizeof(i2c_messages.display_scanning);
        break;
    case DISPLAY_ENABLE:
        length = (uint8_t)sizeof(i2c_messages.display_enable);
        break;
    case DISPLAY_SHUTDOWN:
        length = (uint8_t)sizeof(i2c_messages.display_shutdown);
        break;
    case MOMENTARY_LCD_PWM:
        length = (uint8_t)sizeof(u10BitPWMValue);
        break;
    case INTERRUPT_STATUS_MESSAGE:
        length = (uint8_t)sizeof(i2c_messages.interrupt_status_message);
        break;
    case CORE_ASSEMBLY:
        length = (uint8_t)sizeof(i2c_messages.core_assembly);
        break;
    case DELIVERY_ASSEMBLY:
        length = (uint8_t)sizeof(i2c_messages.delivery_assembly);
        break;
    case SOFTWARE_FORD_PART_NUMBER:
        length = (uint8_t)sizeof(i2c_messages.software_ford_part_number);
        break;
    case SERIAL_NUMBER:
        length = (uint8_t)sizeof(i2c_messages.serial_number);
        break;
    case MAIN_CALIBRATION_FORD_PART_NUMBER:
        length = (uint8_t)sizeof(i2c_messages.main_calibration_ford_part_number);
        break;
    case CLIENT_SPECIFIC_DIAGNOSTIC_MESSAGE:
        length = (uint8_t)sizeof(u8ClientDiag);
        break;  
    case USER_Temputer:
        length = (uint8_t)sizeof(NTC_Buffer);
        break;
    case USER_Temputer2:
        length = 4;
        break;
    case USER_FACTORYMODE_EN:
        length = (uint8_t)sizeof(i2c_messages.factory_status);
        break;
    case POWER_STATUS:
        length = 1U;
        break;
    case USER_ShowErrorState:
        length = 16;
        break;
    case USER_BatteryVoltage:
        length = 4U;
        break;
    case USER_ResetReq:
        length = 1U;
        break;
    case USER_WatchDogToogle_EN:
        length = (uint8_t)sizeof(WatchDogToogle_EN);
        break;
    case CMD_MODE:
        length = (uint8_t)sizeof(gApMode);
        break;
    case CMD_GET_STATE:
        length = 1U;
        break;
    case CMD_GET_VERSION:
        length = 11U;
        break;
    case USER_GetNowPWMDuty:
        length = (uint8_t)sizeof(u10BitPWMValue);
        break;
    case USER_BIST_Crtl:
        length = 1U;
        break;        
    case PN_DelAssmStatus_Ctrl:
        length = (uint8_t)sizeof(i2c_messages.pn_delivery_assembly_status_ctrl);
        break;
    case PN_DelAssmblyFPN:
        length = (uint8_t)sizeof(u8DelAssmFPN);
        break;
    case PN_CoreAssmStatus_Ctrl:
        length = (uint8_t)sizeof(i2c_messages.pn_core_assembly_status_ctrl);
        break;
    case PN_CoreAssmblyFPN:
        length = (uint8_t)sizeof(u8CoreAssmFPN);
        break;
    case PN_MainCalibStatus_Ctrl:
        length = (uint8_t)sizeof(i2c_messages.pn_main_calibration_status_ctrl);
        break;
    case PN_MainCalibFPN:
        length = (uint8_t)sizeof(u8MainCalibFPN);
        break;
    case  PN_DispIDStatus_Ctrl:
        length = (uint8_t)sizeof(i2c_messages.pn_display_identification_status_ctrl);
        break;
    case PN_DispIDFPN:
        length = (uint8_t)sizeof(u8DispIDFPN);
        break;
    case PN_SwFPNStatus_Ctrl:
        length = (uint8_t)sizeof(i2c_messages.pn_software_ford_part_number_status_ctrl);
        break;
    case PN_SoftwareFPN:
        length = (uint8_t)sizeof(u8SoftwareFPN);
        break;
    /* leo 20220621 */
    case PN_SnFPNStatus_Ctrl: 
        length = (uint8_t)sizeof(i2c_messages.pn_serial_number_status_ctrl);
        break;
    case PN_SerialNumFPN:
        length = (uint8_t)sizeof(u8SerialNumFPN);
        break;
        
    case USER_TFT_REV:
        length = (uint8_t)sizeof(u8TFT_Rev_ID);
        break;
        
        /* leo 2021_12_14 */
    case USER_VCOM:
        length = (uint8_t)sizeof(u8VCOM);
        break;
        
    case USER_Set_CoolFan_PWM:
        length = (uint8_t)sizeof(Cool_Fan_Control);
        break;

        /* leo 2021_12_14 */
    case USER_Get_Disp_Voltage:
        length = (uint8_t)sizeof(u8DispVoltSatus);
        break;
        
        /* leo 2021_12_14 */
    case USER_Get_LED_Voltage:
        length = (uint8_t)sizeof(u8LedVoltStatus);
        break;   
        
#if BACKDOOR_ICDIAG_OPEN
    
    case ICDIAG_CMD_ICFETCH:
        length = ICDIAG_CMD_ICFETCH_LENGTH;
        break;
        
    case ICDIAG_CMD_ICCTRL:
        length = ICDIAG_CMD_ICCTRL_LENGTH;
        break;
        
#endif
        
    default:
        break;
    }
    
    return length;
}

void MMIM_MessageData_FunSafe_Get(uint8_t subAddr,uint8_t rollCnt, uint8_t Crc8Value, uint8_t *dataBuffer, uint32_t dataByteSize)
{
    //2.3.6.8 IFS-MMI2C-SR-REQ-140611/C-Reserved Bits --> MASK BIT
    uint8_t subAddress_temp = NUM_ZERO;
    
    *dataBuffer = subAddr;
    (void)memset(&Crc8CheckBuff[0], 0x00U, sizeof(Crc8CheckBuff));
    Crc8CheckBuff[NUM_ZERO] = subAddr;    /* display send new subaddr */
    Crc8CheckBuff[FIRST_DATA_BYTE] = rollCnt;
    
#if BACKDOOR_ICDIAG_OPEN
    uint8_t *u8I2CICDiagBuffer;
#endif
    
    if(dataByteSize < (SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE))
    {
        subAddress_temp =  u8LastHostSendsubAddress;                    /* display send last data */
        dataBuffer[NUM_ZERO] = u8LastHostSendsubAddress;                /* display send last subaddr */
        
        if(dataByteSize == (SUB_ADDRESS_SIZE +  ROLLING_COUNTER_SIZE))  /* update last rolling counter */ 
        {
            u8LastHostRollingCnt = rollCnt;
        }        
        
        
        if(u8MCUSendRollingCnt >= MAX_ROLLING_COUNTER)                  /* calculate rolling counter */
        {
            u8MCUSendRollingCnt = NUM_ZERO; 
        }
        else
        {
            u8MCUSendRollingCnt ++;
        }
        
        (void)memset(&Crc8CheckBuff[0], 0x00U, sizeof(Crc8CheckBuff));  /* clear crc buffer */     
        Crc8CheckBuff[NUM_ZERO] = subAddress_temp;                      /* update last subaddr to crc buffer first byte */
    }
    else if(dataByteSize == (SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE) ) /* data length correct */
    {
        if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE)) == Crc8Value) /* CRC correct */
        {	
            (void)memset(&Crc8CheckBuff[0], 0x00U, sizeof(Crc8CheckBuff));    /* clear crc buffer */
            if(rollCnt == u8LastHostRollingCnt)   /* Rolling Counter wrong */
            {
                /* RC wrong */
                subAddress_temp =  u8LastHostSendsubAddress;       /* display send last last data */
                dataBuffer[NUM_ZERO] = u8LastHostSendsubAddress;   /* display send last subaddr */            
                u8LastHostRollingCnt = rollCnt;                    /* update last rolling counter */
                if(u8MCUSendRollingCnt >= MAX_ROLLING_COUNTER)     /* calculate RC */
                {
                    u8MCUSendRollingCnt = NUM_ZERO; 
                }
                else
                {
                    u8MCUSendRollingCnt ++;
                }
            }
            else if(rollCnt > u8LastHostRollingCnt)    /* Rolling Counter > u8LastHostRollingCnt ,  => OK */
            {
                /* RC correct */
                subAddress_temp = subAddr;                        /* display send new data */
                u8LastHostSendsubAddress = subAddr;               /* update last subaddr */
                u8LastHostRollingCnt = 	rollCnt;                  /* update last rolling counter */
                if(u8MCUSendRollingCnt >= MAX_ROLLING_COUNTER)    /* calculate RC */
                {
                    u8MCUSendRollingCnt = NUM_ZERO; 
                }
                else
                {
                    u8MCUSendRollingCnt ++;
                }
                u8CrcRuleAccept_flag = ENABLE;			  /* This flag indicate that host has been read successful */	
            }
            else  // rollCnt < u8LastHostRollingCnt  =>  OK
            {
                /* RC correct */
                subAddress_temp = subAddr;                   /* display send new data */
                u8LastHostSendsubAddress = subAddr;	     /* update last subaddr */
                u8LastHostRollingCnt = 	rollCnt;             /* update last rolling counter */
                
                u8MCUSendRollingCnt = NUM_ZERO;              /* reset RC to 0 */
                u8CrcRuleAccept_flag = ENABLE;		     /* This flag indicate that host has been read successful */
            }
            
            Crc8CheckBuff[NUM_ZERO] = subAddress_temp;       /* update last subaddr to crc buffer first byte */
        }
        else  /* CRC wrong */ 
        {
            subAddress_temp =  u8LastHostSendsubAddress;       /* display send last data */
            dataBuffer[NUM_ZERO] = u8LastHostSendsubAddress;   /* display send last subaddr */ 
            u8LastHostRollingCnt = rollCnt;                    /* update last rolling counter */
            if(u8MCUSendRollingCnt >= MAX_ROLLING_COUNTER)     /* calculate RC */
            {
                u8MCUSendRollingCnt = NUM_ZERO; 
            }
            else
            {
                u8MCUSendRollingCnt ++;
            }
            
            (void)memset(&Crc8CheckBuff[0], 0x00U, sizeof(Crc8CheckBuff));   /* clear crc buffer */
            Crc8CheckBuff[NUM_ZERO] = subAddress_temp;                       /* update last subaddr to crc buffer first byte */
        }
    }
    else /* SPSS has not defined this scope */
    {
        ;
    }
    
    if(u8CrcRuleAccept_flag == ENABLE)
    {		
        switch (subAddress_temp)
        {
        case DISPLAY_STATUS:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.display_status, sizeof(i2c_messages.display_status));
            dataBuffer[SECOND_DATA_BYTE] &= MASK_8_BIT;
            
            dataBuffer[sizeof(i2c_messages.display_status) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.display_status)+ ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(i2c_messages.display_status) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(i2c_messages.display_status) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));           
            
            
            /*Clear Reg. 0x30*/
            MMIM_InteruptStutsReg_Ctrl(SOUREFROMI2C, eINT_ERR, NUM_ZERO);
            
            /*Clear Reg. 0x00*/
            
            MMIM_DisplayStatusReg_Ctrl(SOUREFROMI2C, eALLReg, NUM_ZERO);
            
            break;
            /*2.2.7.3 IFS-MMI2C-SR-REQ-140616/I-Display Identification Table*/
        case DISPLAY_IDENTIFICATION:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.display_identification, sizeof(i2c_messages.display_identification));
            
            dataBuffer[sizeof(i2c_messages.display_identification) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.display_identification) + ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(i2c_messages.display_identification) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(i2c_messages.display_identification) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));             
            
            
            break;
        case LCD_BACKLIGHT_PWM_VALUE:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.lcd_backlight_pwm_value, sizeof(i2c_messages.lcd_backlight_pwm_value));
            dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_2_BIT;
            
            dataBuffer[sizeof(i2c_messages.lcd_backlight_pwm_value) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.lcd_backlight_pwm_value)+ ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(i2c_messages.lcd_backlight_pwm_value) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(i2c_messages.lcd_backlight_pwm_value) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case DISPLAY_SCANNING:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.display_scanning, sizeof(i2c_messages.display_scanning));
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_2_BIT;
            
            dataBuffer[sizeof(i2c_messages.display_scanning) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.display_scanning)+ ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(i2c_messages.display_scanning) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(i2c_messages.display_scanning) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case DISPLAY_ENABLE:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.display_enable, sizeof(i2c_messages.display_enable));
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;
            
            dataBuffer[sizeof(i2c_messages.display_enable) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.display_enable)+ ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(i2c_messages.display_enable) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(i2c_messages.display_enable) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case DISPLAY_SHUTDOWN:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.display_shutdown, sizeof(i2c_messages.display_shutdown));
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;
            
            dataBuffer[sizeof(i2c_messages.display_shutdown) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.display_shutdown)+ ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(i2c_messages.display_shutdown) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(i2c_messages.display_shutdown) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case MOMENTARY_LCD_PWM:
            dataBuffer[FIRST_DATA_BYTE] = (uint8_t)u10BitPWMValue & 0xFFU;
            dataBuffer[SECOND_DATA_BYTE] =(uint8_t)((u10BitPWMValue >> 8U) & MASK_2_BIT);
                       
            dataBuffer[sizeof(u10BitPWMValue) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;           
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u10BitPWMValue) + ROLLING_COUNTER_SIZE));           
            dataBuffer[sizeof(u10BitPWMValue) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u10BitPWMValue) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            break;
        case INTERRUPT_STATUS_MESSAGE:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.interrupt_status_message, sizeof(i2c_messages.interrupt_status_message));
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_4_BIT;
            /*CLear u8INITActiveFlag when 0x30 has been read */
            u8INITActiveFlag = DISABLE;
            
            dataBuffer[sizeof(i2c_messages.interrupt_status_message) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.interrupt_status_message)+ ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(i2c_messages.interrupt_status_message) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(i2c_messages.interrupt_status_message) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case CORE_ASSEMBLY:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.core_assembly, sizeof(i2c_messages.core_assembly));
            
            dataBuffer[sizeof(i2c_messages.core_assembly) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.core_assembly)+ ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(i2c_messages.core_assembly) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(i2c_messages.core_assembly) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case DELIVERY_ASSEMBLY:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.delivery_assembly, sizeof(i2c_messages.delivery_assembly));
            
            dataBuffer[sizeof(i2c_messages.delivery_assembly) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.delivery_assembly)+ ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(i2c_messages.delivery_assembly) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(i2c_messages.delivery_assembly) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case SOFTWARE_FORD_PART_NUMBER:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.software_ford_part_number, sizeof(i2c_messages.software_ford_part_number));
            
            dataBuffer[sizeof(i2c_messages.software_ford_part_number) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.software_ford_part_number)+ ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(i2c_messages.software_ford_part_number) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(i2c_messages.software_ford_part_number) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case SERIAL_NUMBER:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.serial_number, sizeof(i2c_messages.serial_number));
            
            dataBuffer[sizeof(i2c_messages.serial_number) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.serial_number)+ ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(i2c_messages.serial_number) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(i2c_messages.serial_number) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case MAIN_CALIBRATION_FORD_PART_NUMBER:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.main_calibration_ford_part_number, sizeof(i2c_messages.main_calibration_ford_part_number));
            
            dataBuffer[sizeof(i2c_messages.main_calibration_ford_part_number) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.main_calibration_ford_part_number)+ ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(i2c_messages.main_calibration_ford_part_number) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(i2c_messages.main_calibration_ford_part_number) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
            
        case CLIENT_SPECIFIC_DIAGNOSTIC_MESSAGE:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8ClientDiag[0], sizeof(u8ClientDiag));        
            dataBuffer[sizeof(u8ClientDiag) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;       
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8ClientDiag)+ ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(u8ClientDiag) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(u8ClientDiag) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));              
                                                     
            break;   

        case USER_Set_CoolFan_PWM:

            dataBuffer[RESERVED_IN_FIRST_BYTE] = gu16CoolFanPWMDuty / 10;
            
            dataBuffer[1U + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (1U+ ROLLING_COUNTER_SIZE));
            dataBuffer[1U + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( 1U + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            /*
            DEBUG_PF("Read_USER_Set_CoolFan_PWM\r\n");
            DEBUG_PF("duty =  %x\r\n",duty);
            DEBUG_PF("DATABUFFER0 =  %x\r\n",dataBuffer[0]);
            DEBUG_PF("DATABUFFER1 =  %x\r\n",dataBuffer[1]);
            DEBUG_PF("DATABUFFER2 =  %x\r\n",dataBuffer[2]);
            DEBUG_PF("DATABUFFER3 =  %x\r\n",dataBuffer[3]);
            DEBUG_PF("Read_USER_Set_CoolFan_PWM\r\n");
            */
            break;

        case USER_Reset:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.display_status, sizeof(i2c_messages.display_status));
            dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_BIT6_0;
            
            dataBuffer[sizeof(i2c_messages.display_status) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.display_status)+ ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(i2c_messages.display_status) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(i2c_messages.display_status) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
            /*
            **USER_Temputer 0xB2
            **Length 5
            **Byte1 PCB NTC sign byte 
            **Byte2 PCB NTC temp. high byte 
            **Byte3 PCB NTC temp. low byte 
            
            */
            
        case USER_Temputer:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &NTC_Buffer, sizeof(NTC_Buffer)); /*copy BL Temperuture*/
            
            dataBuffer[sizeof(NTC_Buffer) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(NTC_Buffer)+ ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(NTC_Buffer) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(NTC_Buffer) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
            /*
            **USER_Temputer2 0xB3
            **Length 5
            **Byte1 PCB NTC ADC low byte 
            **Byte2 PCB NTC ADC high byte 
            **Byte3 BL NTC ADC low byte 
            **Byte4 BL NTC ADC high byte 
            */
        case USER_Temputer2:
            
            if (u16PCBNtcAdcVaules != NULL)
            {
                (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u16PCBNtcAdcVaules, sizeof(u16PCBNtcAdcVaules)); /*copy BL Temperuture*/
                (void)memcpy(&dataBuffer[FIRST_DATA_BYTE + sizeof(u16PCBNtcAdcVaules)], &u16BacklightNtcAdcVaules, sizeof(u16BacklightNtcAdcVaules)); /*copy BL Temperuture*/
                
                dataBuffer[sizeof(u16PCBNtcAdcVaules)+ sizeof(u16BacklightNtcAdcVaules) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
                
                (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u16PCBNtcAdcVaules) + sizeof(u16BacklightNtcAdcVaules) + ROLLING_COUNTER_SIZE));
                
                
                dataBuffer[sizeof(u16PCBNtcAdcVaules) + sizeof(u16BacklightNtcAdcVaules) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u16PCBNtcAdcVaules) + sizeof(u16BacklightNtcAdcVaules) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
                
                
            }
            else
            {
                ;
            }
            break;
        case USER_FACTORYMODE_EN:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.factory_status, sizeof(i2c_messages.factory_status)); /*for Factory Mode*/
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;
            
            dataBuffer[sizeof(i2c_messages.factory_status) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.factory_status)+ ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(i2c_messages.factory_status) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(i2c_messages.factory_status) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
            
        case USER_ReadEEprom: /* report derating status to tx */
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8DeratingStatus, sizeof(u8DeratingStatus)); /*copy BL Temperuture*/
            
            dataBuffer[sizeof(u8DeratingStatus) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8DeratingStatus)+ ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(u8DeratingStatus) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(u8DeratingStatus) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;          
            
        case USER_ResetReq:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8LowVolResetReq, sizeof(u8LowVolResetReq));
            
            dataBuffer[sizeof(u8LowVolResetReq) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8LowVolResetReq)+ ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(u8LowVolResetReq) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u8LowVolResetReq) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case CMD_MODE:
            
            if (gApMode[0] != NULL)
            {
#if 1					
                (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &gApMode, sizeof(gApMode));
                
                dataBuffer[sizeof(gApMode) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
                
                (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(gApMode)+ ROLLING_COUNTER_SIZE));
                
                dataBuffer[sizeof(gApMode) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(gApMode) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
                
#else					
                (void)memset(dataBuffer, 0, sizeof(dataBuffer)); /*for bootloader*/
                (void)memcpy(dataBuffer, &gApMode, sizeof(gApMode));
#endif
            }
            else
            {
                ;
            }
            break;
        case CMD_GET_STATE:				
#if 1			
            dataBuffer[FIRST_DATA_BYTE] = NUM_ZERO;
            dataBuffer[FIRST_DATA_BYTE + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE],  (ONE_DATA_BYTE_SIZE + ROLLING_COUNTER_SIZE));
            
            dataBuffer[ONE_DATA_BYTE_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(ONE_DATA_BYTE_SIZE + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
#else 
            dataBuffer[0] = 0;
            
#endif	
            break;
        case CMD_GET_VERSION:
            
            if (gMcuVerReturn[0] != NULL)
            {
#if 1			
                (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &gMcuVerReturn, sizeof(gMcuVerReturn));
                
                dataBuffer[sizeof(gMcuVerReturn)+ ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
                
                (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(gMcuVerReturn) + ROLLING_COUNTER_SIZE));
                
                dataBuffer[sizeof(gMcuVerReturn) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(gMcuVerReturn) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
#else	
                (void)memset(dataBuffer, 0, sizeof(dataBuffer)); /*for bootloader*/	
                (void)memcpy(dataBuffer, &gMcuVerReturn, sizeof(gMcuVerReturn));
#endif
            }
            else
            {
                ;
            }
            
            break;
            
        case CMD_GET_TCON_VERSION:
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8TCONversion, sizeof(u8TCONversion)); 
            
            dataBuffer[sizeof(u8TCONversion) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8TCONversion)+ ROLLING_COUNTER_SIZE));
            dataBuffer[sizeof(u8TCONversion) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],( sizeof(u8TCONversion) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            break;
        case POWER_STATUS:
            /*for power status*/
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &ucErrorPower, sizeof(ucErrorPower));
            
            dataBuffer[sizeof(ucErrorPower) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(ucErrorPower) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(ucErrorPower) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(ucErrorPower) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            
            break;
        case USER_ShowErrorState:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &gReadBackErrorCounter, sizeof(gReadBackErrorCounter));
            
            dataBuffer[sizeof(gReadBackErrorCounter) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(gReadBackErrorCounter) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(gReadBackErrorCounter) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(gReadBackErrorCounter) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
            
        case USER_BatteryVoltage:
            
            dataBuffer[FIRST_DATA_BYTE] = (uint8_t)u16BatteryVoltageValue & 0xFFU;
            dataBuffer[SECOND_DATA_BYTE] =(uint8_t)((u16BatteryVoltageValue >> 8U) & 0xFFU);
            dataBuffer[THIR_DATA_BYTE] = (uint8_t)(uBatteryVoltAdcValue & 0xFFU);
            dataBuffer[FOUR_DATA_BYTE] = (uint8_t)((uBatteryVoltAdcValue >> 8U) & 0xFFU);
            
            dataBuffer[sizeof(u16BatteryVoltageValue) + sizeof(uBatteryVoltAdcValue) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u16BatteryVoltageValue) + sizeof(uBatteryVoltAdcValue) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(u16BatteryVoltageValue) + sizeof(uBatteryVoltAdcValue) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u16BatteryVoltageValue) + sizeof(uBatteryVoltAdcValue) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case USER_WatchDogToogle_EN:
#if 1
            if (WatchDogToogle_EN != NULL)
            {
                
                (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &WatchDogToogle_EN, sizeof(WatchDogToogle_EN));
                
                dataBuffer[sizeof(WatchDogToogle_EN) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
                
                (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(WatchDogToogle_EN) + ROLLING_COUNTER_SIZE));
                
                dataBuffer[sizeof(WatchDogToogle_EN) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(WatchDogToogle_EN) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
                
                
            }
            else
            {
                ;
            }
#else
            (void)memset(dataBuffer, 0, sizeof(dataBuffer)); /*for bootloader*/
            if (WatchDogToogle_EN != NULL)
            {
                
                (void)memcpy(dataBuffer, &WatchDogToogle_EN, sizeof(&WatchDogToogle_EN));
            }
            else
            {
                ;
            }
#endif
            break;
        case USER_GetNowPWMDuty:
            
            dataBuffer[FIRST_DATA_BYTE] = (uint8_t)u10BitPWMValue & 0xFFU;
            dataBuffer[SECOND_DATA_BYTE] =(uint8_t)((u10BitPWMValue >> 8U) & MASK_2_BIT);
            
            
            dataBuffer[sizeof(u10BitPWMValue) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u10BitPWMValue) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(u10BitPWMValue) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u10BitPWMValue) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;                
        case PN_DelAssmStatus_Ctrl:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.pn_delivery_assembly_status_ctrl, sizeof(i2c_messages.pn_delivery_assembly_status_ctrl));
            i2c_messages.pn_delivery_assembly_status_ctrl.INT_WRT = DISABLE;
            i2c_messages.pn_delivery_assembly_status_ctrl.CKSUM_ERR = DISABLE;			
            
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_3_BIT;
            
            dataBuffer[sizeof(i2c_messages.pn_delivery_assembly_status_ctrl) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.pn_delivery_assembly_status_ctrl) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(i2c_messages.pn_delivery_assembly_status_ctrl) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(i2c_messages.pn_delivery_assembly_status_ctrl) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case PN_DelAssmblyFPN:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8DelAssmFPN, sizeof(u8DelAssmFPN));
            
            dataBuffer[sizeof(u8DelAssmFPN) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8DelAssmFPN) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(u8DelAssmFPN) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u8DelAssmFPN) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case PN_CoreAssmStatus_Ctrl:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.pn_core_assembly_status_ctrl, sizeof(i2c_messages.pn_core_assembly_status_ctrl));
            i2c_messages.pn_core_assembly_status_ctrl.INT_WRT = DISABLE;
            i2c_messages.pn_core_assembly_status_ctrl.CKSUM_ERR = DISABLE;
            
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_3_BIT;
            
            dataBuffer[sizeof(i2c_messages.pn_core_assembly_status_ctrl) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.pn_core_assembly_status_ctrl) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(i2c_messages.pn_core_assembly_status_ctrl) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(i2c_messages.pn_core_assembly_status_ctrl) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case PN_CoreAssmblyFPN:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8CoreAssmFPN, sizeof(u8CoreAssmFPN));
            
            dataBuffer[sizeof(u8CoreAssmFPN) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8CoreAssmFPN) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(u8CoreAssmFPN) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u8CoreAssmFPN) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;   
        case PN_MainCalibStatus_Ctrl:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.pn_main_calibration_status_ctrl, sizeof(i2c_messages.pn_main_calibration_status_ctrl));
            
            i2c_messages.pn_main_calibration_status_ctrl.INT_WRT = DISABLE;
            i2c_messages.pn_main_calibration_status_ctrl.CKSUM_ERR = DISABLE;
            
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_3_BIT;
            
            dataBuffer[sizeof(i2c_messages.pn_main_calibration_status_ctrl) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.pn_main_calibration_status_ctrl) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(i2c_messages.pn_main_calibration_status_ctrl) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(i2c_messages.pn_main_calibration_status_ctrl) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case PN_MainCalibFPN:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8MainCalibFPN, sizeof(u8MainCalibFPN));
            
            dataBuffer[sizeof(u8MainCalibFPN) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8MainCalibFPN) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(u8MainCalibFPN) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u8MainCalibFPN) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;    
        case PN_DispIDStatus_Ctrl:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.pn_display_identification_status_ctrl, sizeof(i2c_messages.pn_display_identification_status_ctrl));
            i2c_messages.pn_display_identification_status_ctrl.INT_WRT = DISABLE;
            i2c_messages.pn_display_identification_status_ctrl.CKSUM_ERR = DISABLE;
            
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_3_BIT;
            
            dataBuffer[sizeof(i2c_messages.pn_display_identification_status_ctrl) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.pn_display_identification_status_ctrl) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(i2c_messages.pn_display_identification_status_ctrl) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(i2c_messages.pn_display_identification_status_ctrl) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case PN_DispIDFPN:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8DispIDFPN, sizeof(u8DispIDFPN));
            
            dataBuffer[sizeof(u8DispIDFPN) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8DispIDFPN) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(u8DispIDFPN) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u8DispIDFPN) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case PN_SwFPNStatus_Ctrl:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.pn_software_ford_part_number_status_ctrl, sizeof(i2c_messages.pn_software_ford_part_number_status_ctrl));
            i2c_messages.pn_software_ford_part_number_status_ctrl.INT_WRT = DISABLE;
            i2c_messages.pn_software_ford_part_number_status_ctrl.CKSUM_ERR = DISABLE;
            
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_3_BIT;
            
            dataBuffer[sizeof(i2c_messages.pn_software_ford_part_number_status_ctrl) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.pn_software_ford_part_number_status_ctrl) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(i2c_messages.pn_software_ford_part_number_status_ctrl) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(i2c_messages.pn_software_ford_part_number_status_ctrl) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case PN_SoftwareFPN:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8SoftwareFPN, sizeof(u8SoftwareFPN));
            
            dataBuffer[sizeof(u8SoftwareFPN) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8SoftwareFPN) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(u8SoftwareFPN) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u8SoftwareFPN) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        /* leo 20220621 */
        case PN_SnFPNStatus_Ctrl:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &i2c_messages.pn_serial_number_status_ctrl, sizeof(i2c_messages.pn_serial_number_status_ctrl));
            
            i2c_messages.pn_serial_number_status_ctrl.INT_WRT = DISABLE;
            i2c_messages.pn_serial_number_status_ctrl.CKSUM_ERR = DISABLE;
            
            dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_3_BIT;
            
            dataBuffer[sizeof(i2c_messages.pn_serial_number_status_ctrl) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.pn_serial_number_status_ctrl) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(i2c_messages.pn_serial_number_status_ctrl) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(i2c_messages.pn_serial_number_status_ctrl) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
        case PN_SerialNumFPN:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8SerialNumFPN, sizeof(u8SerialNumFPN));
            
            dataBuffer[sizeof(u8SerialNumFPN) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8SerialNumFPN) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(u8SerialNumFPN) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u8SerialNumFPN) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
            
        case USER_BIST_Crtl:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8BIST_DATA_t, sizeof(u8BIST_DATA_t));
            
            dataBuffer[sizeof(u8BIST_DATA_t) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8BIST_DATA_t) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(u8BIST_DATA_t) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u8BIST_DATA_t) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;   
            
        case USER_VCOM:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8VCOM, sizeof(u8VCOM));
            
            dataBuffer[sizeof(u8VCOM) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8VCOM) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(u8VCOM) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u8VCOM) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
            
            /* leo 2021_12_14 */
        case USER_Get_Disp_Voltage:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8DispVoltSatus, sizeof(u8DispVoltSatus));
            
            dataBuffer[sizeof(u8DispVoltSatus) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8DispVoltSatus) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(u8DispVoltSatus) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u8DispVoltSatus) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;
            
            /* leo 2021_12_14 */
        case USER_Get_LED_Voltage:     
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8LedVoltStatus, sizeof(u8LedVoltStatus));
            
            dataBuffer[sizeof(u8LedVoltStatus) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8LedVoltStatus) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(u8LedVoltStatus) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u8LedVoltStatus) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;             
            
        case USER_TFT_REV:
            
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], &u8TFT_Rev_ID, sizeof(u8TFT_Rev_ID));
            
            dataBuffer[sizeof(u8TFT_Rev_ID) + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8TFT_Rev_ID) + ROLLING_COUNTER_SIZE));
            
            dataBuffer[sizeof(u8TFT_Rev_ID) + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(sizeof(u8TFT_Rev_ID) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));
            
            break;      
            
#if BACKDOOR_ICDIAG_OPEN
        case ICDIAG_CMD_READ:
          
            u8I2CICDiagBuffer = ICDIAG_GetRxBuffer();
//            u8I2CICDiagBuffer = u8I2CICDiagBuffer + 2;
            /* add RC to databuffer */
            (void)memcpy(&dataBuffer[FIRST_DATA_BYTE], u8I2CICDiagBuffer, (ICDIAG_CMD_READ_LENGTH + ROLLING_COUNTER_SIZE));        
//            dataBuffer[ICDIAG_CMD_READ_LENGTH + ROLLING_COUNTER_SIZE] = u8MCUSendRollingCnt;
          
            /* add CRC to databuffer */
//            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (ICDIAG_CMD_READ_LENGTH + ROLLING_COUNTER_SIZE));
//            dataBuffer[ICDIAG_CMD_READ_LENGTH + ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO], ICDIAG_CMD_READ_LENGTH + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE);
            
            break;
#endif              
            
        default:
#if 1            
            dataBuffer[FIRST_DATA_BYTE] = u8MCUSendRollingCnt;
            
            (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (ROLLING_COUNTER_SIZE));
            
            dataBuffer[ROLLING_COUNTER_SIZE + CRC8_SIZE] = CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE));            
#endif
            break;
        }
    }
	
}

void MMIM_I2cMessages_FunSafe_Set(uint8_t subAddr, uint8_t *dataBuffer, uint32_t dataLength)
{
    uint8_t u8HostRollingCnt_temp = NUM_ZERO;
    
    (void)memset(&Crc8CheckBuff[0], 0x00U, sizeof(Crc8CheckBuff));
    Crc8CheckBuff[NUM_ZERO] = subAddr;
#if BACKDOOR_ICDIAG_OPEN
    //uint32_t u32DataAddr;
#endif
    switch (subAddr)
    {
    case LCD_BACKLIGHT_PWM_VALUE:
        
        if (dataLength == sizeof(i2c_messages.lcd_backlight_pwm_value) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {              
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            
            if (dataBuffer != NULL) 
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                {
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.lcd_backlight_pwm_value)+ ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength-1U])
                    {
                      
                        dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_2_BIT;
                        (void)memcpy(&i2c_messages.lcd_backlight_pwm_value, &dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.lcd_backlight_pwm_value));
                        /* Set Flag to send Event */
                        (void)MMIM_BLPWMFlag_Ctrl(ENABLE);
    
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
                
            }
            else
            {
                ;
            }
        }
        break;
    case DISPLAY_SCANNING:
        if (dataLength == sizeof(i2c_messages.display_scanning) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {            
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                {               
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.display_scanning)+ ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {
                        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_2_BIT;
                        (void)memcpy(&i2c_messages.display_scanning, &dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.display_scanning));
                        /* Set Flag to send Event */
                        (void)MMIM_ScanDirectionFlag_Ctrl(ENABLE);
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
                
            }
            else
            {
                ;
            }
        }
        break;
    case DISPLAY_ENABLE:
        if (dataLength == sizeof(i2c_messages.display_enable) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                {                 
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.display_enable) + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {
                        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;
                        (void)memcpy(&i2c_messages.display_enable, &dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.display_enable));
                        /* Set Flag to send Event */
                        MMIM_DispENFlag_Ctrl(ENABLE);
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }
        }
        break;
    case DISPLAY_SHUTDOWN:
        if (dataLength == sizeof(i2c_messages.display_shutdown) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                { 
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.display_shutdown) + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {
                        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;
                        (void)memcpy(&i2c_messages.display_shutdown, &dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.display_shutdown));
                        
                        MMIM_ShutdownFlag_Ctrl(ENABLE);
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }
        }
        break;
    case CLIENT_SPECIFIC_DIAGNOSTIC_MESSAGE:
#if 1
        if (dataLength == sizeof(i2c_messages.client_specific_diagnostic_message) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                { 
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.client_specific_diagnostic_message) + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {
                        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;
                        (void)memcpy(&i2c_messages.client_specific_diagnostic_message, &dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.client_specific_diagnostic_message));
                        
                        MMIM_DiagnosticMessageFlag_Ctrl(ENABLE);
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }
        }
#endif
        break; 
        
    case USER_Reset:
        if (dataLength == sizeof(i2c_messages.tx_cmd_status) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                { 
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.tx_cmd_status) + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {
                        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;
                        (void)memcpy(&i2c_messages.tx_cmd_status, &dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.tx_cmd_status));
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }
        }
        break;
    case USER_FACTORYMODE_EN:
        if (dataLength == sizeof(i2c_messages.factory_status) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                { 
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.factory_status) + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {
                        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;
                        (void)memcpy(&i2c_messages.factory_status, &dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.factory_status));
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }

            if (i2c_messages.factory_status.FACTORY_EN == ENABLE)
            {
                u8FactoryEn = ENABLE;
                COOLING_FAN_Pwm_Duty_Output_Factory_Mode(COOLING_FAN_PWM_GROUP, 1000U);
            }
            else
            {
                u8FactoryEn = DISABLE;
                COOLING_FAN_Pwm_Duty_Output_Factory_Mode(COOLING_FAN_PWM_GROUP, 0U);
            }
        }
        break;
        
    case USER_ReadEEprom:
             
        break;
        
    case USER_ClearErrorState:
        if (dataLength == ONE_DATA_BYTE_SIZE + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                { 
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (ONE_DATA_BYTE_SIZE + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {
                        (void)memset(gErrorCounter, 0, sizeof(gErrorCounter)); /*Clear Counter buffer*/
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
        }
        
        break;
    case USER_Derating_EN:
        if (dataLength == ONE_DATA_BYTE_SIZE + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                { 
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (ONE_DATA_BYTE_SIZE + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {
                        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_1_BIT;
                        
                        u8DeratingEN = dataBuffer[FIRST_DATA_BYTE];
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
        }        
        
        break;
    case USER_WatchDogToogle_EN:
        if (dataLength == sizeof(i2c_messages.tx_cmd_status) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                { 
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.tx_cmd_status)  + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {            
                        if (dataBuffer[FIRST_DATA_BYTE] == 0xBB)
                        {
                            WatchDogToogle_EN = DISABLE;
                        }
                        else
                        {
                            WatchDogToogle_EN = ENABLE;
                        }
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
        }  
        break;

    case USER_Set_CoolFan_PWM : 

        if (dataLength == 1U + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            
           if (dataBuffer != NULL) 
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                {
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (2U + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength-1U])
                    {
                        if(ENABLE == MMIM_FactoryFlag_Ctrl(GETFLAG))
                        {
                            gu16CoolFanPWMDuty = dataBuffer[1]*10;
                            COOLING_FAN_Pwm_Duty_Output_Factory_Mode(COOLING_FAN_PWM_GROUP, gu16CoolFanPWMDuty);
                        }
                        
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }
            
        }

        break;
        
    case USER_Eable_Local_Dimming:
        if (dataLength == sizeof(u8LocalDim_Flag) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                { 
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.tx_cmd_status)  + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {            
                        if (dataBuffer[FIRST_DATA_BYTE] == 0x0)
                        {
                            u8LocalDim_Flag = DISABLE;
                        }
                        else
                        {
                            u8LocalDim_Flag = ENABLE;
                        }
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
        }
        
        break;
        
    case PN_DelAssmStatus_Ctrl:
        if (dataLength == sizeof(i2c_messages.pn_delivery_assembly_status_ctrl) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                { 
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.pn_delivery_assembly_status_ctrl)  + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {    
                        dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_6_BIT;
                        
                        //(void)memcpy( &i2c_messages.pn_delivery_assembly_status_ctrl,&dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.pn_delivery_assembly_status_ctrl));
                        i2c_messages.pn_delivery_assembly_status_ctrl.WRT_ST = (dataBuffer[FIRST_DATA_BYTE] & BIT1);
                        u8WriteDelAssmPNStatusRegFlag = ENABLE;
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }
        }
        
        break;
    case PN_DelAssmblyFPN:
        
        if (dataLength == sizeof(u8DelAssmFPN) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                { 
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8DelAssmFPN)  + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {                  
                        (void)memcpy( u8DelAssmFPN,&dataBuffer[FIRST_DATA_BYTE], sizeof(u8DelAssmFPN));
                        u8DelAssmFPNFlag = ENABLE;
                        u8WriteDelAssmPNStatusRegFlag = ENABLE;
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }
        }
        break;
    case PN_CoreAssmStatus_Ctrl:
        if (dataLength == sizeof(i2c_messages.pn_core_assembly_status_ctrl) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                { 
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.pn_core_assembly_status_ctrl)  + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {    
                        dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_6_BIT;
                        //(void)memcpy( &i2c_messages.pn_core_assembly_status_ctrl,&dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.pn_core_assembly_status_ctrl));
                        i2c_messages.pn_core_assembly_status_ctrl.WRT_ST = (dataBuffer[FIRST_DATA_BYTE] & BIT1);
                        u8WriteCoreAssmPNStatusRegFlag = ENABLE;
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }
        }
        
        break;
    case PN_CoreAssmblyFPN:
        
        if (dataLength == sizeof(u8CoreAssmFPN) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                { 
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8CoreAssmFPN)  + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {                
                        (void)memcpy( u8CoreAssmFPN,&dataBuffer[FIRST_DATA_BYTE], sizeof(u8CoreAssmFPN));
                        u8CoreAssmFPNFlag = ENABLE;
                        u8WriteCoreAssmPNStatusRegFlag = ENABLE;
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            } 
        }
        break;
        
    case PN_MainCalibStatus_Ctrl:
        if (dataLength == sizeof(i2c_messages.pn_main_calibration_status_ctrl) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                {
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.pn_main_calibration_status_ctrl)  + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    { 
                        dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_6_BIT;
                        //(void)memcpy( &i2c_messages.pn_main_calibration_status_ctrl,&dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.pn_main_calibration_status_ctrl));
                        i2c_messages.pn_main_calibration_status_ctrl.WRT_ST = (dataBuffer[FIRST_DATA_BYTE] & BIT1);
                        u8WriteMainCalibPNStatusRegFlag = ENABLE;
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }
        }
        
        break;
    case PN_MainCalibFPN:
        
        if (dataLength == sizeof(u8MainCalibFPN) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                {
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8MainCalibFPN)  + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {                 
                        (void)memcpy( u8MainCalibFPN,&dataBuffer[FIRST_DATA_BYTE], sizeof(u8MainCalibFPN));
                        u8MainCalibFPNFlag = ENABLE;
                        u8WriteMainCalibPNStatusRegFlag = ENABLE;
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }  
        }
        break;     
    case PN_DispIDStatus_Ctrl:
        if (dataLength == sizeof(i2c_messages.pn_display_identification_status_ctrl) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                {
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.pn_display_identification_status_ctrl)  + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {
                        dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_6_BIT;
                        //(void)memcpy( &i2c_messages.pn_display_identification_status_ctrl,&dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.pn_display_identification_status_ctrl));
                        i2c_messages.pn_display_identification_status_ctrl.WRT_ST = (dataBuffer[FIRST_DATA_BYTE] & BIT1);
                        u8WriteDispIDPNStatusRegFlag = ENABLE;
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }
        }
        
        break;
    case PN_DispIDFPN:
        
        if (dataLength == sizeof(u8DispIDFPN) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                {
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8DispIDFPN)  + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {                 
                        
                        (void)memcpy( u8DispIDFPN,&dataBuffer[FIRST_DATA_BYTE], sizeof(u8DispIDFPN));
                        u8DispIDFPNFlag = ENABLE;
                        u8WriteDispIDPNStatusRegFlag = ENABLE;
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }
        }
        break;
    case PN_SwFPNStatus_Ctrl:
        if (dataLength == sizeof(i2c_messages.pn_software_ford_part_number_status_ctrl) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                {
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.pn_software_ford_part_number_status_ctrl)  + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {
                        dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_6_BIT;
                        //(void)memcpy( &i2c_messages.pn_software_ford_part_number_status_ctrl,&dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.pn_software_ford_part_number_status_ctrl));
                        i2c_messages.pn_software_ford_part_number_status_ctrl.WRT_ST = (dataBuffer[FIRST_DATA_BYTE] & BIT1);
                        u8WriteSoftwareFPNStatusRegFlag = ENABLE;
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }
        }
        
        break;
    case PN_SoftwareFPN:
        if (dataLength == sizeof(u8SoftwareFPN) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                {
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8SoftwareFPN)  + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {                 
                        
                        (void)memcpy( u8SoftwareFPN,&dataBuffer[FIRST_DATA_BYTE], sizeof(u8SoftwareFPN));
                        u8SoftwareFPNFlag = ENABLE;
                        u8WriteSoftwareFPNStatusRegFlag = ENABLE;
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }
        }
        break;   
    /* leo 20220621 */
    case PN_SnFPNStatus_Ctrl:
        if (dataLength == sizeof(i2c_messages.pn_serial_number_status_ctrl) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                {
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(i2c_messages.pn_serial_number_status_ctrl)  + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    { 
                        dataBuffer[RESERVED_IN_SECOND_BYTE] &= MASK_6_BIT;
                        //(void)memcpy( &i2c_messages.pn_serial_number_status_ctrl,&dataBuffer[FIRST_DATA_BYTE], sizeof(i2c_messages.pn_serial_number_status_ctrl));
                        i2c_messages.pn_serial_number_status_ctrl.WRT_ST = (dataBuffer[FIRST_DATA_BYTE] & BIT1);
                        u8WriteSerialNumFPNStatusRegFlag = ENABLE;
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            }
        }
        break;
    case PN_SerialNumFPN:
        if (dataLength == sizeof(u8SerialNumFPN) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                { 
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8SerialNumFPN)  + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    {                
                        (void)memcpy( u8SerialNumFPN,&dataBuffer[FIRST_DATA_BYTE], sizeof(u8SerialNumFPN));
                        u8SerialNumFPNFlag = ENABLE;
                        u8WriteSerialNumFPNStatusRegFlag = ENABLE;
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
            else
            {
                ;
            } 
        }
        break;
        break;
    
    case USER_BIST_Crtl:
        if (dataLength == sizeof(u8BIST_DATA_t) + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE)
        {
            /*COPY DATA FORM I2C RXBUFFER TO I2C GLOBAL STRUCTURE*/
            if (dataBuffer != NULL)
            {
                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
                
                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
                {
                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (sizeof(u8BIST_DATA_t)  + ROLLING_COUNTER_SIZE));
                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength - 1U])
                    { 
                        dataBuffer[RESERVED_IN_FIRST_BYTE] &= MASK_2_BIT;
                        MMIM_BISTModeFlag_Ctrl(ENABLE);
                        (void)memcpy(&u8BIST_DATA_t,&dataBuffer[FIRST_DATA_BYTE], sizeof(u8BIST_DATA_t));
                        
                        u8LastHostSendsubAddress = subAddr;	                    
                        u8CrcRuleAccept_flag = ENABLE;
                    }
                }
                u8LastHostRollingCnt = u8HostRollingCnt_temp;
            }
        }    
        
        break; 
#if BACKDOOR_ICDIAG_OPEN
    case ICDIAG_CMD_ICFETCH: 

//        if (dataLength == ICDIAG_CMD_ICFETCH_LENGTH + ROLLING_COUNTER_SIZE + CRC8_SIZE)
//        {                         
//            if (dataBuffer != NULL) 
//            {
//                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
//                
//                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
//                {
//                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (ICDIAG_CMD_ICFETCH_LENGTH + ROLLING_COUNTER_SIZE));
//                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength-1U])
//                    {
                        /*
                        *   parameter1 = WRITE (ICDIAG_CMD_ICCTRL)       
                        *   parameter2 = interface type , I2C = 1      //dataBuffer[1]
                        *   perameter3 = i2c channel , SCB5 = ch5      //dataBuffer[2]
                        *   parameter4 = device address                //dataBuffer[3]
                        *   parameter5 = data address                  //dataBuffer[4]
                        *   parameter6 = data length                   //dataBuffer[8]
                        *   parameter7 = data buffer                   //dataBuffer[9]
                        */
                        ICDIAG_CmdTrigger(ICDIAG_CMD_ICCTRL, dataBuffer[1], dataBuffer[2] , dataBuffer[3], dataBuffer[4], dataBuffer[8], NULL);
                                                                           
//                        u8LastHostSendsubAddress = subAddr;	                    
//                        u8CrcRuleAccept_flag = ENABLE;
//                    }
//                }
//                u8LastHostRollingCnt = u8HostRollingCnt_temp;
//                
//            }
//            else
//            {
//                ;
//            }
//        }            

        break;
        
    case ICDIAG_CMD_ICCTRL: 
      
//        if (dataLength == ICDIAG_CMD_ICCTRL_LENGTH + ROLLING_COUNTER_SIZE + CRC8_SIZE)
//        {              
//            if (dataBuffer != NULL) 
//            {
//                u8HostRollingCnt_temp =  dataBuffer[dataLength-2U];
//                
//                if(u8HostRollingCnt_temp > u8LastHostRollingCnt ||  u8HostRollingCnt_temp < u8LastHostRollingCnt)
//                {
//                    (void)memcpy(&Crc8CheckBuff[FIRST_DATA_BYTE], &dataBuffer[FIRST_DATA_BYTE], (ICDIAG_CMD_ICCTRL_LENGTH + ROLLING_COUNTER_SIZE));
//                    if(CRC8_Calculation(&Crc8CheckBuff[NUM_ZERO],(dataLength - CRC8_SIZE)) == dataBuffer[dataLength-1U])
//                    {
                        /*
                        *   parameter1 = WRITE (ICDIAG_CMD_ICCTRL)       
                        *   parameter2 = interface type , I2C = 1      //dataBuffer[1]
                        *   perameter3 = i2c channel , SCB5 = ch5      //dataBuffer[2]
                        *   parameter4 = device address                //dataBuffer[3]
                        *   parameter5 = data address                  //dataBuffer[4]
                        *   parameter6 = data length                   //dataBuffer[8]
                        *   parameter7 = data buffer                   //dataBuffer[9]
                        */
                        ICDIAG_CmdTrigger(ICDIAG_CMD_ICCTRL, dataBuffer[1], dataBuffer[2] , dataBuffer[3], dataBuffer[4], dataBuffer[8], &dataBuffer[9]);
                                                                           
//                        u8LastHostSendsubAddress = subAddr;	                    
//                        u8CrcRuleAccept_flag = ENABLE;
//                    }
//                }
//                u8LastHostRollingCnt = u8HostRollingCnt_temp;
//                
//            }
//            else
//            {
//                ;
//            }
//        }   

        break;
        
#endif
               
    default:
        
        break;
    }
}

bool MMIM_Update_Flag_Get(void)
{
    return gbUpdateRead;
}

void MMIM_Update_Flag_Set(bool bStatus)
{
    gbUpdateRead = bStatus;
}

/* leo 20211117 */
void MMIM_VCOM_Set(uint8_t u8VCOMVal)
{
    u8VCOM = u8VCOMVal;
}

uint8_t MMIM_VCOM_Get(void)
{
    return u8VCOM;
}

void MMIM_VoltageDRT_Flag_Set(bool bSetVDRT)
{
    bVoltageDerating = bSetVDRT;
}

bool MMIM_VoltageDRT_Flag_Get(void)
{
    return bVoltageDerating;
}

/* leo 2021_12_14 */
void MMIM_DispVoltError_Set(uint8_t u8Result)
{
    u8DispVoltSatus = u8Result;
}

/* leo 2021_12_14 */
uint8_t MMIM_DispVoltError_Get(void)
{
    return u8DispVoltSatus;
}

/* leo 2021_12_14 */
void MMIM_LedVoltError_Set(uint8_t u8Result)
{
    u8LedVoltStatus = u8LedVoltStatus & u8Result;
}

/* leo 2021_12_14 */
uint8_t MMIM_LedVoltError_Get(void)
{
    return u8LedVoltStatus;
}

void MMIM_TFT_Revision_Set(uint8_t u8RevID)
{
    u8TFT_Rev_ID = u8RevID;
}

uint8_t MMIM_TFT_Revision_Get(void)
{
    return u8TFT_Rev_ID;
}

/* leo 2022_week6 modify */
void MMIM_Client_Diag_Set(ERR_DataType_E Index, uint8_t u8Value)
{
    u8ClientDiag[Index] = u8Value;   
}
/* Joel 20220316 modify */
uint8_t MMIM_Client_Diag_Get(ERR_DataType_E Index)
{
    return u8ClientDiag[Index];
}

uint8_t MMIM_LocalDim_Flag_Get(void)
{ 
    return u8LocalDim_Flag; 
}

void MMIM_First_Diag_Set(uint8_t u8Enable)
{
    u8FirstDiag = u8Enable;  
    /* 1 means do fisrt diag,  0 means donot preform first diag */
}

uint8_t MMIM_First_Diag_Get(void)
{
    return u8FirstDiag;
}

void MMIM_Derating_Status_Set(uint8_t u8Status)
{
    u8DeratingStatus = u8Status;
}

void MMIM_TCON_VER_Set(uint8_t u8tconver)
{
    u8TCONversion = u8tconver;
}