/*
 * main.c
 *
 *  Created on:
 *      Author: Tom
 */

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "GCN64/device.h"

#include <string.h>
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "usblib/usb-ids.h"
#include "usblib/usblib.h"
#include "usblib/device/usbdevice.h"
#include "usblib/usbhid.h"
#include "usblib/device/usbdhid.h"
#include "USB/usbhidgamepad.h"

#define LED_BASE  GPIO_PORTF_BASE
#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

#define NUM_CONTROLLERS 4

extern const tUSBDHIDGamepadDevice g_sGamepadDevice;

static void *g_psDevice;
volatile static tBoolean g_ucSendReport;

void main(void)
{
	unsigned char pucReport[1 + sizeof(tN64Buttons)];
	//tGCN64Status *status;
	tN64Buttons *buttons;
	unsigned long i;

	// 80MHz
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PLL);
	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	// LED
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(LED_BASE, RED_LED | BLUE_LED | GREEN_LED);
	GPIOPinWrite(LED_BASE, RED_LED | BLUE_LED | GREEN_LED, RED_LED);

	// UART
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	SysCtlPeripheralReset(SYSCTL_PERIPH_UART0);
	UARTStdioInitExpClk(0, 115200);

	UARTprintf("\033[2JN64 Controller\n");

	// CONTROLLER
	GCN64DevInitialize();

	// USB
	memset(pucReport, 0, sizeof pucReport);
	pucReport[0] = 1;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	USBStackModeSet(0, USB_MODE_FORCE_DEVICE, 0);

	g_psDevice = USBDHIDGamepadInit(0, &g_sGamepadDevice, pucReport, 0);

	if (!g_psDevice)
	{
		UARTprintf("USBDHIDGamepadInit failed.\n");
		return;
	}

	/*GPIOPinWrite(LED_BASE, RED_LED | BLUE_LED | GREEN_LED, GREEN_LED);

	while (1)
	{
		status = GCN64DevStatus(0);

		if (status)
		{
			UARTprintf("ID%04x HasPak%u HadPak%u\n", status->ID, status->IsConnected, status->WasConnected);
			break;
		}
	}*/

	GPIOPinWrite(LED_BASE, RED_LED | BLUE_LED | GREEN_LED, BLUE_LED);

	for (i = 0; ; i = (i + 1) % NUM_CONTROLLERS)
	{
		while (!g_ucSendReport)
		{
		}

		//IntDisable(INT_USB0);
		buttons = N64DevButtons(i);
		//IntEnable(INT_USB0);

		if (buttons)
		{
			pucReport[0] = i + 1;

			UARTprintf("%u A%u B%u Z%u Start%u Up%u Down%u Left%u Right%u L%u R%u CUp%u CDown%u CLeft%u CRight%u Reset%u X%03i Y%03i\n",
					i + 1,
					buttons->A,
					buttons->B,
					buttons->Z,
					buttons->Start,
					buttons->Up,
					buttons->Down,
					buttons->Left,
					buttons->Right,
					buttons->L,
					buttons->R,
					buttons->CUp,
					buttons->CDown,
					buttons->CLeft,
					buttons->CRight,
					buttons->Reset,
					buttons->X,
					buttons->Y);

			memcpy(pucReport + 1, buttons, sizeof(tN64Buttons));

			g_ucSendReport = false;

			USBDHIDGamepadStateChange(g_psDevice, 1 + sizeof(tN64Buttons));
		}
	}

	GPIOPinWrite(LED_BASE, RED_LED | BLUE_LED | GREEN_LED, RED_LED);
}

unsigned long USBHIDGamepadEventHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgParam, void *pvMsgData)
{
	UARTprintf("pvCBData%04x ulEvent%04x ulMsgParam%04x pvMsgData%04x\n", pvCBData, ulEvent, ulMsgParam, pvMsgData);

	switch (ulEvent)
	{
		case USB_EVENT_CONNECTED:
		case USB_EVENT_TX_COMPLETE:
		{
			g_ucSendReport = true;
			break;
		}
		/*case USBD_HID_EVENT_SET_REPORT:
		{
			GPIOPinWrite(LED_BASE, RED_LED | BLUE_LED | GREEN_LED, ((unsigned char*)pvMsgData)[0]);
			break;
		}*/
	}

	return 0;
}
