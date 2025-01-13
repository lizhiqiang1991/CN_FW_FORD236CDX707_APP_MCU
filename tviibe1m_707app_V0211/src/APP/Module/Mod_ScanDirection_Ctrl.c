/******************************************************************************
;       Program  : Mod_ScanDirection_Ctrl.c
;       SWDD_ID  : SWDD_003
;       Project  : C120UAA01.0
;       Chip     : NXP S32K144
;       Date     : 2020 / 10 / 27
;       Author   : Derek
;       Describe : Set GPIO FCS =1 and���omponent靘��誘嚗�10ms�摰�ontrol screen scanning direction
;                   ��GPIO: pinFCS�inUD�inSS��
;                   ��身�潛bit0(HSD)=0,bit1(VSD)=0��
;
;
;
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
/* include module */
#include "Mod_ScanDirection_Ctrl.h"

/*------------------------ Declare External Var -----------------------------*/


DisplayScanStatus_E eDisplayScanStatus = eLTR_TTB_Status;
/*---------------------------- Start Program ---------------------------------*/
/******************************************************************************
;       Function Name			:	MSDC_ScanDirection_Ctrl
;       Function Description	:	��unit8 ucBLEN�LEN_PowerState_E ePowerState�LEN_PowerState_E eLockState瘙箏��Enable
;       Parameters				:	DisplayScanStatus_E eScanDirectionCtrlVal
;       Return Values			:	DisplayScanStatus_E eDisplayScanStatus
;		Source ID				:	SWEDD_003_01
******************************************************************************/
DisplayScanStatus_E MSDC_ScanDirection_Ctrl(DisplayScanStatus_E eScanDirectionCtrlVal)
{
    //GPIO_HIGH(portFCS,pinFCS);
	switch(eScanDirectionCtrlVal)
	{
        /*Left to right,top to bottom*/
	case eLTR_TTB_Status:
		//GPIO_LOW(portUD,pinUD);
		//GPIO_LOW(portSS,pinSS);
        eDisplayScanStatus = eLTR_TTB_Status;
		break;
        /*right to Left,bottom to top*/
	case eRTL_TTB_Status:
		//GPIO_LOW(portUD,pinUD);
		//GPIO_HIGH(portSS,pinSS);
        eDisplayScanStatus = eRTL_TTB_Status;
		break;
        /*Left to right,bottom to top*/
	case eLTR_BTT_status:
		//GPIO_HIGH(portUD,pinUD);
		//GPIO_LOW(portSS,pinSS);
        eDisplayScanStatus = eLTR_BTT_status;
		break;
        /*right to Left,top to bottom*/
	case eRTL_BTT_Status:
		//GPIO_HIGH(portUD,pinUD);
		//GPIO_HIGH(portSS,pinSS);
        eDisplayScanStatus = eRTL_BTT_Status;
		break;
	default:
		break;
	}
    return eDisplayScanStatus;
}
