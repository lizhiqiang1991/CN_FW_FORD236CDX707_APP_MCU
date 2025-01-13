/*
* Mod_DetectGPIO.c
*
*  Created on: 2020�~11��12��
*      Author: DerekWen
*/

/* include global */
#include "gPinDef.h"

/* include module */
#include "Mod_DataManagement.h"
#include "Mod_DetectGPIO.h"


/* include APP */
#include "GpioApp.h"


static DISPLAY_STATUS_T ErrorState_T= {NUM_ZERO};
/* static uint8_t gu8TouchCHGFlag = 0U; */
//static uint8_t gu8DisplayENState   = DISABLE;
static MDGPIO_IF_DETECTGPIO_T MDGPIO_IF_t = {NULL};
//static FPCStrategyState_E FPCStrategyState_e = FPCOutHigh;
static FPCStrategyState_E FPCStrategyState_e_L = FPCOut, FPCStrategyState_e_R = FPCOut;



typedef struct
{
    uint16_t u16DebounceCount_L;
    uint16_t u16DebounceCount_R;
    uint8_t  u8Input_L;
    uint8_t  u8Input_R;
}FPC_Config;

FPC_Config fpc_config = 
{
    .u16DebounceCount_L = 0,
    .u16DebounceCount_R = 0,
    .u8Input_L = 0,
    .u8Input_R = 0,
};


/*2.1.13.6 IFS-MMI2C-SR-REQ-199357/A-Avoid driving INTB_IN during loss-of-lock*/
/*because it is too late to  poll lock,it is quickly in ISR*/
/* IO_P13_4 */
#if 0
static void MDGPIO_984Lock_IRQHandler(void)
{   
    uint8_t u8Input = NUM_ZERO;
	if(gu8DisplayENState == ENABLE)
	{
        HAL_GPIO_PinRead(PIN_984_LOCK,&u8Input);
        
        if(u8Input == LEVEL_HIGH)
        {
            MDGPIO_IF_t.MDGPIO_LockState_Ctrl(eLocked);
            
            ErrorState_T.LLOSS = MDGPIO_IF_t.MDGPIO_LLOSS_Set(DISABLE);/*NO Loss of lock*/
        }
        else
        {
            MDGPIO_IF_t.MDGPIO_LockState_Ctrl(eUnLocked);
            
            ErrorState_T.LLOSS = MDGPIO_IF_t.MDGPIO_LLOSS_Set(ENABLE);/* Loss of lock*/
        }
        
	}
}
#endif


bool MDGPIO_ErrorCounter_Maintain(DisplayReg0x00_E ErrorCounterIndex, bool LastState, bool CurrentState)
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


