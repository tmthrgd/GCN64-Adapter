/*
 * usbhidgamepad.c
 *
 *  Created on: 16/12/2012
 *      Author: Tom
 */

#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/usb.h"
#include "usblib/usblib.h"
#include "usblib/device/usbdevice.h"
#include "usblib/usbhid.h"
#include "usblib/device/usbdhid.h"
#include "usbhidgamepad.h"

static unsigned long HIDGamepadRxHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgData, void *pvMsgData)
{
    tHIDGamepadInstance *psInst;
    tUSBDHIDGamepadDevice *psDevice;

    ASSERT(pvCBData);

    psDevice = (tUSBDHIDGamepadDevice *)pvCBData;
    psInst = psDevice->psPrivateHIDGamepadData;

    switch (ulEvent)
    {
        case USB_EVENT_CONNECTED:
        {
            psInst->ucUSBConfigured = true;

            psDevice->pfnCallback(psDevice->pvCBData, USB_EVENT_CONNECTED, 0, (void *)0);
            break;
        }
        case USB_EVENT_DISCONNECTED:
        {
            psInst->ucUSBConfigured = false;

            psDevice->pfnCallback(psDevice->pvCBData, USB_EVENT_DISCONNECTED, 0, (void *)0);
            break;
        }
        case USBD_HID_EVENT_IDLE_TIMEOUT:
        case USBD_HID_EVENT_GET_REPORT:
        {
            *(unsigned char **)pvMsgData = psInst->pucReport;
            return psInst->ulReportSize;
        }
        case USBD_HID_EVENT_REPORT_SENT:
        {
            break;
        }
        case USBD_HID_EVENT_GET_REPORT_BUFFER:
        {
        	/*if ((unsigned long)pvMsgData <= psInt->ulDataReportSize)
        	{*/
        		return (unsigned long)psInst->pucDataBuffer;
        	/*}
        	else
        	{
        		return 0;
        	}*/
        }
        case USBD_HID_EVENT_SET_REPORT:
        {
        	psDevice->pfnCallback(psDevice->pvCBData, USBD_HID_EVENT_SET_REPORT, 0, psInst->pucDataBuffer);
        	break;
        }
        case USB_EVENT_ERROR:
        case USB_EVENT_SUSPEND:
        case USB_EVENT_RESUME:
        {
            return psDevice->pfnCallback(psDevice->pvCBData, ulEvent, ulMsgData, pvMsgData);
        }
        default:
        {
            break;
        }
    }

    return 0;
}

static unsigned long HIDGamepadTxHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgData, void *pvMsgData)
{
	tHIDGamepadInstance *psInst;
    tUSBDHIDGamepadDevice *psDevice;

    ASSERT(pvCBData);

    psDevice = (tUSBDHIDGamepadDevice *)pvCBData;
    psInst = psDevice->psPrivateHIDGamepadData;

    switch (ulEvent)
    {
        case USB_EVENT_TX_COMPLETE:
        {
            psInst->eGamepadState = HID_GAMEPAD_STATE_IDLE;

            psDevice->pfnCallback(psDevice->pvCBData, USB_EVENT_TX_COMPLETE, ulMsgData, (void *)0);
            break;
        }
        default:
        {
            break;
        }
    }

    return 0;
}

void *USBDHIDGamepadInit(unsigned long ulIndex, const tUSBDHIDGamepadDevice *psDevice, unsigned char *pucReport, unsigned char *pucDataBuffer)
{
    void *pvRetcode;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(psDevice);
    ASSERT(psDevice->ppStringDescriptors);
    ASSERT(psDevice->psPrivateHIDGamepadData);
    ASSERT(psDevice->pfnCallback);
    ASSERT(pucReport);
    ASSERT(pucDataBuffer);

    psDevice->psPrivateHIDGamepadData->pucReport = pucReport;
    psDevice->psPrivateHIDGamepadData->pucDataBuffer = pucDataBuffer;

    psHIDDevice = &psDevice->psPrivateHIDGamepadData->sHIDDevice;

    pvRetcode = USBDHIDGamepadCompositeInit(ulIndex, psDevice);

    if (pvRetcode)
    {
        pvRetcode = USBDHIDInit(ulIndex, psHIDDevice);
        return (void *)psDevice;
    }
    else
    {
        return (void *)0;
    }
}

