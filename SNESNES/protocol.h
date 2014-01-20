/*
 * protocol.h
 *
 *  Created on: 19/12/2012
 *      Author: Tom
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

typedef struct
{
	unsigned long ulData;
	unsigned char ucData;

	unsigned long ulClock;
	unsigned char ucClock;

	unsigned long ulLatch;
	unsigned char ucLatch;
} tSNESNESPins;

void SNESNESInitializeProtocol(void);

unsigned long SNESNESSend(tSNESNESPins *psPins, unsigned char *pucBuffer, unsigned long ulToSend);
unsigned long SNESNESReceive(tSNESNESPins *psPins, unsigned char *pucBuffer, unsigned long ulCount);

#endif /* PROTOCOL_H_ */
