/*
* Mod_ScanDirection_Ctrl.h
*
*  Created on: 2020�~11��13��
*      Author: DerekWen
*/

#ifndef MOD_SCANDIRECTION_CTRL_H_
#define MOD_SCANDIRECTION_CTRL_H_

#include "gPinDef.h"

#define MSDC_DATABYTE_ONE 0U

typedef enum
{
	eLTR_TTB_Status =0U,            /*Left to right,top to bottom*/
	eRTL_TTB_Status,                /*right to Left,bottom to top*/
	eLTR_BTT_status,                /*Left to right,bottom to top*/
	eRTL_BTT_Status                 /*right to Left,top to bottom*/
}DisplayScanStatus_E;

DisplayScanStatus_E MSDC_ScanDirection_Ctrl(DisplayScanStatus_E eScanDirectionCtrlVal);

#endif /* MOD_SCANDIRECTION_CTRL_H_ */
