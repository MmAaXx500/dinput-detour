#include <unordered_set>

#include "direct_input_device_8.h"
#include "log.h"
#include "stringconv.h"
#include "utils.h"

using namespace std;

template <typename IDInput>
DITraits<IDInput>::DIDeviceVtbl RealDirectInputDevice8Vtbl = {};

namespace std {
template <> struct hash<GUID> {
	size_t operator()(const GUID &guid) const noexcept {
		const uint64_t *p = reinterpret_cast<const uint64_t *>(&guid);
		return std::hash<uint64_t>{}(p[0]) ^ std::hash<uint64_t>{}(p[1]);
	}
};
} // namespace std

static unordered_set<GUID> seenDevices;

template <typename IDInput> struct EnumEffectsCallbackData {
	DITraits<IDInput>::DIEnumEffectsCallback realCallback;
	PVOID realData;
};

template <typename IDInput>
HRESULT DirectInputDevice8CreateEffect(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice, REFGUID rguid,
    LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT *ppdeff, LPUNKNOWN punkOuter) {
	using EffectInfo = DITraits<IDInput>::DIEffectInfo;

	LOG_PRE("lpDirectInputDevice: {}, rguid: {}, lpeff: {}, ppdeff: {}, "
	        "punkOuter: {}\n",
	        static_cast<void *>(lpDirectInputDevice), guid_to_str(rguid),
	        static_cast<const void *>(lpeff), static_cast<void *>(ppdeff),
	        static_cast<void *>(punkOuter));

	EffectInfo effinfo = {};
	effinfo.dwSize = sizeof(EffectInfo);
	HRESULT hr = RealDirectInputDevice8Vtbl<IDInput>.GetEffectInfo(
	    lpDirectInputDevice, &effinfo, rguid);
	if (FAILED(hr))
		effinfo.dwEffType = 0;

	LOG_INFO("lpeff: {}", DIEFFECTToString(lpeff, effinfo.dwEffType));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.CreateEffect(
	    lpDirectInputDevice, rguid, lpeff, ppdeff, punkOuter);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8GetCapabilities(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    LPDIDEVCAPS lpDIDevCaps) {
	LOG_PRE("lpDirectInputDevice: {}, lpDIDevCaps: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<void *>(lpDIDevCaps));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.GetCapabilities(
	    lpDirectInputDevice, lpDIDevCaps);

	if (SUCCEEDED(ret) && lpDIDevCaps)
		LOG_INFO("{}\n", DIDEVCAPSToString(*lpDIDevCaps));

	LOG_POST("ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8GetProperty(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    REFGUID rguidProp, LPDIPROPHEADER pdiph) {
	LOG_PRE("lpDirectInputDevice: {}, rguidProp: {}, pdiph: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<const void *>(&rguidProp), static_cast<void *>(pdiph));

	LOG_INFO("rguidProp: {}\n", DIPROPToString(rguidProp));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.GetProperty(
	    lpDirectInputDevice, rguidProp, pdiph);

	if (SUCCEEDED(ret))
		LOG_INFO("pdiph: {}\n", DIPROPHEADERToString(rguidProp, *pdiph));

	LOG_POST("ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8SetProperty(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    REFGUID rguidProp, LPCDIPROPHEADER pdiph) {
	LOG_PRE("lpDirectInputDevice: {}, rguidProp: {}, pdiph: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<const void *>(&rguidProp),
	        static_cast<const void *>(pdiph));

	LOG_INFO("rguidProp: {}\n", DIPROPToString(rguidProp));

	LOG_INFO("pdiph: {}\n", DIPROPHEADERToString(rguidProp, *pdiph));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.SetProperty(
	    lpDirectInputDevice, rguidProp, pdiph);
	LOG_POST("ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8SetDataFormat(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    LPCDIDATAFORMAT lpdf) {
	LOG_PRE("lpDirectInputDevice: {}, lpdf: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<const void *>(lpdf));

	LOG_INFO("lpdf: {}\n", DIDATAFORMATToString(lpdf));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.SetDataFormat(
	    lpDirectInputDevice, lpdf);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8SetEventNotification(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice, HANDLE hEvent) {
	LOG_PRE("lpDirectInputDevice: {}, hEvent: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<void *>(hEvent));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.SetEventNotification(
	    lpDirectInputDevice, hEvent);

	LOG_POST("ret: {}\n", ret);

	return ret;
}

template <typename IDInput>
BOOL WINAPI EnumEffectsCallback(
    const typename DITraits<IDInput>::DIEffectInfo *pdei, LPVOID pvRef) {
	using DIEffectInfo = DITraits<IDInput>::DIEffectInfo;

	LOG_PRE("pdei: {}, pvRef: {}\n", reinterpret_cast<const void *>(pdei),
	        pvRef);

	EnumEffectsCallbackData<IDInput> *data =
	    reinterpret_cast<EnumEffectsCallbackData<IDInput> *>(pvRef);

	LOG_INFO("pdei dwSize: {}", pdei->dwSize);
	if (pdei->dwSize >= offsetof(DIEffectInfo, guid) + sizeof(pdei->guid))
		LOG(", guid: {}", guid_to_str(pdei->guid));
	if (pdei->dwSize
	    >= offsetof(DIEffectInfo, dwEffType) + sizeof(pdei->dwEffType))
		LOG(", dwEffType: {}({:#x})", DIEFTToString(pdei->dwEffType),
		    pdei->dwEffType);
	if (pdei->dwSize >= offsetof(DIEffectInfo, dwStaticParams)
	                        + sizeof(pdei->dwStaticParams))
		LOG(", dwStaticParams: {}({:#x})", DIEPToString(pdei->dwStaticParams),
		    pdei->dwStaticParams);
	if (pdei->dwSize >= offsetof(DIEffectInfo, dwDynamicParams)
	                        + sizeof(pdei->dwDynamicParams))
		LOG(", dwDynamicParams: {}({:#x})", DIEPToString(pdei->dwDynamicParams),
		    pdei->dwDynamicParams);
	if (pdei->dwSize >= offsetof(DIEffectInfo, tszName) + sizeof(pdei->tszName))
		LOG(", tszName: {}", ToString(pdei->tszName));

	LOG("\n");

	if (data == nullptr || data->realCallback == nullptr) {
		return DIENUM_CONTINUE;
	}

	BOOL ret = data->realCallback(pdei, data->realData);

	LOG_POST("ret: {}\n", ret);

	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8EnumEffects(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    typename DITraits<IDInput>::DIEnumEffectsCallback lpCallback, LPVOID pvRef,
    DWORD dwEffType) {
	LOG_PRE("lpDirectInputDevice: {}, lpCallback: {}, pvRef: {}, "
	        "dwEffType: {:x}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        reinterpret_cast<void *>(lpCallback), pvRef, dwEffType);

	LOG_INFO("dwEffType: {}\n", DIEFTToString(dwEffType));

	EnumEffectsCallbackData<IDInput> data{
	    .realCallback = lpCallback,
	    .realData = pvRef,
	};

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.EnumEffects(
	    lpDirectInputDevice, EnumEffectsCallback<IDInput>, &data, dwEffType);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8GetDeviceInfo(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    typename DITraits<IDInput>::DIDeviceInstance *pdidi) {
	using DIDeviceInstance = DITraits<IDInput>::DIDeviceInstance;

	LOG_PRE("lpDirectInputDevice: {}, pdidi: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<void *>(pdidi));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.GetDeviceInfo(
	    lpDirectInputDevice, pdidi);

	if (SUCCEEDED(ret)) {
		LOG_INFO("pdidi dwSize: {}", pdidi->dwSize);

		if (pdidi->dwSize >= offsetof(DIDeviceInstance, guidInstance)
		                         + sizeof(pdidi->guidInstance))
			LOG(", guidInstance: {}", guid_to_str(pdidi->guidInstance));

		if (pdidi->dwSize >= offsetof(DIDeviceInstance, guidProduct)
		                         + sizeof(pdidi->guidProduct))
			LOG(", guidProduct: {}", guid_to_str(pdidi->guidProduct));

		if (pdidi->dwSize
		    >= offsetof(DIDeviceInstance, dwDevType) + sizeof(pdidi->dwDevType))
			LOG(", dwDevType: {:#x}", pdidi->dwDevType);

		if (pdidi->dwSize >= offsetof(DIDeviceInstance, tszInstanceName)
		                         + sizeof(pdidi->tszInstanceName))
			LOG(", tszInstanceName: {}", ToString(pdidi->tszInstanceName));

		if (pdidi->dwSize >= offsetof(DIDeviceInstance, tszProductName)
		                         + sizeof(pdidi->tszProductName))
			LOG(", tszProductName: {}", ToString(pdidi->tszProductName));

		if (pdidi->dwSize >= offsetof(DIDeviceInstance, guidFFDriver)
		                         + sizeof(pdidi->guidFFDriver))
			LOG(", guidFFDriver: {}", guid_to_str(pdidi->guidFFDriver));

		if (pdidi->dwSize >= offsetof(DIDeviceInstance, wUsagePage)
		                         + sizeof(pdidi->wUsagePage))
			LOG(", wUsagePage: {:#x}", pdidi->wUsagePage);

		if (pdidi->dwSize
		    >= offsetof(DIDeviceInstance, wUsage) + sizeof(pdidi->wUsage))
			LOG(", wUsage: {:#x}", pdidi->wUsage);

		LOG("\n");
	}

	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8GetForceFeedbackState(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice, LPDWORD pdwOut) {
	LOG_PRE("lpDirectInputDevice: {}, pdwOut: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<void *>(pdwOut));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.GetForceFeedbackState(
	    lpDirectInputDevice, pdwOut);

	if (SUCCEEDED(ret) && pdwOut)
		DIGFFSToString(*pdwOut);

	LOG_POST("ret: {}\n", ret);

	return ret;
}

/*
 * Collect device information for a DirectInput device.
 * This must be called after IDirectInputDevice8 is hooked
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 */
template <typename IDInput>
static void
CollectDeviceInfo(typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
                  const GUID rguid) {
	using DIDeviceInstance = DITraits<IDInput>::DIDeviceInstance;
	if (seenDevices.contains(rguid))
		return;

	LOG_PRE("Collecting device info for: {}\n", guid_to_str(rguid));

	seenDevices.insert(rguid);

	DIDeviceInstance deviceInfo = {};
	deviceInfo.dwSize = sizeof(DIDeviceInstance);
	lpDirectInputDevice->lpVtbl->GetDeviceInfo(lpDirectInputDevice,
	                                           &deviceInfo);

	DIDEVCAPS devCaps = {};
	devCaps.dwSize = sizeof(DIDEVCAPS);
	lpDirectInputDevice->lpVtbl->GetCapabilities(lpDirectInputDevice, &devCaps);

	RealDirectInputDevice8Vtbl<IDInput>.EnumEffects(
	    lpDirectInputDevice, EnumEffectsCallback<IDInput>, nullptr, DIEFT_ALL);

	LOG_POST("Collection ended for: {}\n", guid_to_str(rguid));
}

template <typename IDInput>
LONG DirectInputDevice8DetourAttach(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice, REFGUID rguid) {
	LONG ret = NO_ERROR;

	if (RealDirectInputDevice8Vtbl<IDInput>.AddRef == nullptr
	    && lpDirectInputDevice) {
		RealDirectInputDevice8Vtbl<IDInput> = *lpDirectInputDevice->lpVtbl;

		ret = DetourTransaction([]() {
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.CreateEffect,
			             DirectInputDevice8CreateEffect<IDInput>);
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.GetCapabilities,
			             DirectInputDevice8GetCapabilities<IDInput>);
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.GetProperty,
			             DirectInputDevice8GetProperty<IDInput>);
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.SetProperty,
			             DirectInputDevice8SetProperty<IDInput>);
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.SetDataFormat,
			             DirectInputDevice8SetDataFormat<IDInput>);
			DetourAttach(
			    &RealDirectInputDevice8Vtbl<IDInput>.SetEventNotification,
			    DirectInputDevice8SetEventNotification<IDInput>);
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.EnumEffects,
			             DirectInputDevice8EnumEffects<IDInput>);
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.GetDeviceInfo,
			             DirectInputDevice8GetDeviceInfo<IDInput>);
			DetourAttach(
			    &RealDirectInputDevice8Vtbl<IDInput>.GetForceFeedbackState,
			    DirectInputDevice8GetForceFeedbackState<IDInput>);
		});
	}

	if (ret == NO_ERROR)
		CollectDeviceInfo<IDInput>(lpDirectInputDevice, rguid);

	return ret;
}

template <typename IDInput>
LONG DirectInputDevice8DetourDetach(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice) {
	LONG ret = ERROR_INVALID_OPERATION;

	if (RealDirectInputDevice8Vtbl<IDInput>.AddRef != nullptr
	    && lpDirectInputDevice) {
		ret = DetourTransaction([]() {
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.CreateEffect,
			             DirectInputDevice8CreateEffect<IDInput>);
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.GetCapabilities,
			             DirectInputDevice8GetCapabilities<IDInput>);
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.GetProperty,
			             DirectInputDevice8GetProperty<IDInput>);
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.SetProperty,
			             DirectInputDevice8SetProperty<IDInput>);
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.SetDataFormat,
			             DirectInputDevice8SetDataFormat<IDInput>);
			DetourDetach(
			    &RealDirectInputDevice8Vtbl<IDInput>.SetEventNotification,
			    DirectInputDevice8SetEventNotification<IDInput>);
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.EnumEffects,
			             DirectInputDevice8EnumEffects<IDInput>);
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.GetDeviceInfo,
			             DirectInputDevice8GetDeviceInfo<IDInput>);
			DetourDetach(
			    &RealDirectInputDevice8Vtbl<IDInput>.GetForceFeedbackState,
			    DirectInputDevice8GetForceFeedbackState<IDInput>);
		});

		memset(&RealDirectInputDevice8Vtbl<IDInput>, 0,
		       sizeof(RealDirectInputDevice8Vtbl<IDInput>));
	}

	return ret;
}

template LONG DirectInputDevice8DetourAttach<IDirectInput8A>(
    typename DITraits<IDirectInput8A>::DIDevice *lpDirectInputDevice,
    REFGUID rguid);

template LONG DirectInputDevice8DetourAttach<IDirectInput8W>(
    typename DITraits<IDirectInput8W>::DIDevice *lpDirectInputDevice,
    REFGUID rguid);

template LONG DirectInputDevice8DetourDetach<IDirectInput8A>(
    typename DITraits<IDirectInput8A>::DIDevice *lpDirectInputDevice);

template LONG DirectInputDevice8DetourDetach<IDirectInput8W>(
    typename DITraits<IDirectInput8W>::DIDevice *lpDirectInputDevice);