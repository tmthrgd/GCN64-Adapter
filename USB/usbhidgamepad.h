/*
 * usbhidgamepad.h
 *
 *  Created on: 16/12/2012
 *      Author: Tom
 */

#ifndef USBHIDGAMEPAD_H_
#define USBHIDGAMEPAD_H_

#include "inc/hw_types.h"
#include "usblib/usblib.h"
#include "usblib/device/usbdevice.h"

#ifdef __cplusplus
extern "C"
{
#endif

	#define USB_HID_GAMEPAD 0x05

	typedef enum
	{
	    //
	    // Unconfigured.
	    //
	    HID_GAMEPAD_STATE_UNCONFIGURED,

	    //
	    // No keys to send and not waiting on data.
	    //
	    HID_GAMEPAD_STATE_IDLE,

	    //
	    // Waiting on report data from the host.
	    //
	    HID_GAMEPAD_STATE_WAIT_DATA,

	    //
	    // Waiting on data to be sent out.
	    //
	    HID_GAMEPAD_STATE_SEND
	}
	tGamepadState;

	typedef struct
	{
	    //
	    // The USB configuration number set by the host or 0 of the device is
	    // currently unconfigured.
	    //
	    unsigned char ucUSBConfigured;

	    //
	    // A buffer used to hold the last input report sent to the host.
	    //
	    unsigned char *pucReport;

	    //
	    //
	    //
	    unsigned long ulReportSize;

	    //
	    //
	    //
	    unsigned char *pucDataBuffer;

	    //
	    //
	    //
	    //unsigned long ulDataBufferSize;

	    //
	    // The current state of the mouse interrupt IN endpoint.
	    //
	    volatile tGamepadState eGamepadState;

	    //
	    // The idle timeout control structure for our input report.  This is
	    // required by the lower level HID driver.
	    //
	    tHIDReportIdle sReportIdle;

	    //
	    // The lower level HID driver's instance data.
	    //
	    tHIDInstance sHIDInstance;

	    //
	    // This is needed for the lower level HID driver.
	    //
	    tUSBDHIDDevice sHIDDevice;
	}
	tHIDGamepadInstance;

	typedef struct
	{
	    //
	    //! The vendor ID that this device is to present in the device descriptor.
	    //
	    unsigned short usVID;

	    //
	    //! The product ID that this device is to present in the device descriptor.
	    //
	    unsigned short usPID;

	    //
	    //! The maximum power consumption of the device, expressed in milliamps.
	    //
	    unsigned short usMaxPowermA;

	    //
	    //! Indicates whether the device is self- or bus-powered and whether or not
	    //! it supports remote wakeup.  Valid values are USB_CONF_ATTR_SELF_PWR or
	    //! USB_CONF_ATTR_BUS_PWR, optionally ORed with USB_CONF_ATTR_RWAKE.
	    //
	    unsigned char ucPwrAttributes;

	    //
	    //! A pointer to the callback function which will be called to notify
	    //! the application of events relating to the operation of the mouse.
	    //
	    tUSBCallback pfnCallback;

	    //
	    //! A client-supplied pointer which will be sent as the first
	    //! parameter in all calls made to the gamepad callback, pfnCallback.
	    //
	    void *pvCBData;

	    //
	    //! A pointer to the string descriptor array for this device.  This array
	    //! must contain the following string descriptor pointers in this order.
	    //! Language descriptor, Manufacturer name string (language 1), Product
	    //! name string (language 1), Serial number string (language 1),HID
	    //! Interface description string (language 1), Configuration description
	    //! string (language 1).
	    //!
	    //! If supporting more than 1 language, the descriptor block (except for
	    //! string descriptor 0) must be repeated for each language defined in the
	    //! language descriptor.
	    //
	    const unsigned char * const *ppStringDescriptors;

	    //
	    //! The number of descriptors provided in the ppStringDescriptors
	    //! array.  This must be (1 + (5 * (num languages))).
	    //
	    unsigned long ulNumStringDescriptors;

	    //
	    //! A pointer to private instance data for this device.  This memory must
	    //! remain accessible for as long as the mouse device is in use and must
	    //! not be modified by any code outside the HID mouse driver.
	    //
	    tHIDGamepadInstance *psPrivateHIDGamepadData;

	    //
	    //
	    //
	    const unsigned char * const *ppClassDescriptors;

	    //
	    //
	    //
	    const tHIDDescriptor * const sHIDDescriptor;
	}
	tUSBDHIDGamepadDevice;

	#define GAMEPAD_SUCCESS            0
	#define GAMEPAD_ERR_TX_ERROR       2
	#define GAMEPAD_ERR_NOT_CONFIGURED 4

	extern void *USBDHIDGamepadInit(unsigned long ulIndex, const tUSBDHIDGamepadDevice *psDevice, unsigned char *pucReport, unsigned char *pucDataBuffer);
	extern void *USBDHIDGamepadCompositeInit(unsigned long ulIndex, const tUSBDHIDGamepadDevice *psDevice);
	extern void USBDHIDGamepadTerm(void *pvInstance);
	extern void *USBDHIDGamepadSetCBData(void *pvInstance, void *pvCBData);
	extern unsigned long USBDHIDGamepadStateChange(void *pvInstance, unsigned long ulReportSize);
	extern void USBDHIDGamepadPowerStatusSet(void *pvInstance, unsigned char ucPower);
	extern tBoolean USBDHIDGamepadRemoteWakeupRequest(void *pvInstance);

#ifdef __cplusplus
}
#endif

#endif /* USBHIDGAMEPAD_H_ */