#if 0
uint8_t MDGPIO_Powerlife_Check(uint8_t status, uint8_t times)
{
    uint8_t isAlive = NUM_ZERO;
    
    if (times == NUM_ZERO)
    {   /* times can not be zero */
        return false;
    }
    
    for( ; times>(uint8_t)NUM_ZERO; times--)
    {
        switch(status)
        {
        case PIN_MAX20419_PG:
            (void)HAL_GPIO_PinRead(PIN_MAX20419_PG,&isAlive); // P3V3
            /* ucErrorPower &= ~(uint8_t)ERROR_PGOOD_P5V0; */
            break;
        case PIN_LM61460_PG:
            (void)HAL_GPIO_PinRead(PIN_LM61460_PG,&isAlive); // P3V3
            /* ucErrorPower &= ~(uint8_t)ERROR_PGOOD_P3V3; */
            break;
        case PIN_MAX25221_FLTB:
            (void)HAL_GPIO_PinRead(PIN_MAX25221_FLTB,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_GOOD_P1V2; */
            break;
        case PIN_25240_PG_L:
            (void)HAL_GPIO_PinRead(PIN_25240_PG_L,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_LCDBIAS_FAULT; */
            break;
        case PIN_25240_PG_R:
            (void)HAL_GPIO_PinRead(PIN_25240_PG_R,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_LCDBIAS_FAULT; */
            break;
        case PIN_25210_RESET_L:
            (void)HAL_GPIO_PinRead(PIN_25210_RESET_L,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_LCDBIAS_FAULT; */
            break;
        case PIN_25210_RESET_R:
            (void)HAL_GPIO_PinRead(PIN_25210_RESET_R,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_LCDBIAS_FAULT; */
            break; 
        case PIN_25501_FLTB_L:
            (void)HAL_GPIO_PinRead(PIN_25501_FLTB_L,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_LCDBIAS_FAULT; */
            break;
        case PIN_MAX25501_FLTB_R:
            (void)HAL_GPIO_PinRead(PIN_MAX25501_FLTB_R,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_LCDBIAS_FAULT; */
            break; 
        default:
            break;
        }
        
        if (isAlive == NUM_ONE)
        {   /* pin good! */
            return true;
        }
        else
        {
        	Cy_SysLib_Delay(1);
        }
        
    }
    
    /* error flag */
    MMIM_Check_Power_Error(status);
    return false;
}

uint8_t MDGPIO_984_Check(uint8_t status, uint8_t times)
{
    uint8_t isAlive = NUM_ZERO;
    
    if (times == NUM_ZERO)
    {   /* times can not be zero */
        return false;
    }
    
    for( ; times>(uint8_t)NUM_ZERO; times--)
    {
        switch(status)
        {
        case PIN_984_LOCK:
            (void)HAL_GPIO_PinRead(PIN_984_LOCK,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_tU301_LOCK; */
            break;
        case PIN_MCU_DETECT0:
            (void)HAL_GPIO_PinRead(PIN_MCU_DETECT0,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_LCDBIAS_FAULT; */
            break;
        case PIN_MCU_DETECT1:
            (void)HAL_GPIO_PinRead(PIN_MCU_DETECT1,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_LCDBIAS_FAULT; */
            break;  
        default:
            break;
        }
        
        if (isAlive == NUM_ONE)
        {   /* pin good! */
            return true;
        }
        else
        {
        	Cy_SysLib_Delay(1);
        }
        
    }
    
    /* error flag */
    MMIM_Check_Power_Error(status);
    return false;
}

uint8_t MDGPIO_TCON_LCD_Check(uint8_t status, uint8_t times)
{
    uint8_t isAlive = NUM_ZERO;
    
    if (times == NUM_ZERO)
    {   /* times can not be zero */
        return false;
    }
    
    for( ; times>(uint8_t)NUM_ZERO; times--)
    {
        switch(status)
        {
        case PIN_FPCA_DET_IN_R:
            (void)HAL_GPIO_PinRead(PIN_FPCA_DET_IN_R,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_LCDBIAS_FAULT; */
            break;
        case PIN_FPCA_DET_IN_L:
            (void)HAL_GPIO_PinRead(PIN_FPCA_DET_IN_L,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_LCDBIAS_FAULT; */
            break;  
        case PIN_PANEL_ABD:
            (void)HAL_GPIO_PinRead(PIN_PANEL_ABD,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_LCDBIAS_FAULT; */
            break;   
        case PIN_TCON_ASIL_O:
            (void)HAL_GPIO_PinRead(PIN_TCON_ASIL_O,&isAlive);  // TCON fail flag
            /* ucErrorPower &= ~(uint8_t)ERROR_LCDBIAS_FAULT; */
            break; 
        default:
            break;
        }
        
        if (isAlive == NUM_ONE)
        {   /* pin good! */
            return true;
        }
        else
        {
        	Cy_SysLib_Delay(1);
        }
        
    }
    
    /* error flag */
    MMIM_Check_Power_Error(status);
    return false;
}
#endif
/* ------------------------------External Interface------------------------------- */
#if 0
void MDGPIO_PIN984Lock_IRQCallback_Instal(void)
{
    (void)HAL_GPIO_Callback_Register(PIN_984_LOCK, CY_GPIO_INTR_FALLING , MDGPIO_984Lock_IRQHandler);
}
#endif

FaultState_E MDGPIO_Display_Diagnostic(void)
{
    uint8_t Disp_Fault_state = NUM_ZERO;
    //    uint8_t Disp_Fault_state_S = NUM_ZERO;
    static uint8_t Disp_Fault_Counter = NUM_ZERO;
    
    (void)HAL_GPIO_PinRead(PIN_PANEL_ABD,&Disp_Fault_state);
    //   (void)HAL_GPIO_PinRead(PIN_TCON_ASIL_O,&Disp_Fault_state_S);
    
	/*Read Display State*/
    /*40 times filter*/
    if((Disp_Fault_state == LEVEL_LOW)) /* DISP_FAULT =LOW*/
	{
    	Disp_Fault_Counter++;
    	if(Disp_Fault_Counter == LCDFAULTDEBOUNCECOUNT)  
		{
			Disp_Fault_Counter = 0U;
			/*log error*/
			ErrorState_T.LCDERR = MDGPIO_ErrorCounter_Maintain(eLCDERR,ErrorState_T.LCDERR,true );
		}
		else
		{;} 
	}
	else
	{
		Disp_Fault_Counter = 0U;
		ErrorState_T.LCDERR = MDGPIO_ErrorCounter_Maintain(eLCDERR,ErrorState_T.LCDERR,false );
	}
    
    return  (FaultState_E)ErrorState_T.LCDERR;
}

FaultState_E MDGPIO_Diagnostic_LCD_BL_Fault(void)
{
    static uint16_t u16DebounceCount = NUM_ZERO;
    
    uint8_t BL_Fault_state_L = NUM_ZERO;
    uint8_t BL_Fault_state_R = NUM_ZERO;
    
    (void)HAL_GPIO_PinRead(PIN_MAX25501_FLTB_R,&BL_Fault_state_R); 
    (void)HAL_GPIO_PinRead(PIN_25501_FLTB_L,&BL_Fault_state_L);
    
    if(BL_Fault_state_R == LEVEL_LOW  || BL_Fault_state_L == LEVEL_LOW) /*Detect LED driver PIN_MAX25501_FLTB = low, */
    {
        u16DebounceCount++;
        if(u16DebounceCount == LCD_BL_FAULTDEBOUNCECOUNT)
        {
            
            /*log error*/
            ErrorState_T.BLERR = MDGPIO_ErrorCounter_Maintain(eLCD_BL_Fault,ErrorState_T.BLERR,true );
            u16DebounceCount = NUM_ZERO;
        }
        else
        {
            ;
        }
    
    }
    else
    {
            ErrorState_T.BLERR = MDGPIO_ErrorCounter_Maintain(eLCD_BL_Fault,ErrorState_T.BLERR,false );
    
            u16DebounceCount = NUM_ZERO;
    
    }
    
    return  (FaultState_E)ErrorState_T.BLERR;
}

FaultState_E MDGPIO_LLOSS_Detect(void)
{
    static uint16_t u16DebounceCount = NUM_ZERO;
    static uint16_t u16DebounceCountForRecov = NUM_ZERO;
    uint8_t u8Input = NUM_ZERO;
    
    (void)HAL_GPIO_PinRead(PIN_984_LOCK,&u8Input);
	if(u8Input == LEVEL_LOW)/*Detect PIN_984_LOCK = low*/
	{
        u16DebounceCount++;
        if(u16DebounceCount == LLOSSFAULTDEBOUNCECOUNT)
        {
            
            /*log error*/
            MMIM_Client_Diag_Set(General_Byte_1 , (MMIM_Client_Diag_Get(General_Byte_1) | Serdes_Lock_Error));
            ErrorState_T.LLOSS = MDGPIO_ErrorCounter_Maintain(elossoflock,ErrorState_T.LLOSS,true );
            u16DebounceCount = NUM_ZERO;
            
        }
        else
        {
            ;
        }
        
        u16DebounceCountForRecov = NUM_ZERO;
	}
	else
	{
        u16DebounceCountForRecov++;
        if(u16DebounceCountForRecov == LLOSSFAULTDEBOUNCECOUNT_RECOVER)
        {
            MMIM_Client_Diag_Set(General_Byte_1 , (MMIM_Client_Diag_Get(General_Byte_1) & (~Serdes_Lock_Error)));
            ErrorState_T.LLOSS = MDGPIO_ErrorCounter_Maintain(elossoflock,ErrorState_T.LLOSS,false );
            u16DebounceCountForRecov = NUM_ZERO;
            
        }
        else
        {
            ;
        }
        
        u16DebounceCount = NUM_ZERO;
	}
    
    return  (FaultState_E)ErrorState_T.LLOSS;
}

FaultState_E MDGPIO_Diagnostic_TCON_Fault(void)
{
    static uint16_t u16DebounceCount = NUM_ZERO;   
    uint8_t TCON_Fault_status = NUM_ZERO;
    bool bResult = false;
    
    (void)HAL_GPIO_PinRead(PIN_TCON_ASIL_O,&TCON_Fault_status); 
    
    
    if(TCON_Fault_status == LEVEL_LOW) /*Detect TCON_ASIL_O = low, */
    {
    	if(u16DebounceCount < TCON_ASIL_DEBOUNCECOUNT)
    	{
        u16DebounceCount++;
    	}
    	
        if(u16DebounceCount == TCON_ASIL_DEBOUNCECOUNT)
        {
            bResult = true;    /* detect TCON fault pin = error. */
        }
        else
        {
            ;
        }
    
    }
    else
    {        
        u16DebounceCount = NUM_ZERO;
        bResult = false;    /* detect TCON fault pin = good. */
    }
    
    return (FaultState_E)bResult;
    
}

#if 0
void MDGPIO_DisplayENState_Set(uint8_t u8DisplayENState)
{
    gu8DisplayENState = u8DisplayENState;
}
#endif
void MDGPIO_Interface_Set(MDGPIO_IF_POWERSTATE_CTRL_T pPowerState_Ctrl,
                          MDGPIO_IF_LLOSS_SET_T pLLOSS_Set,
                          MDGPIO_IF_INTBACTIVEFLAG_SET_T pINTB_Set,
                          MDGPIO_IF_LOCKSTATE_CTRL_T pLockState_ctrl)
{
    MDGPIO_IF_t.MDGPIO_PwrInitState_Ctrl = pPowerState_Ctrl;
    MDGPIO_IF_t.MDGPIO_LLOSS_Set = pLLOSS_Set;
    MDGPIO_IF_t.MDGPIO_INTBActiveFlag_Set =	pINTB_Set;
    MDGPIO_IF_t.MDGPIO_LockState_Ctrl = pLockState_ctrl;
}

FaultState_E MDGPIO_DisconnectERR_L_Detect(void)
{
    static uint8_t u8judge_L;
    
    HAL_GPIO_PinRead(PIN_FPCA_DET_IN_L,&fpc_config.u8Input_L);
    
    switch (FPCStrategyState_e_L)
    {
        case FPCOut:
    
            if (LEVEL_LOW == fpc_config.u8Input_L)
        {
                (void)HAL_GPIO_PinWrite(PIN_PANEL_FPCA_DET_OUT_L,PIN_HIGH);
                u8judge_L = LEVEL_LOW;
            }
            else
            {
                (void)HAL_GPIO_PinWrite(PIN_PANEL_FPCA_DET_OUT_L,PIN_LOW);
                u8judge_L = LEVEL_HIGH;
            }
            
            FPCStrategyState_e_L = FPC_Read;

        break;

        case FPC_Read:

            if (u8judge_L == fpc_config.u8Input_L)
            {
                fpc_config.u16DebounceCount_L++;
        }
        else
        {
                fpc_config.u16DebounceCount_L = 0;
                FPCStrategyState_e_L = FPCOut;
            }
            
            if (fpc_config.u16DebounceCount_L == DISCONNECTERRDEBOUNCECOUNT)
            {
                MMIM_Client_Diag_Set(General_Byte_1 , (MMIM_Client_Diag_Get(General_Byte_1) | FPC_L_Disconnect));
                ErrorState_T.DCERR = MDGPIO_ErrorCounter_Maintain(eDisconnecterror,ErrorState_T.DCERR,true );
                fpc_config.u16DebounceCount_L = NUM_ZERO;
                FPCStrategyState_e_L = FPCOut;
            }
            else
            {
                ;
            }
            
        break;
        default:
        break;
        }
    return  (FaultState_E)ErrorState_T.DCERR;
}
        
FaultState_E MDGPIO_DisconnectERR_R_Detect(void)
{
    static uint8_t u8judge_R;

    HAL_GPIO_PinRead(PIN_FPCA_DET_IN_R,&fpc_config.u8Input_R);
        
    switch (FPCStrategyState_e_R)
        {
        case FPCOut:

            if (LEVEL_LOW == fpc_config.u8Input_R)
            {
                //DEBUG_PF("GPIO_Pin_Write_High\r\n");
                (void)HAL_GPIO_PinWrite(PIN_PANEL_FPCA_DET_OUT_R,PIN_HIGH);
                u8judge_R = LEVEL_LOW;
            }
            else
            {
                //DEBUG_PF("GPIO_Pin_Write_Low\r\n");
                (void)HAL_GPIO_PinWrite(PIN_PANEL_FPCA_DET_OUT_R,PIN_LOW);
                u8judge_R = LEVEL_HIGH;
            }
            
            FPCStrategyState_e_R = FPC_Read;

        break;

        case FPC_Read:

            if (u8judge_R == fpc_config.u8Input_R)
            {
                //DEBUG_PF("Error\r\n");
                fpc_config.u16DebounceCount_R++;
            }
            else
            {
                //DEBUG_PF("NO Error\r\n");
                fpc_config.u16DebounceCount_R = 0;
                FPCStrategyState_e_R = FPCOut;
            }
            
            if (fpc_config.u16DebounceCount_R == DISCONNECTERRDEBOUNCECOUNT)
            {
                //DEBUG_PF("set error\r\n");
                MMIM_Client_Diag_Set(General_Byte_1 , (MMIM_Client_Diag_Get(General_Byte_1) | FPC_R_Disconnect));
                ErrorState_T.DCERR = MDGPIO_ErrorCounter_Maintain(eDisconnecterror,ErrorState_T.DCERR,true );
                fpc_config.u16DebounceCount_R = NUM_ZERO;
                FPCStrategyState_e_R = FPCOut;
            }
            else
            {
                ;
            }
            
        break;
        
    default:
        break;
        
    }
    return  (FaultState_E)ErrorState_T.DCERR;
}

FaultState_E MDGPIO_Diagnostic_TFT_Bias_Fault(void)
{
    static uint16_t u16DebounceCount = NUM_ZERO;   
    uint8_t TFT_Fault_status = NUM_ZERO;
    bool bResult = false;
    
    (void)HAL_GPIO_PinRead(PIN_MAX25221_FLTB,&TFT_Fault_status); 
    
    
    if(TFT_Fault_status == LEVEL_LOW) /*Detect MAX25221_FLTB = low, */
    {
    	if(u16DebounceCount < TCON_ASIL_DEBOUNCECOUNT)
    	{
    		u16DebounceCount++;
    	}
    	
        if(u16DebounceCount == TCON_ASIL_DEBOUNCECOUNT)
        {
            bResult = eFault;    /* detect TFT fault pin = error. */
        }
        else
        {
            ;
        }
    
    }
    else
    {        
        u16DebounceCount = NUM_ZERO;
        bResult = eNormal;    /* detect TFT fault pin = good. */
    }
    
    return (FaultState_E)bResult;
}
