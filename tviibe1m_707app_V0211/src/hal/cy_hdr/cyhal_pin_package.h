#ifndef CYHAL_PIN_PACKAGE
#define CYHAL_PIN_PACKAGE

/** Gets a pin definition from the provided port and pin numbers */
#define CYHAL_GET_GPIO(port, pin)   ((((uint8_t)(port)) << 3U) + ((uint8_t)(pin)))

/** Macro that, given a gpio, will extract the pin number */
#define CYHAL_GET_PIN(pin)          ((uint8_t)(((uint8_t)pin) & 0x07U))
/** Macro that, given a gpio, will extract the port number */
#define CYHAL_GET_PORT(pin)         ((uint8_t)((((uint8_t)pin) >> 3U) & 0x1FU))

#include "cyhal_cyt2b75.h"

#endif

