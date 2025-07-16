#pragma once

#define CINTERFACE
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

extern IDirectInputDevice8AVtbl RealDirectInputDevice8VtblA;
extern IDirectInputDevice8WVtbl RealDirectInputDevice8VtblW;

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

/*
 * Collect device information for a DirectInput device.
 * This must be called after IDirectInputDevice8 is hooked
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 */
void CollectDeviceInfoA(const LPDIRECTINPUTDEVICE8A lpDirectInputDevice,
                        const GUID rguid);

/*
 * Collect device information for a DirectInput device.
 * This must be called after IDirectInputDevice8 is hooked
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 */
void CollectDeviceInfoW(const LPDIRECTINPUTDEVICE8W lpDirectInputDevice,
                        const GUID rguid);