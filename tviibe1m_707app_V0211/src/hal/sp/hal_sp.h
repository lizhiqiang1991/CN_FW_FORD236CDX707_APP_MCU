/***************************************************************************
*\file hal_sp.h 
****************************************************************************/

#ifndef HAL_SP_H
#define HAL_SP_H

Status_Return_Typedef_e HAL_SP_Init           (Sp_Config_Typedef_s *tsSp);
Status_Return_Typedef_e HAL_SP_Overflow_Check (Sp_Config_Typedef_s *tsSp);
                uint8_t HAL_SP_Usage_Get      (Sp_Config_Typedef_s *tsSp);

#endif

  