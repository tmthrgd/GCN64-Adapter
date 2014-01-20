/*
 * device.h
 *
 *  Created on: 19/12/2012
 *      Author: Tom
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include "inc/hw_types.h"

#ifndef PACKED
#define PACKED __attribute__ ((packed))
#endif

typedef struct
{
	unsigned char Reserved;
} tNESButtons;

typedef struct
{
	unsigned char Reserved[2];
} tSNESButtons;

void SNESNESDevInitialize(void);

tNESButtons *NESDevButtons(unsigned char controller);
tSNESButtons *SNESDevButtons(unsigned char controller);

#endif /* DEVICE_H_ */
