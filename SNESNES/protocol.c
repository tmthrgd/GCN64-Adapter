/*
 * protocol.c
 *
 *  Created on: 19/12/2012
 *      Author: Tom
 */

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "protocol.h"

#define GPIOPINWRITE(ulPort, ucPins, ucVal) \
	(HWREGB((ulPort) + GPIO_O_DATA + ((ucPins) << 2)) = (ucVal))

#define GPIOPINREAD(ulPort, ucPins) \
	(HWREGB((ulPort) + GPIO_O_DATA + ((ucPins) << 2)))

void SNESNESInitializeProtocol(void)
{

}

unsigned long SNESNESSend(tSNESNESPins *psPins, unsigned char *pucBuffer, unsigned long ulToSend)
{
	ASSERT(0);
	return 0;
}

unsigned long SNESNESReceive(tSNESNESPins *psPins, unsigned char *pucBuffer, unsigned long ulCount)
{
	unsigned long i;
	unsigned char j;

	GPIOPINWRITE(psPins->ulLatch, psPins->ucLatch, 0xFF);

	// sleep

	GPIOPINWRITE(psPins->ulLatch, psPins->ucLatch, 0x00);

	for (i = 0; i < ulCount; i++)
	{
		for (j = 0x80; j != 0x00; j >>= 1)
		{
			GPIOPINWRITE(psPins->ulClock, psPins->ucClock, 0xFF);

			// sleep

			pucBuffer[i] |= GPIOPINREAD(psPins->ulData, psPins->ucData);

			GPIOPINWRITE(psPins->ulClock, psPins->ucClock, 0x00);
		}
	}

	return i;
}
