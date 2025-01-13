

#ifndef TESTPATTERN_H_
#define TESTPATTERN_H_

#include <stdint.h>
#include "gPinDef.h"
//#include "TaskUser.h"

/* Define */


#define DeratingTestPattern DISABLE

//#define Pattern1 ENABLE

#define TempArraySize 839U

/*Global value*/

extern const uint8_t TestPattern_RA_TempUp[TempArraySize];
/* function */

uint16_t TestPattern_Temperaature_Get();

#endif /* TESTPATTERN_H_ */
