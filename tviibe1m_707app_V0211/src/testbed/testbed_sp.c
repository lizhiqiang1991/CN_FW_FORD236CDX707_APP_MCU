/*

File    : testbed_sp.c
Purpose : Check main stack usgae testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>

void testbed_sp(void)
{
  __enable_irq();
  
  /*Dummy data alloc in main stack*/
  uint8_t dummy[768];
  for (int i = 0; i < sizeof(dummy) ; i++)
    dummy[i] = 0;
   
  /* Get stack usage*/
  uint8_t u8Persent = 0;
  HAL_SP_Init(&gtsSPConfig);
  u8Persent = HAL_SP_Usage_Get(&gtsSPConfig);
#if CY_CPU_CORTEX_M0P
  printf("CPU_M0+ Main Stack Usage: %d%%\n",u8Persent);
#else
  printf("CPU_M4 Main Stack Usage: %d%%\n",u8Persent);
#endif
  
  UNUSED(dummy);
  for (;;);
}