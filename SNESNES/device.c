/*
 * device.c
 *
 *  Created on: 19/12/2012
 *      Author: Tom
 */

#include "inc/hw_types.h"
#include "protocol.h"
#include "device.h"

void SNESNESDevInitialize(void)
{
	SNESNESInitializeProtocol();
}

tNESButtons *NESDevButtons(unsigned char controller)
{
	return 0;
}

tSNESButtons *SNESDevButtons(unsigned char controller)
{
	return 0;
}
