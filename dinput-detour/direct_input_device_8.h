#pragma once

#define CINTERFACE
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

HRESULT WINAPI RoutedDirectInputDevice8GetCapabilitiesA(
    LPDIRECTINPUTDEVICE8A lpDirectInputDevice, LPDIDEVCAPS lpDIDevCaps);

HRESULT WINAPI RoutedDirectInputDevice8GetCapabilitiesW(
    LPDIRECTINPUTDEVICE8W lpDirectInputDevice, LPDIDEVCAPS lpDIDevCaps);

HRESULT WINAPI
RoutedDirectInputDevice8GetPropertyA(LPDIRECTINPUTDEVICE8A lpDirectInputDevice,
                                     REFGUID rguidProp, LPDIPROPHEADER pdiph);
HRESULT WINAPI
RoutedDirectInputDevice8GetPropertyW(LPDIRECTINPUTDEVICE8W lpDirectInputDevice,
                                     REFGUID rguidProp, LPDIPROPHEADER pdiph);

HRESULT WINAPI
RoutedDirectInputDevice8SetPropertyA(LPDIRECTINPUTDEVICE8A lpDirectInputDevice,
                                     REFGUID rguidProp, LPCDIPROPHEADER pdiph);
HRESULT WINAPI
RoutedDirectInputDevice8SetPropertyW(LPDIRECTINPUTDEVICE8W lpDirectInputDevice,
                                     REFGUID rguidProp, LPCDIPROPHEADER pdiph);

HRESULT WINAPI RoutedDirectInputDevice8SetDataFormatA(
    LPDIRECTINPUTDEVICE8A lpDirectInputDevice, LPCDIDATAFORMAT lpdf);

HRESULT WINAPI RoutedDirectInputDevice8SetDataFormatW(
    LPDIRECTINPUTDEVICE8W lpDirectInputDevice, LPCDIDATAFORMAT lpdf);

HRESULT WINAPI RoutedDirectInputDevice8SetEventNotificationA(
    LPDIRECTINPUTDEVICE8A lpDirectInputDevice, HANDLE hEvent);

HRESULT WINAPI RoutedDirectInputDevice8SetEventNotificationW(
    LPDIRECTINPUTDEVICE8W lpDirectInputDevice, HANDLE hEvent);

HRESULT WINAPI RoutedDirectInputDevice8EnumEffectsA(
    LPDIRECTINPUTDEVICE8A lpDirectInputDevice,
    LPDIENUMEFFECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwEffType);

HRESULT WINAPI RoutedDirectInputDevice8EnumEffectsW(
    LPDIRECTINPUTDEVICE8W lpDirectInputDevice,
    LPDIENUMEFFECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwEffType);

HRESULT WINAPI RoutedDirectInputDevice8GetDeviceInfoA(
    LPDIRECTINPUTDEVICE8A lpDirectInputDevice, LPDIDEVICEINSTANCEA pdidi);

HRESULT WINAPI RoutedDirectInputDevice8GetDeviceInfoW(
    LPDIRECTINPUTDEVICE8W lpDirectInputDevice, LPDIDEVICEINSTANCEW pdidi);

HRESULT WINAPI RoutedDirectInputDevice8GetForceFeedbackStateA(
    LPDIRECTINPUTDEVICE8A lpDirectInputDevice, LPDWORD pdwOut);

HRESULT WINAPI RoutedDirectInputDevice8GetForceFeedbackStateW(
    LPDIRECTINPUTDEVICE8W lpDirectInputDevice, LPDWORD pdwOut);

/**
 * Attach to DirectInputDevice8 (A) functions
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 * @param rguid GUID of the device
 * @return Result of DetourTransaction
 */
LONG DirectInputDevice8DetourAttachA(LPDIRECTINPUTDEVICE8A lpDirectInputDevice,
                                     REFGUID rguid);

/**
 * Attach to DirectInputDevice8 (W) functions
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 * @param rguid GUID of the device
 * @return Result of DetourTransaction
 */
LONG DirectInputDevice8DetourAttachW(LPDIRECTINPUTDEVICE8W lpDirectInputDevice,
                                     REFGUID rguid);

/**
 * Detach from DirectInputDevice8 (A) functions
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 * @return Result of DetourTransaction
 */
LONG DirectInputDevice8DetourDetachA(LPDIRECTINPUTDEVICE8A lpDirectInputDevice);

/**
 * Detach from DirectInputDevice8 (W) functions
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 * @return Result of DetourTransaction
 */
LONG DirectInputDevice8DetourDetachW(LPDIRECTINPUTDEVICE8W lpDirectInputDevice);