void *USBDHIDGamepadCompositeInit(unsigned long ulIndex, const tUSBDHIDGamepadDevice *psDevice)
{
    tHIDGamepadInstance *psInst;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(psDevice);
    ASSERT(psDevice->ppStringDescriptors);
    ASSERT(psDevice->psPrivateHIDGamepadData);
    ASSERT(psDevice->pfnCallback);

    psInst = psDevice->psPrivateHIDGamepadData;

    psHIDDevice = &psDevice->psPrivateHIDGamepadData->sHIDDevice;

    psInst->ucUSBConfigured = 0;
    psInst->sReportIdle.ucDuration4mS = 0;
    psInst->sReportIdle.ucReportID = 0;
    psInst->sReportIdle.ulTimeSinceReportmS = 0;
    psInst->sReportIdle.usTimeTillNextmS = 0;
    psInst->eGamepadState = HID_GAMEPAD_STATE_UNCONFIGURED;

    psHIDDevice->usPID = psDevice->usPID;
    psHIDDevice->usVID = psDevice->usVID;
    psHIDDevice->usMaxPowermA = psDevice->usMaxPowermA;
    psHIDDevice->ucPwrAttributes = psDevice->ucPwrAttributes;
    psHIDDevice->ucSubclass = USB_HID_SCLASS_NONE;
    psHIDDevice->ucProtocol = USB_HID_PROTOCOL_NONE;
    psHIDDevice->ucNumInputReports = 1;
    psHIDDevice->psReportIdle = &psInst->sReportIdle;
    psHIDDevice->pfnRxCallback = HIDGamepadRxHandler;
    psHIDDevice->pvRxCBData = (void *)psDevice;
    psHIDDevice->pfnTxCallback = HIDGamepadTxHandler;
    psHIDDevice->pvTxCBData = (void *)psDevice;
    psHIDDevice->bUseOutEndpoint = false;
    psHIDDevice->psHIDDescriptor = psDevice->sHIDDescriptor;
    psHIDDevice->ppClassDescriptors = psDevice->ppClassDescriptors;
    psHIDDevice->ppStringDescriptors = psDevice->ppStringDescriptors;
    psHIDDevice->ulNumStringDescriptors = psDevice->ulNumStringDescriptors;
    psHIDDevice->psPrivateHIDData = &psInst->sHIDInstance;

    return USBDHIDCompositeInit(ulIndex, psHIDDevice);
}

void USBDHIDGamepadTerm(void *pvInstance)
{
    tUSBDHIDGamepadDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvInstance);

    psDevice = (tUSBDHIDGamepadDevice *)pvInstance;

    psHIDDevice = &psDevice->psPrivateHIDGamepadData->sHIDDevice;

    psDevice->psPrivateHIDGamepadData->ucUSBConfigured = 0;

    USBDHIDTerm(psHIDDevice);
}

void *USBDHIDGamepadSetCBData(void *pvInstance, void *pvCBData)
{
    void *pvOldCBData;
    tUSBDHIDGamepadDevice *psDevice;

    ASSERT(pvInstance);

    psDevice = (tUSBDHIDGamepadDevice *)pvInstance;

    pvOldCBData = psDevice->pvCBData;
    psDevice->pvCBData = pvCBData;

    return pvOldCBData;
}

unsigned long USBDHIDGamepadStateChange(void *pvInstance, unsigned long ulReportSize)
{
    unsigned long ulRetcode;
    unsigned long ulCount;
    tHIDGamepadInstance *psInst;
    tUSBDHIDGamepadDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(ulReportSize);

    psDevice = (tUSBDHIDGamepadDevice *)pvInstance;

    psDevice->psPrivateHIDGamepadData->ulReportSize = ulReportSize;

    psHIDDevice = &psDevice->psPrivateHIDGamepadData->sHIDDevice;

    psInst = psDevice->psPrivateHIDGamepadData;

    if (!psInst->ucUSBConfigured)
    {
        return GAMEPAD_ERR_NOT_CONFIGURED;
    }

    if (USBDHIDTxPacketAvailable((void *)psHIDDevice))
    {
        psInst->eGamepadState = HID_GAMEPAD_STATE_SEND;
        ulCount = USBDHIDReportWrite((void *)psHIDDevice, psInst->pucReport, ulReportSize, true);

        if (!ulCount)
        {
            ulRetcode = GAMEPAD_ERR_TX_ERROR;
        }
        else
        {
            ulRetcode = GAMEPAD_SUCCESS;
        }
    }
    else
    {
        ulRetcode = GAMEPAD_ERR_TX_ERROR;
    }

    return ulRetcode;
}

void USBDHIDGamepadPowerStatusSet(void *pvInstance, unsigned char ucPower)
{
    tUSBDHIDGamepadDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvInstance);

    psDevice = (tUSBDHIDGamepadDevice *)pvInstance;

    psHIDDevice = &psDevice->psPrivateHIDGamepadData->sHIDDevice;

    USBDHIDPowerStatusSet((void *)psHIDDevice, ucPower);
}

tBoolean USBDHIDGamepadRemoteWakeupRequest(void *pvInstance)
{
    tUSBDHIDGamepadDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvInstance);

    psDevice = (tUSBDHIDGamepadDevice *)pvInstance;

    psHIDDevice = &psDevice->psPrivateHIDGamepadData->sHIDDevice;

    return USBDHIDRemoteWakeupRequest((void *)&psHIDDevice);
}
