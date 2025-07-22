#include <unordered_map>
#include <unordered_set>

#include "direct_input_device_8.h"
#include "direct_input_effect.h"
#include "log.h"
#include "stringconv.h"
#include "utils.h"

using namespace std;

namespace std {
template <> struct hash<GUID> {
	size_t operator()(const GUID &guid) const noexcept {
		const uint64_t *p = reinterpret_cast<const uint64_t *>(&guid);
		return std::hash<uint64_t>{}(p[0]) ^ std::hash<uint64_t>{}(p[1]);
	}
};
} // namespace std

template <typename IDInput>
DITraits<IDInput>::DIDeviceVtbl RealDirectInputDevice8Vtbl = {};

template <typename IDInput>
unordered_map<GUID, typename DITraits<IDInput>::DIEffectInfo> EffectInfoCache;

static unordered_map<GUID, DIDATAFORMAT> DataFormatCache;

static unordered_set<GUID> seenDevices;

template <typename IDInput> struct EnumEffectsCallbackData {
	DITraits<IDInput>::DIEnumEffectsCallback realCallback;
	PVOID realData;
};

template <typename IDInput> struct EnumDeviceObjectsCallbackData {
	DITraits<IDInput>::DIEnumDeviceObjectCallback realCallback;
	PVOID realData;
};

