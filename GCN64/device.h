/*
 * device.h
 *
 *  Created on:
 *      Author: Tom
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "inc/hw_types.h"

#ifndef PACKED
#define PACKED __attribute__ ((packed))
#endif

#define ID_N64_CONTROLLER   0x0500
#define ID_N64_MICROPHONE   0x0001
#define ID_N64_KEYBOARD     0x0002
#define ID_N64_MOUSE        0x0200
#define ID_GBA0             0x0004
#define ID_GBA1             0x0800
#define ID_GC_CONTROLLER    0x0900
#define ID_GC_WAVEBIRD0     0xe960
#define ID_GC_WAVEBIRD1     0xe9a0
#define ID_GC_WAVEBIRD2     0xa800
#define ID_GC_WAVEBIRD3     0xebb0
#define ID_GC_KEYBOARD      0x0820
#define ID_GC_STEERINGWHEEL 0x0800
#define ID_GC_DKONGAS       0x0900

typedef struct
{
	unsigned short ID;

	unsigned char Reserved:6;
	tBoolean WasConnected:1;
	tBoolean IsConnected:1;
} PACKED tGCN64Status;

typedef struct
{
	unsigned char Reserved[3];
} PACKED tGCButtons;

typedef struct
{
	tBoolean Right:1;
	tBoolean Left:1;
	tBoolean Down:1;
	tBoolean Up:1;
	tBoolean Start:1;
	tBoolean Z:1;
	tBoolean B:1;
	tBoolean A:1;

	tBoolean CRight:1;
	tBoolean CLeft:1;
	tBoolean CDown:1;
	tBoolean CUp:1;
	tBoolean R:1;
	tBoolean L:1;
	unsigned char Reserved:1;
	tBoolean Reset:1;

	signed char X;

	signed char Y;
} PACKED tN64Buttons;

void GCN64DevInitialize(void);

void GCN64DevReset(unsigned char controller);
tGCN64Status *GCN64DevStatus(unsigned char controller);
tN64Buttons *N64DevButtons(unsigned char controller);
tGCButtons *GCDevButtons(unsigned char controller);

//#undef PACKED

#endif /* CONTROLLER_H_ */
