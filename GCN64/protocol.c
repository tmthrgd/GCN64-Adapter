/*
 * protocol.c
 *
 *  Created on:
 *      Author: Tom
 */

#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "protocol.h"

#define READTIMEOUT 40

#define GPIOPINWRITE(ulPort, ucPins, ucVal) \
	(HWREGB((ulPort) + GPIO_O_DATA + ((ucPins) << 2)) = (ucVal))

#define GPIOPINREAD(ulPort, ucPins) \
	(HWREGB((ulPort) + GPIO_O_DATA + ((ucPins) << 2)))

#define GPIODIRSET(ulPort, ucPins, ulPinIO) \
	(HWREG((ulPort) + GPIO_O_DIR) = (((ulPinIO) & 1) ? (HWREG((ulPort) + GPIO_O_DIR) | (ucPins)) : (HWREG((ulPort) + GPIO_O_DIR) & ~(ucPins))))

#define TIMERENABLE(ulBase, ulTimer) \
	(HWREG((ulBase) + TIMER_O_CTL) |= (ulTimer) & (TIMER_CTL_TAEN | TIMER_CTL_TBEN))

#define TIMERDISABLE(ulBase, ulTimer) \
	(HWREG((ulBase) + TIMER_O_CTL) &= ~((ulTimer) & (TIMER_CTL_TAEN | TIMER_CTL_TBEN)))

#define TIMERALOADSET(ulBase, ulValue) \
	(HWREG((ulBase) + TIMER_O_TAILR) = (ulValue))

#define TIMERINTCLEAR(ulBase, ulIntFlags) \
	(HWREG((ulBase) + TIMER_O_ICR) = (ulIntFlags))

static tBoolean g_ucHasInitialized = false;
static volatile unsigned char g_ucNextDelay;

void Timer0IntHandler(void)
{
	TIMERINTCLEAR(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	if (g_ucNextDelay != 0)
	{
		g_ucNextDelay--;
	}
}

void GCN64InitializeProtocol(void)
{
	if (g_ucHasInitialized)
	{
		return;
	}

	ASSERT(SysCtlClockGet() >= 40000000 && (SysCtlClockGet() % 1000000) == 0);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	// 1us resolution
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 1000000);

	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	IntMasterEnable();

	IntEnable(INT_TIMER0A);

	g_ucHasInitialized = true;
}

unsigned long GCN64Send(unsigned long ulPort, unsigned char ucPin, unsigned char *pucBuffer, unsigned long ulToSend)
{
	unsigned long i;
	unsigned char j;
	
	GPIOPINWRITE(ulPort, ucPin, 0xFF);

	GPIODIRSET(ulPort, ucPin, GPIO_DIR_MODE_OUT);
	
	g_ucNextDelay = 1;

	TIMERENABLE(TIMER0_BASE, TIMER_A);

	for (i = 0; i < ulToSend; i++)
	{
		for (j = 0x80; j != 0x00; j >>= 1)
		{
			while (g_ucNextDelay != 0)
			{
			}

			GPIOPINWRITE(ulPort, ucPin, 0);

			if (pucBuffer[i] & j)
			{
				g_ucNextDelay = 1;
			}
			else
			{
				g_ucNextDelay = 3;
			}

			while (g_ucNextDelay != 0)
			{
			}

			GPIOPINWRITE(ulPort, ucPin, 0xFF);

			if (pucBuffer[i] & j)
			{
				g_ucNextDelay = 3;
			}
			else
			{
				g_ucNextDelay = 1;
			}
		}
	}

	while (g_ucNextDelay != 0)
	{
	}

	// Stop bit
	GPIOPINWRITE(ulPort, ucPin, 0);

	g_ucNextDelay = 1;

	while (g_ucNextDelay != 0)
	{
	}

	GPIOPINWRITE(ulPort, ucPin, 0xFF);

	TIMERDISABLE(TIMER0_BASE, TIMER_A);

	return i;
}

unsigned long GCN64Receive(unsigned long ulPort, unsigned char ucPin, unsigned char *pucBuffer)
{
	unsigned long i, l1, l2;
	unsigned char j, k;

	GPIODIRSET(ulPort, ucPin, GPIO_DIR_MODE_IN);

	for (l1 = 0; GPIOPINREAD(ulPort, ucPin) && l1 < READTIMEOUT; l1++)
	{
	}

	// Timeout
	if (l1 == READTIMEOUT)
	{
		return 0;
	}

	for (i = 0; ; i++)
	{
		for (j = 0x80, k = 0; j != 0x00; j >>= 1)
		{
			for (l1 = 0; !GPIOPINREAD(ulPort, ucPin) && l1 < READTIMEOUT; l1++)
			{
			}

			for (l2 = 0; GPIOPINREAD(ulPort, ucPin) && l2 < READTIMEOUT; l2++)
			{
			}

			// Stop bit
			if (l1 != READTIMEOUT && l2 == READTIMEOUT)
			{
				return i;
			}

			// Timeout
			if (l1 == READTIMEOUT || l2 == READTIMEOUT)
			{
				return 0;
			}

			if (l1 < l2)
			{
				k |= j;
			}
		}

		pucBuffer[i] = k;
	}
}
