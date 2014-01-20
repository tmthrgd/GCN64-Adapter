/*
 * protocol.h
 *
 *  Created on:
 *      Author: Tom
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

void GCN64InitializeProtocol(void);

unsigned long GCN64Send(unsigned long ulPort, unsigned char ucPin, unsigned char *pucBuffer, unsigned long ulToSend);
unsigned long GCN64Receive(unsigned long ulPort, unsigned char ucPin, unsigned char *pucBuffer);

#endif /* PROTOCOL_H_ */
