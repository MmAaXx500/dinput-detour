#include <winerror.h>
#define CINTERFACE

#include "direct_input_8.h"
#include "direct_input_device_8.h"
#include "log.h"
#include "utils.h"

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
		if (RealDirectInputDevice8VtblA.AddRef == nullptr) {
			RealDirectInputDevice8VtblA = *((*lplpDirectInputDevice)->lpVtbl);

			DetourTransaction([]() {
				DetourAttach(&RealDirectInputDevice8VtblA.GetCapabilities,
				             RoutedDirectInputDevice8GetCapabilitiesA);
				DetourAttach(&RealDirectInputDevice8VtblA.GetProperty,
				             RoutedDirectInputDevice8GetPropertyA);
				DetourAttach(&RealDirectInputDevice8VtblA.SetProperty,
				             RoutedDirectInputDevice8SetPropertyA);
				DetourAttach(&RealDirectInputDevice8VtblA.SetDataFormat,
				             RoutedDirectInputDevice8SetDataFormatA);
				DetourAttach(&RealDirectInputDevice8VtblA.SetEventNotification,
				             RoutedDirectInputDevice8SetEventNotificationA);
				DetourAttach(&RealDirectInputDevice8VtblA.EnumEffects,
				             RoutedDirectInputDevice8EnumEffectsA);
				DetourAttach(&RealDirectInputDevice8VtblA.GetDeviceInfo,
				             RoutedDirectInputDevice8GetDeviceInfoA);
			});
		}

		CollectDeviceInfoA(*lplpDirectInputDevice, rguid);
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
		if (RealDirectInputDevice8VtblW.AddRef == nullptr) {
			RealDirectInputDevice8VtblW = *((*lplpDirectInputDevice)->lpVtbl);

			DetourTransaction([]() {
				DetourAttach(&RealDirectInputDevice8VtblW.GetCapabilities,
				             RoutedDirectInputDevice8GetCapabilitiesW);
				DetourAttach(&RealDirectInputDevice8VtblW.GetProperty,
				             RoutedDirectInputDevice8GetPropertyW);
				DetourAttach(&RealDirectInputDevice8VtblW.SetProperty,
				             RoutedDirectInputDevice8SetPropertyW);
				DetourAttach(&RealDirectInputDevice8VtblW.SetDataFormat,
				             RoutedDirectInputDevice8SetDataFormatW);
				DetourAttach(&RealDirectInputDevice8VtblW.SetEventNotification,
				             RoutedDirectInputDevice8SetEventNotificationW);
				DetourAttach(&RealDirectInputDevice8VtblW.EnumEffects,
				             RoutedDirectInputDevice8EnumEffectsW);
				DetourAttach(&RealDirectInputDevice8VtblW.GetDeviceInfo,
				             RoutedDirectInputDevice8GetDeviceInfoW);
			});
		}

		CollectDeviceInfoW(*lplpDirectInputDevice, rguid);
	}

	LOG_POST("ret: {}\n", ret);
	return ret;
}