/*
 * dvicee.c
 *
 *  Created on:
 *      Author: Tom
 */

#include <stddef.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "protocol.h"
#include "device.h"

typedef struct
{
	unsigned long ulPort;
	unsigned char ucPin;
} tControllerPortMap;

static const tControllerPortMap controllerPorts[] =
{
	{ GPIO_PORTB_AHB_BASE, GPIO_PIN_0 },
	{ GPIO_PORTB_AHB_BASE, GPIO_PIN_1 },
	{ GPIO_PORTB_AHB_BASE, GPIO_PIN_2 },
	{ GPIO_PORTB_AHB_BASE, GPIO_PIN_3 }
};

// http://stackoverflow.com/a/1598827/1474139
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

static unsigned char gcN64Buffer[36];

static unsigned long GCN64Transaction(unsigned long ulPort, unsigned char ucPin, unsigned char *buffer, unsigned long toSend)
{
	if (GCN64Send(ulPort, ucPin, buffer, toSend))
	{
		return GCN64Receive(ulPort, ucPin, buffer);
	}
	else
	{
		return 0;
	}
}

void GCN64DevInitialize(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPadConfigSet(GPIO_PORTB_AHB_BASE, 0x0F, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_OD_WPU);

	GCN64InitializeProtocol();
}

void GCN64DevReset(unsigned char controller)
{
	if (controller > COUNT_OF(controllerPorts))
	{
		return;
	}

	gcN64Buffer[0] = 0xFF;
	GCN64Send(controllerPorts[controller].ulPort, controllerPorts[controller].ucPin, gcN64Buffer, 1);
}

tGCN64Status *GCN64DevStatus(unsigned char controller)
{
	unsigned long len;

	if (controller > COUNT_OF(controllerPorts))
	{
		return 0;
	}

	gcN64Buffer[0] = 0x00;
	len = GCN64Transaction(controllerPorts[controller].ulPort, controllerPorts[controller].ucPin, gcN64Buffer, 1);

	if (len != sizeof(tGCN64Status))
	{
		return 0;
	}

	return (tGCN64Status*)gcN64Buffer;
}

tN64Buttons *N64DevButtons(unsigned char controller)
{
	unsigned long len;

	if (controller > COUNT_OF(controllerPorts))
	{
		return 0;
	}

	gcN64Buffer[0] = 0x01;
	len = GCN64Transaction(controllerPorts[controller].ulPort, controllerPorts[controller].ucPin, gcN64Buffer, 1);

	if (len != sizeof(tN64Buttons))
	{
		return 0;
	}

	return (tN64Buttons*)gcN64Buffer;
}
