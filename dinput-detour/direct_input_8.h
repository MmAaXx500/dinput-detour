#pragma once

#define CINTERFACE
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

extern IDirectInput8AVtbl RealDirectInput8VtblA;
extern IDirectInput8WVtbl RealDirectInput8VtblW;

HRESULT WINAPI RoutedDirectInput8CreateDeviceA(
    LPDIRECTINPUT8A lpDI, REFGUID rguid,
    LPDIRECTINPUTDEVICE8A *lplpDirectInputDevice, LPUNKNOWN pUnkOuter);
HRESULT WINAPI RoutedDirectInput8CreateDeviceW(
    LPDIRECTINPUT8W lpDI, REFGUID rguid,
    LPDIRECTINPUTDEVICE8W *lplpDirectInputDevice, LPUNKNOWN pUnkOuter);