#ifndef HAL_COMMON_H
#define HAL_COMMON_H

#define UNUSED(x)               ((void)(x))

#define DRIVER_FALSE            (0x00U)
#define DRIVER_TRUE             (0x01U)
#define DRIVER_TIMEOUT          (0x02U)
#define DRIVER_PARAMETER_ERROR  (0x04U)
#define DRIVER_WRITE_ERROR      (0x06U)
#define DRIVER_REGISTER_ERROR   (0x08U)

#define PIN_LOW                 (0x0U)
#define PIN_HIGH                (0x1U)

#define PROCESS_FAIL            (0U)
#define PROCESS_OK              (1U)

#define FUNC_DISABLE            (0x00U)
#define FUNC_ENABLE             (0x01U)

typedef enum
{
    ePROCESS_OK      = 0x5A,
    ePROCESS_FAIL    = 0xA5
}Status_Return_Typedef_e;

/*Cypress TraveoII*/
#include "cyhal_pin_package.h"

#endif


