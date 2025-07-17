#define CINTERFACE

#include "direct_input_8.h"
#include "direct_input_device_8.h"
#include "log.h"

IDirectInput8AVtbl RealDirectInput8VtblA = {};
IDirectInput8WVtbl RealDirectInput8VtblW = {};

HRESULT WINAPI RoutedDirectInput8CreateDeviceA(
    LPDIRECTINPUT8A lpDI, REFGUID rguid,
    LPDIRECTINPUTDEVICE8A *lplpDirectInputDevice, LPUNKNOWN pUnkOuter) {
	LOG_PRE("lpDI: {} guid: {}, lplpDirectInputDevice: {}, pUnkOuter: {}\n",
	        static_cast<void *>(lpDI), guid_to_str(rguid),
	        static_cast<void *>(lplpDirectInputDevice),
	        static_cast<void *>(pUnkOuter));

	DWORD ret = RealDirectInput8VtblA.CreateDevice(
	    lpDI, rguid, lplpDirectInputDevice, pUnkOuter);

	if (SUCCEEDED(ret)) {
		DirectInputDevice8DetourAttachA(*lplpDirectInputDevice, rguid);
	}

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInput8CreateDeviceW(
    LPDIRECTINPUT8W lpDI, REFGUID rguid,
    LPDIRECTINPUTDEVICE8W *lplpDirectInputDevice, LPUNKNOWN pUnkOuter) {
	LOG_PRE("lpDI: {} guid: {}, lplpDirectInputDevice: {}, pUnkOuter: {}\n",
	        static_cast<void *>(lpDI), guid_to_str(rguid),
	        static_cast<void *>(lplpDirectInputDevice),
	        static_cast<void *>(pUnkOuter));

	DWORD ret = RealDirectInput8VtblW.CreateDevice(
	    lpDI, rguid, lplpDirectInputDevice, pUnkOuter);

	if (SUCCEEDED(ret)) {
		DirectInputDevice8DetourAttachW(*lplpDirectInputDevice, rguid);
	}

	LOG_POST("ret: {}\n", ret);
	return ret;
}