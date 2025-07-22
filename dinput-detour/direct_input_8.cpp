#include "direct_input_8.h"
#include "direct_input_device_8.h"
#include "log.h"
#include "stringconv.h"
#include "utils.h"

template <typename IDInput>
DITraits<IDInput>::DInputVtbl RealDirectInput8Vtbl = {};

template <typename IDInput> struct EnumDevicesCallbackData {
	DITraits<IDInput>::DIEnumDevicesCallback realCallback;
	PVOID realData;
};

template <typename IDInput>
HRESULT WINAPI DirectInput8CreateDevice(
    typename DITraits<IDInput>::DInput *lpDI, REFGUID rguid,
    typename DITraits<IDInput>::DIDevice **lplpDirectInputDevice,
    LPUNKNOWN pUnkOuter) {
	LOG_PRE_T(IDInput,
	          "lpDI: {} guid: {}, lplpDirectInputDevice: {}, pUnkOuter: {}\n",
	          static_cast<void *>(lpDI), GUIDToString(rguid),
	          static_cast<void *>(lplpDirectInputDevice),
	          static_cast<void *>(pUnkOuter));

	DWORD ret = RealDirectInput8Vtbl<IDInput>.CreateDevice(
	    lpDI, rguid, lplpDirectInputDevice, pUnkOuter);

	if (SUCCEEDED(ret)) {
		DirectInputDevice8DetourAttach<IDInput>(*lplpDirectInputDevice, rguid);
	}

	LOG_POST_T(IDInput, "ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
BOOL WINAPI EnumDevicesCallback(
    const typename DITraits<IDInput>::DIDeviceInstance *lpddi, LPVOID pvref) {

	BOOL ret = DIENUM_CONTINUE;

	LOG_PRE_T(IDInput, "lpddi: {}, pvref: {}\n",
	          static_cast<const void *>(lpddi), pvref);

	LOG_INFO_T(IDInput, "lpddi: {}\n",
	           DIDEVICEINSTANCEToString<IDInput>(*lpddi));

	auto data = reinterpret_cast<EnumDevicesCallbackData<IDInput> *>(pvref);

	if (data == nullptr || data->realCallback == nullptr) {
		LOG_POST_T(IDInput, "ret: DIENUM_CONTINUE\n");
		return DIENUM_CONTINUE;
	}

	ret = data->realCallback(lpddi, data->realData);

	LOG_POST_T(IDInput, "ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInput8EnumDevices(
    typename DITraits<IDInput>::DInput *lpDI, DWORD dwDevType,
    typename DITraits<IDInput>::DIEnumDevicesCallback lpCallback, LPVOID pvRef,
    DWORD dwFlags) {
	LOG_PRE_T(IDInput,
	          "lpDI: {}, dwDevType: {} ({:#x}), lpCallback: {}, pvRef: {}, "
	          "dwFlags: {} ({:#x})\n",
	          static_cast<void *>(lpDI), DI8DEVTYPEOrCLASSToString(dwDevType),
	          dwDevType, reinterpret_cast<void *>(lpCallback), pvRef,
	          DIEDFLToString(dwFlags), dwFlags);

	EnumDevicesCallbackData<IDInput> data{
	    .realCallback = lpCallback,
	    .realData = pvRef,
	};

	HRESULT ret = RealDirectInput8Vtbl<IDInput>.EnumDevices(
	    lpDI, dwDevType, EnumDevicesCallback<IDInput>, &data, dwFlags);

	LOG_POST_T(IDInput, "ret: {}\n", ret);
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
			DetourAttach(&RealDirectInput8Vtbl<IDInput>.EnumDevices,
			             DirectInput8EnumDevices<IDInput>);
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
			DetourDetach(&RealDirectInput8Vtbl<IDInput>.EnumDevices,
			             DirectInput8EnumDevices<IDInput>);
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
