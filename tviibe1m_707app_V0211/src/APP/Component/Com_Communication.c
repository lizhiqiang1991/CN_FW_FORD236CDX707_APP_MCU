/**
* @file Com_Commnucation.c
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-03-30
* 
* @copyright Copyright (c) 2021
* 
*/
/* include public information */
#include "gPinDef.h"
/* include component */
#include "Com_Communication.h"
/* include module */
#include "Mod_Datamanagement.h"

/* include APP */
#include "I2CSlaveApp.h"



static CC_EVENT_T CC_Event_t={NUM_ZERO,{NUM_ZERO}};
static I2C_MESSAGES_T CC_I2CMSG_t;

/*state machine */

CC_StateMachine_E CC_StateMachine_e = CC_STATE_INIT;

uint8_t  CC_Event_Get(CC_EVENT_T *pEvent_t)
{
    uint8_t ProcessStatus = NUM_ZERO;
    
    if(pEvent_t == NULL)
    {
        ProcessStatus = NUM_ZERO;
    }
    else
    {
        memcpy(pEvent_t,&CC_Event_t,sizeof(CC_EVENT_T));
        ProcessStatus = true;
    }
    
    return ProcessStatus;
}
uint8_t  CC_Event_Set(CC_EVENT_T *pEvent_t)
{
    uint8_t ProcessStatus = NUM_ZERO;
    
    if(pEvent_t == NULL)
    {
        ProcessStatus = NUM_ZERO;
    }
    else
    {
        memcpy(&CC_Event_t,pEvent_t,sizeof(CC_EVENT_T));
        ProcessStatus = true;
    }
    
    return ProcessStatus;
}




static void CC_Service_Init(void)
{
    CC_StateMachine_e = CC_STATE_PROCESS;
    CC_Event_t.Event_e = NUM_ZERO;
    memset(CC_Event_t.EventBuff,NUM_ZERO,sizeof(CC_Event_t.EventBuff));
}