template <typename IDInput>
HRESULT DirectInputDevice8CreateEffect(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice, REFGUID rguid,
    LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT *ppdeff, LPUNKNOWN punkOuter) {
	using EffectInfo = DITraits<IDInput>::DIEffectInfo;

	LOG_PRE_T(IDInput,
	          "lpDirectInputDevice: {}, rguid: {}, lpeff: {}, ppdeff: {}, "
	          "punkOuter: {}\n",
	          static_cast<void *>(lpDirectInputDevice), GUIDToString(rguid),
	          static_cast<const void *>(lpeff), static_cast<void *>(ppdeff),
	          static_cast<void *>(punkOuter));

	EffectInfo effinfo = {};
	effinfo.dwSize = sizeof(EffectInfo);
	HRESULT hr = RealDirectInputDevice8Vtbl<IDInput>.GetEffectInfo(
	    lpDirectInputDevice, &effinfo, rguid);
	if (FAILED(hr))
		effinfo.dwEffType = 0;

	LOG_INFO_T(IDInput, "lpeff: {}\n",
	           DIEFFECTToString(lpeff, effinfo.dwEffType));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.CreateEffect(
	    lpDirectInputDevice, rguid, lpeff, ppdeff, punkOuter);

	if (SUCCEEDED(hr))
		DirectInputEffectDetourAttach(*ppdeff);

	LOG_POST_T(IDInput, "ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8GetCapabilities(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    LPDIDEVCAPS lpDIDevCaps) {
	LOG_PRE_T(IDInput, "lpDirectInputDevice: {}, lpDIDevCaps: {}\n",
	          static_cast<void *>(lpDirectInputDevice),
	          static_cast<void *>(lpDIDevCaps));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.GetCapabilities(
	    lpDirectInputDevice, lpDIDevCaps);

	if (SUCCEEDED(ret) && lpDIDevCaps)
		LOG_INFO_T(IDInput, "lpDIDevCaps: {}\n",
		           DIDEVCAPSToString(*lpDIDevCaps));

	LOG_POST_T(IDInput, "ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
BOOL WINAPI EnumObjectsCallback(
    const typename DITraits<IDInput>::DIDeviceObjectInstance *lpddoi,
    LPVOID pvRef) {

	LOG_PRE_T(IDInput, "lpddoi: {}, pvRef: {}\n",
	          static_cast<const void *>(lpddoi), pvRef);

	LOG_INFO_T(IDInput, "lpddoi: {}\n",
	           DIDEVICEOBJECTINSTANCEToString<IDInput>(*lpddoi));

	auto data =
	    reinterpret_cast<EnumDeviceObjectsCallbackData<IDInput> *>(pvRef);

	if (data == nullptr || data->realCallback == nullptr) {
		LOG_POST_T(IDInput, "ret: DIENUM_CONTINUE\n");
		return DIENUM_CONTINUE;
	}

	BOOL ret = data->realCallback(lpddoi, data->realData);

	LOG_POST_T(IDInput, "ret: {}\n", ret);

	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8EnumObjects(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    typename DITraits<IDInput>::DIEnumDeviceObjectCallback lpCallback,
    LPVOID pvRef, DWORD dwFlags) {
	LOG_PRE_T(IDInput,
	          "lpDirectInputDevice: {}, lpCallback: {}, pvRef: {}, "
	          "dwFlags: {} ({:#x})\n",
	          static_cast<void *>(lpDirectInputDevice),
	          reinterpret_cast<void *>(lpCallback), pvRef,
	          DIDFTToString(dwFlags), dwFlags);

	EnumDeviceObjectsCallbackData<IDInput> data{
	    .realCallback = lpCallback,
	    .realData = pvRef,
	};

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.EnumObjects(
	    lpDirectInputDevice, EnumObjectsCallback<IDInput>, &data, dwFlags);

	LOG_POST_T(IDInput, "ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8GetProperty(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    REFGUID rguidProp, LPDIPROPHEADER pdiph) {
	LOG_PRE_T(
	    IDInput, "lpDirectInputDevice: {}, rguidProp: {} ({}), pdiph: {}\n",
	    static_cast<void *>(lpDirectInputDevice), DIPROPToString(rguidProp),
	    static_cast<const void *>(&rguidProp), static_cast<void *>(pdiph));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.GetProperty(
	    lpDirectInputDevice, rguidProp, pdiph);

	if (SUCCEEDED(ret))
		LOG_INFO_T(IDInput, "pdiph: {}\n",
		           DIPROPHEADERToString(rguidProp, *pdiph));

	LOG_POST_T(IDInput, "ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8SetProperty(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    REFGUID rguidProp, LPCDIPROPHEADER pdiph) {
	LOG_PRE_T(IDInput,
	          "lpDirectInputDevice: {}, rguidProp: {} ({}), pdiph: {}\n",
	          static_cast<void *>(lpDirectInputDevice),
	          DIPROPToString(rguidProp), static_cast<const void *>(&rguidProp),
	          static_cast<const void *>(pdiph));

	LOG_INFO_T(IDInput, "pdiph: {}\n", DIPROPHEADERToString(rguidProp, *pdiph));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.SetProperty(
	    lpDirectInputDevice, rguidProp, pdiph);
	LOG_POST_T(IDInput, "ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8GetDeviceState(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice, DWORD cbData,
    LPVOID lpvData) {
	using DIDeviceInstance = DITraits<IDInput>::DIDeviceInstance;

	LOG_PRE_T(IDInput, "lpDirectInputDevice: {}, cbData: {}, lpvData: {}\n",
	          static_cast<void *>(lpDirectInputDevice), cbData,
	          static_cast<void *>(lpvData));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.GetDeviceState(
	    lpDirectInputDevice, cbData, lpvData);

	if (SUCCEEDED(ret) && lpvData) {
		DIDeviceInstance dinst = {};
		dinst.dwSize = sizeof(DIDeviceInstance);
		HRESULT hr = RealDirectInputDevice8Vtbl<IDInput>.GetDeviceInfo(
		    lpDirectInputDevice, &dinst);

		if (SUCCEEDED(hr)) {
			if (DataFormatCache.contains(dinst.guidInstance)) {
				LPDIDATAFORMAT lpDf = &DataFormatCache[dinst.guidInstance];
				LOG_INFO_T(IDInput, "lpvData: ");
				if (*lpDf == c_dfDIJoystick)
					LOG("DIJOYSTATE: {}\n",
					    DIJOYSTATEToString(
					        *reinterpret_cast<DIJOYSTATE *>(lpvData)));
				else if (*lpDf == c_dfDIJoystick2)
					LOG("DIJOYSTATE2: {}\n",
					    DIJOYSTATE2ToString(
					        *reinterpret_cast<DIJOYSTATE2 *>(lpvData)));
				else {
					LOG("Unsupported format: {}\n", DIDATAFORMATToString(lpDf));
				}
			} else
				LOG_INFO_T(IDInput, "lpvData: DIDATAFORMAT not cached\n");
		}
	}

	LOG_POST_T(IDInput, "ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8GetDeviceData(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut,
    DWORD dwFlags) {
	using DIDeviceInstance = DITraits<IDInput>::DIDeviceInstance;

	LOG_PRE_T(IDInput,
	          "lpDirectInputDevice: {}, cbObjectData: {}, rgdod: {}, "
	          "pdwInOut: {}, dwFlags: {} ({:#x})\n",
	          static_cast<void *>(lpDirectInputDevice), cbObjectData,
	          static_cast<const void *>(rgdod), static_cast<void *>(pdwInOut),
	          DIGDDToString(dwFlags), dwFlags);

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.GetDeviceData(
	    lpDirectInputDevice, cbObjectData, rgdod, pdwInOut, dwFlags);

	if (SUCCEEDED(ret) && rgdod) {
		DIDeviceInstance dinst = {};
		dinst.dwSize = sizeof(DIDeviceInstance);
		HRESULT hr = RealDirectInputDevice8Vtbl<IDInput>.GetDeviceInfo(
		    lpDirectInputDevice, &dinst);

		if (SUCCEEDED(hr)) {
			if (DataFormatCache.contains(dinst.guidInstance))
				LOG_INFO_T(IDInput, "rgdod: {}\n",
				           DIDEVICEOBJECTDATAToString(
				               *rgdod, DataFormatCache[dinst.guidInstance]));
			else
				LOG_INFO_T(IDInput, "rgdod: DIDATAFORMAT not cached\n");
		}
	}

	LOG_POST_T(IDInput, "ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8SetDataFormat(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    LPCDIDATAFORMAT lpdf) {
	using DIDeviceInstance = DITraits<IDInput>::DIDeviceInstance;

	LOG_PRE_T(IDInput, "lpDirectInputDevice: {}, lpdf: {}\n",
	          static_cast<void *>(lpDirectInputDevice),
	          static_cast<const void *>(lpdf));

	LOG_INFO_T(IDInput, "lpdf: {}\n", DIDATAFORMATToString(lpdf));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.SetDataFormat(
	    lpDirectInputDevice, lpdf);

	if (SUCCEEDED(ret)) {
		DIDeviceInstance dinst = {};
		dinst.dwSize = sizeof(DIDeviceInstance);
		HRESULT hr = RealDirectInputDevice8Vtbl<IDInput>.GetDeviceInfo(
		    lpDirectInputDevice, &dinst);

		if (SUCCEEDED(hr)) {
			if (DataFormatCache.contains(dinst.guidInstance)) {
				FreeDIDATAFORMAT(&DataFormatCache[dinst.guidInstance]);
			}

			CopyDIDATAFORMAT(&DataFormatCache[dinst.guidInstance], *lpdf);
		}
	}

	LOG_POST_T(IDInput, "ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8SetEventNotification(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice, HANDLE hEvent) {
	LOG_PRE_T(IDInput, "lpDirectInputDevice: {}, hEvent: {}\n",
	          static_cast<void *>(lpDirectInputDevice),
	          static_cast<void *>(hEvent));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.SetEventNotification(
	    lpDirectInputDevice, hEvent);

	LOG_POST_T(IDInput, "ret: {}\n", ret);

	return ret;
}

template <typename IDInput>
BOOL WINAPI EnumEffectsCallback(
    const typename DITraits<IDInput>::DIEffectInfo *pdei, LPVOID pvRef) {
	using DIEffectInfo = DITraits<IDInput>::DIEffectInfo;

	LOG_PRE_T(IDInput, "pdei: {}, pvRef: {}\n",
	          reinterpret_cast<const void *>(pdei), pvRef);

	if (pdei->dwSize >= offsetof(DIEffectInfo, guid) + sizeof(pdei->guid))
		EffectInfoCache<IDInput>[pdei->guid] = *pdei;

	auto data = reinterpret_cast<EnumEffectsCallbackData<IDInput> *>(pvRef);

	LOG_INFO_T(IDInput, "pdei dwSize: {}", pdei->dwSize);
	if (pdei->dwSize >= offsetof(DIEffectInfo, guid) + sizeof(pdei->guid))
		LOG(", guid: {}", GUIDToString(pdei->guid));
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

	LOG_POST_T(IDInput, "ret: {}\n", ret);

	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8EnumEffects(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    typename DITraits<IDInput>::DIEnumEffectsCallback lpCallback, LPVOID pvRef,
    DWORD dwEffType) {
	LOG_PRE_T(IDInput,
	          "lpDirectInputDevice: {}, lpCallback: {}, pvRef: {}, "
	          "dwEffType: {} ({:#x})\n",
	          static_cast<void *>(lpDirectInputDevice),
	          reinterpret_cast<void *>(lpCallback), pvRef,
	          DIEFTToString(dwEffType), dwEffType);

	EnumEffectsCallbackData<IDInput> data{
	    .realCallback = lpCallback,
	    .realData = pvRef,
	};

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.EnumEffects(
	    lpDirectInputDevice, EnumEffectsCallback<IDInput>, &data, dwEffType);

	LOG_POST_T(IDInput, "ret: {}\n", ret);
	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8GetObjectInfo(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    typename DITraits<IDInput>::DIDeviceObjectInstance *pdidoi, DWORD dwObj,
    DWORD dwHow) {
	LOG_PRE_T(IDInput,
	          "lpDirectInputDevice: {}, pdidoi: {}, dwObj: {:#x}, dwHow: {} "
	          "({:#x})\n",
	          static_cast<void *>(lpDirectInputDevice),
	          static_cast<void *>(pdidoi), dwObj, DIPHHowToString(dwHow),
	          dwHow);

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.GetObjectInfo(
	    lpDirectInputDevice, pdidoi, dwObj, dwHow);

	if (SUCCEEDED(ret))
		LOG_INFO_T(IDInput, "pdidoi: {}\n",
		           DIDEVICEOBJECTINSTANCEToString<IDInput>(*pdidoi));

	LOG_POST_T(IDInput, "ret: {}\n", ret);

	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8GetDeviceInfo(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    typename DITraits<IDInput>::DIDeviceInstance *pdidi) {
	using DIDeviceInstance = DITraits<IDInput>::DIDeviceInstance;

	LOG_PRE_T(IDInput, "lpDirectInputDevice: {}, pdidi: {}\n",
	          static_cast<void *>(lpDirectInputDevice),
	          static_cast<void *>(pdidi));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.GetDeviceInfo(
	    lpDirectInputDevice, pdidi);

	if (SUCCEEDED(ret)) {
		LOG_INFO_T(IDInput, "pdidi dwSize: {}", pdidi->dwSize);

		if (pdidi->dwSize >= offsetof(DIDeviceInstance, guidInstance)
		                         + sizeof(pdidi->guidInstance))
			LOG(", guidInstance: {}", GUIDToString(pdidi->guidInstance));

		if (pdidi->dwSize >= offsetof(DIDeviceInstance, guidProduct)
		                         + sizeof(pdidi->guidProduct))
			LOG(", guidProduct: {}", GUIDToString(pdidi->guidProduct));

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
			LOG(", guidFFDriver: {}", GUIDToString(pdidi->guidFFDriver));

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
	LOG_PRE_T(IDInput, "lpDirectInputDevice: {}, pdwOut: {}\n",
	          static_cast<void *>(lpDirectInputDevice),
	          static_cast<void *>(pdwOut));

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.GetForceFeedbackState(
	    lpDirectInputDevice, pdwOut);

	if (SUCCEEDED(ret) && pdwOut)
		DIGFFSToString(*pdwOut);

	LOG_POST_T(IDInput, "ret: {}\n", ret);

	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8SendForceFeedbackCommand(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice, DWORD dwFlags) {
	LOG_PRE_T(IDInput, "lpDirectInputDevice: {}, dwFlags: {} ({:#x})\n",
	          static_cast<void *>(lpDirectInputDevice), DISFFCToString(dwFlags),
	          dwFlags);

	HRESULT ret = RealDirectInputDevice8Vtbl<IDInput>.SendForceFeedbackCommand(
	    lpDirectInputDevice, dwFlags);

	LOG_POST_T(IDInput, "ret: {}\n", ret);

	return ret;
}

template <typename IDInput>
HRESULT WINAPI DirectInputDevice8Escape(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice,
    LPDIEFFESCAPE pesc) {
	LOG_PRE_T(IDInput, "lpDirectInputDevice: {}, pesc: {}\n",
	          static_cast<void *>(lpDirectInputDevice),
	          static_cast<void *>(pesc));

	if (pesc)
		LOG_INFO_T(IDInput, "sent  pesc: {}\n", DIEFFESCAPEToString(*pesc));

	HRESULT ret =
	    RealDirectInputDevice8Vtbl<IDInput>.Escape(lpDirectInputDevice, pesc);

	if (pesc)
		LOG_INFO_T(IDInput, "recvd pesc: {}\n", DIEFFESCAPEToString(*pesc));

	LOG_POST_T(IDInput, "ret: {}\n", ret);

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

	LOG_PRE_T(IDInput, "Collecting device info for: {}\n", GUIDToString(rguid));

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

	RealDirectInputDevice8Vtbl<IDInput>.EnumObjects(
	    lpDirectInputDevice, EnumObjectsCallback<IDInput>, nullptr, DIDFT_ALL);

	LOG_POST_T(IDInput, "Collection ended for: {}\n", GUIDToString(rguid));
}

template <typename IDInput>
DITraits<IDInput>::DIEffectInfo GetCachedEffectInfo(REFGUID rguid) {
	using DIEffectInfo = DITraits<IDInput>::DIEffectInfo;

	DIEffectInfo retInfo = {};

	if (!EffectInfoCache<IDInput>.contains(rguid))
		return retInfo;

	return EffectInfoCache<IDInput>[rguid];
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
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.EnumObjects,
			             DirectInputDevice8EnumObjects<IDInput>);
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.GetProperty,
			             DirectInputDevice8GetProperty<IDInput>);
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.SetProperty,
			             DirectInputDevice8SetProperty<IDInput>);
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.GetDeviceState,
			             DirectInputDevice8GetDeviceState<IDInput>);
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.GetDeviceData,
			             DirectInputDevice8GetDeviceData<IDInput>);
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.SetDataFormat,
			             DirectInputDevice8SetDataFormat<IDInput>);
			DetourAttach(
			    &RealDirectInputDevice8Vtbl<IDInput>.SetEventNotification,
			    DirectInputDevice8SetEventNotification<IDInput>);
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.EnumEffects,
			             DirectInputDevice8EnumEffects<IDInput>);
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.GetObjectInfo,
			             DirectInputDevice8GetObjectInfo<IDInput>);
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.GetDeviceInfo,
			             DirectInputDevice8GetDeviceInfo<IDInput>);
			DetourAttach(
			    &RealDirectInputDevice8Vtbl<IDInput>.GetForceFeedbackState,
			    DirectInputDevice8GetForceFeedbackState<IDInput>);
			DetourAttach(
			    &RealDirectInputDevice8Vtbl<IDInput>.SendForceFeedbackCommand,
			    DirectInputDevice8SendForceFeedbackCommand<IDInput>);
			DetourAttach(&RealDirectInputDevice8Vtbl<IDInput>.Escape,
			             DirectInputDevice8Escape<IDInput>);
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
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.EnumObjects,
			             DirectInputDevice8EnumObjects<IDInput>);
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.GetProperty,
			             DirectInputDevice8GetProperty<IDInput>);
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.SetProperty,
			             DirectInputDevice8SetProperty<IDInput>);
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.GetDeviceState,
			             DirectInputDevice8GetDeviceState<IDInput>);
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.GetDeviceData,
			             DirectInputDevice8GetDeviceData<IDInput>);
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.SetDataFormat,
			             DirectInputDevice8SetDataFormat<IDInput>);
			DetourDetach(
			    &RealDirectInputDevice8Vtbl<IDInput>.SetEventNotification,
			    DirectInputDevice8SetEventNotification<IDInput>);
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.EnumEffects,
			             DirectInputDevice8EnumEffects<IDInput>);
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.GetObjectInfo,
			             DirectInputDevice8GetObjectInfo<IDInput>);
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.GetDeviceInfo,
			             DirectInputDevice8GetDeviceInfo<IDInput>);
			DetourDetach(
			    &RealDirectInputDevice8Vtbl<IDInput>.GetForceFeedbackState,
			    DirectInputDevice8GetForceFeedbackState<IDInput>);
			DetourDetach(
			    &RealDirectInputDevice8Vtbl<IDInput>.SendForceFeedbackCommand,
			    DirectInputDevice8SendForceFeedbackCommand<IDInput>);
			DetourDetach(&RealDirectInputDevice8Vtbl<IDInput>.Escape,
			             DirectInputDevice8Escape<IDInput>);
		});

		RealDirectInputDevice8Vtbl<IDInput> = {};
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

template DITraits<IDirectInput8A>::DIEffectInfo
GetCachedEffectInfo<IDirectInput8A>(REFGUID rguid);

template DITraits<IDirectInput8W>::DIEffectInfo
GetCachedEffectInfo<IDirectInput8W>(REFGUID rguid);