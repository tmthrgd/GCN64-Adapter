/*
 * usb_structs.c
 *
 *  Created on:
 *      Author: Tom
 */

#include <stddef.h>
#include "inc/hw_types.h"
#include "usblib/usb-ids.h"
#include "usblib/usblib.h"
#include "usblib/device/usbdevice.h"
#include "usblib/usbhid.h"
#include "usblib/device/usbdhid.h"
#include "USB/usbhidgamepad.h"

// http://stackoverflow.com/a/1598827/1474139
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define USB_VID USB_VID_STELLARIS
#define USB_PID USB_PID_MOUSE

extern unsigned long USBHIDGamepadEventHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgParam, void *pvMsgData);

//*****************************************************************************
//
// The languages supported by this device.
//
//*****************************************************************************
const unsigned char g_pucLangDescriptor[] =
{
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};

//*****************************************************************************
//
// The manufacturer string.
//
//*****************************************************************************
const unsigned char g_pucManufacturerString[] =
{
    (8 + 1) * 2,
    USB_DTYPE_STRING,
    'X', 0, 'e', 0, 'n', 0, 't', 0, 'h', 0, 'r', 0, 'a', 0, 'x', 0
};

//*****************************************************************************
//
// The product string.
//
//*****************************************************************************
const unsigned char g_pucProductString[] =
{
    (14 + 1) * 2,
    USB_DTYPE_STRING,
    'N', 0, '6', 0, '4', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 't', 0, 'r', 0, 'o', 0, 'l', 0, 'l', 0, 'e', 0, 'r', 0
};

//*****************************************************************************
//
// The serial number string.
//
//*****************************************************************************
const unsigned char g_pucSerialNumberString[] =
{
    (8 + 1) * 2,
    USB_DTYPE_STRING,
    '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0
};

//*****************************************************************************
//
// The interface description string.
//
//*****************************************************************************
const unsigned char g_pucInterfaceString[] =
{
	(21 + 1) * 2,
	USB_DTYPE_STRING,
	'H', 0, 'I', 0, 'D', 0, ' ', 0, 'G', 0, 'a', 0, 'm', 0, 'e', 0,
	'p', 0, 'a', 0, 'd', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0, 'e', 0,
	'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0
};

//*****************************************************************************
//
// The configuration description string.
//
//*****************************************************************************
const unsigned char g_pucConfigString[] =
{
	(25 + 1) * 2,
	USB_DTYPE_STRING,
	'H', 0, 'I', 0, 'D', 0, ' ', 0, 'G', 0, 'a', 0, 'm', 0, 'e', 0,
	'p', 0, 'a', 0, 'd', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 'f', 0,
	'i', 0, 'g', 0, 'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0, 'o', 0,
	'n', 0
};

//*****************************************************************************
//
// The descriptor string table.
//
//*****************************************************************************
const unsigned char * const g_pStringDescriptors[] =
{
    g_pucLangDescriptor,
    g_pucManufacturerString,
    g_pucProductString,
    g_pucSerialNumberString,
    g_pucInterfaceString,
    g_pucConfigString
};

//*****************************************************************************
//
// The report descriptor for the gamepad class device.
//
//*****************************************************************************
#define CONTROLLER_REPORT(n) \
	Usage(USB_HID_GAMEPAD), \
	Collection(USB_HID_APPLICATION), \
		ReportID(n), \
		Collection(USB_HID_PHYSICAL), \
			/*
			 * The buttons.
			 */ \
			UsagePage(USB_HID_BUTTONS), \
			UsageMinimum(1), \
			UsageMaximum(16), \
			LogicalMinimum(0), \
			LogicalMaximum(1), \
			\
			/*
			 * 14 - 1 bit values for the buttons.
			 */ \
			ReportSize(1), \
			ReportCount(16), \
			Input(USB_HID_INPUT_DATA | USB_HID_INPUT_VARIABLE \
				| USB_HID_INPUT_ABS | USB_HID_INPUT_NOWRAP \
				| USB_HID_INPUT_PREFER | USB_HID_INPUT_NONULL), \
			\
			/*
			 * The X and Y axis.
			 */ \
			UsagePage(USB_HID_GENERIC_DESKTOP), \
			Usage(USB_HID_X), \
			Usage(USB_HID_Y), \
			LogicalMinimum(-80), \
			LogicalMaximum(80), \
			\
			/*
			 * 2 - 8 bit Values for X and Y.
			 */ \
			ReportSize(8), \
			ReportCount(2), \
			Input(USB_HID_INPUT_DATA | USB_HID_INPUT_VARIABLE \
					| USB_HID_INPUT_ABS | USB_HID_INPUT_NOWRAP \
					| USB_HID_INPUT_LINEAR | USB_HID_INPUT_PREFER \
					| USB_HID_INPUT_NONULL), \
			\
			EndCollection, \
		EndCollection

const unsigned char g_pucGamepadReportDescriptor[] =
{
	UsagePage(USB_HID_GENERIC_DESKTOP),
	CONTROLLER_REPORT(1),
	CONTROLLER_REPORT(2),
	CONTROLLER_REPORT(3),
	CONTROLLER_REPORT(4)
};

//*****************************************************************************
//
// The HID class descriptor table. For the gamepad class, we have only a single
// report descriptor.
//
//*****************************************************************************
const unsigned char * const g_pGamepadClassDescriptors[] =
{
	g_pucGamepadReportDescriptor
};

const tHIDDescriptor g_sGamepadHIDDescriptor =
{
    9,                                 // bLength
    USB_HID_DTYPE_HID,                 // bDescriptorType
    0x111,                             // bcdHID (version 1.11 compliant)
    0,                                 // bCountryCode (not localized)
    1,                                 // bNumDescriptors
    {
        {
            USB_HID_DTYPE_REPORT,                 // Report descriptor
            sizeof g_pucGamepadReportDescriptor   // Size of report descriptor
        }
    }
};

//*****************************************************************************
//
// The HID device private data.
//
//*****************************************************************************
tHIDGamepadInstance g_sGamepadInstance;

//*****************************************************************************
//
// The HID descriptor for the gamepad device.
//
//*****************************************************************************
const tUSBDHIDGamepadDevice g_sGamepadDevice =
{
	//
	// The Vendor ID you have been assigned by USB-IF.
	//
	USB_VID,

	//
	// The product ID you have assigned for this device.
	//
	USB_PID,

	//
	// The power consumption of your device in milliamps.
	//
	100,

	//// The value to be passed to the host in the USB configuration descriptor’s
	// bmAttributes field.
	//
	USB_CONF_ATTR_BUS_PWR,

	//
	// A pointer to your mouse callback event handler.
	//
	USBHIDGamepadEventHandler,

	//
	// A value that you want passed to the callback alongside every event.
	//
	(void *)&g_sGamepadDevice,

	//
	// A pointer to your string table.
	//
	g_pStringDescriptors,

	//
	// The number of entries in your string table. This must equal
	// (1 + (5 * (num languages))).
	//
	COUNT_OF(g_pStringDescriptors),

	//
	// A pointer to the private instance data allocated for the API to use.
	//
	&g_sGamepadInstance,

	//
	//
	//
	g_pGamepadClassDescriptors,

	//
	//
	//
	&g_sGamepadHIDDescriptor
};