static void CC_Service_Data_Process (void)
{  
    /* Get i2c message structure */
    CC_StateMachine_e = CC_STATE_PROCESS;
    USER_BIST_MODE_CTRL_T CC_User_BIST_Data;
    
    //if(MMIM_pI2CMessage_Get()!=NULL)
    //{
    //  CC_I2CMSG_t = *MMIM_pI2CMessage_Get();
    //}
    
    /* Polling Flag to send event to other Task */
    /* Display enable flag  */
    if(MMIM_DispENFlag_Ctrl(GETFLAG) == ENABLE) 
    {
        if(MMIM_pI2CMessage_Get()!=NULL)
        {
          CC_I2CMSG_t = *MMIM_pI2CMessage_Get();
        }
        /* clear event */
        CC_Event_t.Event_e |= ( eEVENT_DISPEN);
        memcpy(CC_Event_t.EventBuff,&CC_I2CMSG_t.display_enable,sizeof(CC_I2CMSG_t.display_enable));
        (void)MMIM_DispENFlag_Ctrl(DISABLE);
        
    }
    /* Backlight PWM flag */
    if(MMIM_BLPWMFlag_Ctrl(GETFLAG) == ENABLE) 
    {
        if(MMIM_pI2CMessage_Get()!=NULL)
        {
          CC_I2CMSG_t = *MMIM_pI2CMessage_Get();
        }
        /* Clear buffer */
        memset(CC_Event_t.EventBuff + CC_MEM_SHIFT4 ,NUM_ZERO, CC_EVENT_BUFF_SIZE_4);
        
        CC_Event_t.Event_e |= (eEVENT_BLEN);   	
       
        /* Set buffer */
        CC_Event_t.EventBuff[CC_MEM_SHIFT4] = CC_I2CMSG_t.lcd_backlight_pwm_value.BL_PWM_9_8;
        CC_Event_t.EventBuff[CC_MEM_SHIFT5] = CC_I2CMSG_t.lcd_backlight_pwm_value.BL_PWM_7_0;        
        CC_Event_t.EventBuff[CC_MEM_SHIFT6] = ENABLE;     
    }
    
    /* Scan direction falg */
    if(MMIM_ScanDirectionFlag_Ctrl(GETFLAG) == ENABLE) 
    {
        if(MMIM_pI2CMessage_Get()!=NULL)
        {
          CC_I2CMSG_t = *MMIM_pI2CMessage_Get();
        }
        /* Clear buffer */
        memset(CC_Event_t.EventBuff + CC_MEM_SHIFT8 ,NUM_ZERO, CC_EVENT_BUFF_SIZE_4);
        
        CC_Event_t.Event_e |= (eEVENT_SCANDIRECT);
        
        /* Set buffer */
        CC_Event_t.EventBuff[CC_MEM_SHIFT8] |= CC_I2CMSG_t.display_scanning.HSD;
        CC_Event_t.EventBuff[CC_MEM_SHIFT8] |= CC_I2CMSG_t.display_scanning.VSD << SHIFT_BIT_1;

        (void)MMIM_ScanDirectionFlag_Ctrl(DISABLE);
    }
    
    /* Host Shutdown flag */
    if(MMIM_ShutdownFlag_Ctrl(GETFLAG) == ENABLE) 
    {
        if(MMIM_pI2CMessage_Get()!=NULL)
        {
          CC_I2CMSG_t = *MMIM_pI2CMessage_Get();
        }
        /* Clear buffer */
        memset(CC_Event_t.EventBuff + CC_MEM_SHIFT12 ,NUM_ZERO, CC_EVENT_BUFF_SIZE_4);

        /* Set buffer */
        if((CC_I2CMSG_t.display_shutdown.SHDWN & BIT1) == DISPSHUTDOWNENABLE)
        {
                CC_Event_t.Event_e |= (eEVENT_HOSTSHUTDOWN);    /* 20220311 Joel test close display off */
        }
        
        (void)MMIM_ShutdownFlag_Ctrl(DISABLE);
    }
    
    /* Jump to boot code */
    if(MMIM_JumpToBootCodeFlag_Ctrl(GETFLAG) == ENABLE) 
    {
        if(MMIM_pI2CMessage_Get()!=NULL)
        {
          CC_I2CMSG_t = *MMIM_pI2CMessage_Get();
        }     
      
        CC_Event_t.Event_e |= (eEVENT_JUMPTOBOOTcode);
        // memcpy(CC_Event_t.EventBuff,&CC_I2CMSG_t.display_shutdown,sizeof(CC_I2CMSG_t.display_shutdown));
        (void)MMIM_JumpToBootCodeFlag_Ctrl(DISABLE);   
    }
    
    /* BIST mode  flag */
    if(MMIM_BISTModeFlag_Ctrl(GETFLAG) == ENABLE) 
    {
        if(MMIM_pI2CMessage_Get()!=NULL)
        {
          CC_I2CMSG_t = *MMIM_pI2CMessage_Get();
        }
      
        CC_User_BIST_Data = MMIM_BISTData_Get();
        CC_Event_t.Event_e |= (eEVENT_BISTMODE);
        memcpy(CC_Event_t.EventBuff + CC_MEM_SHIFT16 ,&CC_User_BIST_Data,sizeof(CC_User_BIST_Data));
        (void)MMIM_BISTModeFlag_Ctrl(DISABLE);   
    } 
    
    /* Diagnostic Message flag */
    if(MMIM_DiagnosticMessageFlag_Ctrl(GETFLAG) == ENABLE) 
    {
        if(MMIM_pI2CMessage_Get()!=NULL)
        {
          CC_I2CMSG_t = *MMIM_pI2CMessage_Get();
        }
      
        CC_Event_t.Event_e |= (eEVENT_DIAGNOSTICMESSAGE);
        memcpy(CC_Event_t.EventBuff + CC_MEM_SHIFT20 ,&CC_I2CMSG_t.client_specific_diagnostic_message,sizeof(CC_I2CMSG_t.client_specific_diagnostic_message));
        (void)MMIM_DiagnosticMessageFlag_Ctrl(DISABLE);          
    } 
    
}

static void CC_Service_Error_Alarm (void)
{
    ;
}
void (*const CC_State_Machine[CC_STATE_MAX + 1])(void) =
{CC_Service_Init, CC_Service_Data_Process, CC_Service_Error_Alarm};