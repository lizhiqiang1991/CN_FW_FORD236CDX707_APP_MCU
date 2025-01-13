/*

File    : testbed_clock.c
Purpose : CLOCK testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"

void testbed_clock(void) 
{
  //HAL_Clock_Init(&gtsClockInit);
  __enable_irq();
  for (;;);
}
