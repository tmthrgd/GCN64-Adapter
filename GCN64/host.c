/*
 * host.c
 *
 *  Created on:
 *      Author: Tom
 */

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "protocol.h"
#include "host.h"

#include "device.h"

extern unsigned long GCN64HstEventHandler(unsigned char ucEvent, unsigned char *pucBuffer, unsigned long ulLength);

static unsigned char gcN64Buffer[36];

void GPIOEIntHandler(void)
{
	/*unsigned long len;

	IntDisable(INT_GPIOE);

	len = GCN64Receive(GPIO_PORTB_AHB_BASE, GPIO_PIN_0, gcN64Buffer);

	if (len)
	{
		GCN64HstEventHandler(gcN64Buffer[0], gcN64Buffer + 1, len - 1);
	}

	IntEnable(INT_GPIOE);*/
}

void GCN64HstInitialize(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPadConfigSet(GPIO_PORTB_AHB_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_OD_WPU);
	//GPIODirModeSet(GPIO_PORTB_AHB_BASE, GPIO_PIN_0, GPIO_DIR_MODE_OUT);

	GCN64InitializeProtocol();

	/*IntMasterEnable();
	IntEnable(INT_GPIOE);

	GPIOIntTypeSet(GPIO_PORTB_AHB_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);*/

	unsigned long len, i;

	while (1)
	{
		do
		{
			len = GCN64Receive(GPIO_PORTE_AHB_BASE, GPIO_PIN_4, gcN64Buffer);
		}
		while (!len);

		if (len == 1)
		{
			if (gcN64Buffer[0] == 0x00)
			{
				((tGCN64Status*)gcN64Buffer)->ID = ID_N64_CONTROLLER;

				GCN64Send(GPIO_PORTE_AHB_BASE, GPIO_PIN_4, gcN64Buffer, sizeof(tGCN64Status));
			}
			else if (gcN64Buffer[0] == 0x01)
			{
				((tN64Buttons*)gcN64Buffer)->Start = true;

				GCN64Send(GPIO_PORTE_AHB_BASE, GPIO_PIN_4, gcN64Buffer, sizeof(tN64Buttons));
			}
		}

		UARTprintf("%u ", len);

		for (i = 0; i < len; i++)
		{
			UARTprintf("%02x ", gcN64Buffer[i]);
		}

		UARTprintf("\n");
	}
}
