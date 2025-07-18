#include "direct_input_8.h"
#include "direct_input_device_8.h"
#include "log.h"
#include "utils.h"

template <typename IDInput>
DITraits<IDInput>::DInputVtbl RealDirectInput8Vtbl = {};

template <typename IDInput>
HRESULT WINAPI DirectInput8CreateDevice(
    typename DITraits<IDInput>::DInput *lpDI, REFGUID rguid,
    typename DITraits<IDInput>::DIDevice **lplpDirectInputDevice,
    LPUNKNOWN pUnkOuter) {
	LOG_PRE("lpDI: {} guid: {}, lplpDirectInputDevice: {}, pUnkOuter: {}\n",
	        static_cast<void *>(lpDI), guid_to_str(rguid),
	        static_cast<void *>(lplpDirectInputDevice),
	        static_cast<void *>(pUnkOuter));

	DWORD ret = RealDirectInput8Vtbl<IDInput>.CreateDevice(
	    lpDI, rguid, lplpDirectInputDevice, pUnkOuter);

	if (SUCCEEDED(ret)) {
		DirectInputDevice8DetourAttach<IDInput>(*lplpDirectInputDevice, rguid);
	}

	LOG_POST("ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
LONG DirectInput8DetourAttach(typename DITraits<IDInput>::DInput *lpDI) {
	LONG ret = NO_ERROR;

	if (RealDirectInput8Vtbl<IDInput>.AddRef == nullptr && lpDI) {
		RealDirectInput8Vtbl<IDInput> = *lpDI->lpVtbl;

		ret = DetourTransaction([]() {
			DetourAttach(&RealDirectInput8Vtbl<IDInput>.CreateDevice,
			             DirectInput8CreateDevice<IDInput>);
		});
	}

	return ret;
}

template <typename IDInput>
LONG DirectInput8DetourDetach(typename DITraits<IDInput>::DInput *lpDI) {
	LONG ret = ERROR_INVALID_OPERATION;

	if (RealDirectInput8Vtbl<IDInput>.AddRef != nullptr && lpDI) {
		RealDirectInput8Vtbl<IDInput> = *lpDI->lpVtbl;

		ret = DetourTransaction([]() {
			DetourDetach(&RealDirectInput8Vtbl<IDInput>.CreateDevice,
			             DirectInput8CreateDevice<IDInput>);
		});

		RealDirectInput8Vtbl<IDInput> = {};
	}

	return ret;
}

template LONG DirectInput8DetourAttach<IDirectInput8A>(
    typename DITraits<IDirectInput8A>::DInput *lpDI);

template LONG DirectInput8DetourAttach<IDirectInput8W>(
    typename DITraits<IDirectInput8W>::DInput *lpDI);

template LONG DirectInput8DetourDetach<IDirectInput8A>(
    typename DITraits<IDirectInput8A>::DInput *lpDI);

template LONG DirectInput8DetourDetach<IDirectInput8W>(
    typename DITraits<IDirectInput8W>::DInput *lpDI);
