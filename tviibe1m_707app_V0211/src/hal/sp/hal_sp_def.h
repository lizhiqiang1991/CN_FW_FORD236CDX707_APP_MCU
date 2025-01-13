/***************************************************************************
*\file hal_sp_def.h 
****************************************************************************/

#ifndef HAL_SP_DEF_H
#define HAL_SP_DEF_H

typedef struct 
{
  void* pvStackStart;
  void* pvStackEnd;
  void* pvMinStackSize;
}Sp_Config_Typedef_s;

extern Sp_Config_Typedef_s gtsSPConfig;

#endif

  