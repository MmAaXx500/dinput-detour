#include <unordered_set>

#include "direct_input_device_8.h"
#include "log.h"
#include "stringconv.h"
#include "utils.h"

using namespace std;

IDirectInputDevice8AVtbl RealDirectInputDevice8VtblA = {};
IDirectInputDevice8WVtbl RealDirectInputDevice8VtblW = {};

namespace std {
template <> struct hash<GUID> {
	size_t operator()(const GUID &guid) const noexcept {
		const uint64_t *p = reinterpret_cast<const uint64_t *>(&guid);
		return std::hash<uint64_t>{}(p[0]) ^ std::hash<uint64_t>{}(p[1]);
	}
};
} // namespace std

static unordered_set<GUID> seenDevicesA;
static unordered_set<GUID> seenDevicesW;

struct EnumEffectsCallbackDataA {
	LPDIENUMEFFECTSCALLBACKA realCallback;
	PVOID realData;
};

struct EnumEffectsCallbackDataW {
	LPDIENUMEFFECTSCALLBACKW realCallback;
	PVOID realData;
};

HRESULT WINAPI RoutedDirectInputDevice8CreateEffectA(
    LPDIRECTINPUTDEVICE8A lpDirectInputDevice, REFGUID rguid, LPCDIEFFECT lpeff,
    LPDIRECTINPUTEFFECT *ppdeff, LPUNKNOWN punkOuter) {
	LOG_PRE("lpDirectInputDevice: {}, rguid: {}, lpeff: {}, ppdeff: {}, "
	        "punkOuter: {}\n",
	        static_cast<void *>(lpDirectInputDevice), guid_to_str(rguid),
	        static_cast<const void *>(lpeff), static_cast<void *>(ppdeff),
	        static_cast<void *>(punkOuter));

	DIEFFECTINFOA effinfo = {};
	effinfo.dwSize = sizeof(DIEFFECTINFOA);
	HRESULT hr = RealDirectInputDevice8VtblA.GetEffectInfo(lpDirectInputDevice,
	                                                       &effinfo, rguid);
	if (FAILED(hr))
		effinfo.dwEffType = 0;

	LOG_INFO("lpeff: {}", DIEFFECTToString(lpeff, effinfo.dwEffType));

	HRESULT ret = RealDirectInputDevice8VtblA.CreateEffect(
	    lpDirectInputDevice, rguid, lpeff, ppdeff, punkOuter);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInputDevice8CreateEffectW(
    LPDIRECTINPUTDEVICE8W lpDirectInputDevice, REFGUID rguid, LPCDIEFFECT lpeff,
    LPDIRECTINPUTEFFECT *ppdeff, LPUNKNOWN punkOuter) {
	LOG_PRE("lpDirectInputDevice: {}, rguid: {}, lpeff: {}, ppdeff: {}, "
	        "punkOuter: {}\n",
	        static_cast<void *>(lpDirectInputDevice), guid_to_str(rguid),
	        static_cast<const void *>(lpeff), static_cast<void *>(ppdeff),
	        static_cast<void *>(punkOuter));

	DIEFFECTINFOW effinfo = {};
	effinfo.dwSize = sizeof(DIEFFECTINFOW);
	HRESULT hr = RealDirectInputDevice8VtblW.GetEffectInfo(lpDirectInputDevice,
	                                                       &effinfo, rguid);
	if (FAILED(hr))
		effinfo.dwEffType = 0;

	LOG_INFO("lpeff: {}", DIEFFECTToString(lpeff, effinfo.dwEffType));

	HRESULT ret = RealDirectInputDevice8VtblW.CreateEffect(
	    lpDirectInputDevice, rguid, lpeff, ppdeff, punkOuter);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInputDevice8GetCapabilitiesA(
    LPDIRECTINPUTDEVICE8A lpDirectInputDevice, LPDIDEVCAPS lpDIDevCaps) {
	LOG_PRE("lpDirectInputDevice: {}, lpDIDevCaps: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<void *>(lpDIDevCaps));

	HRESULT ret = RealDirectInputDevice8VtblA.GetCapabilities(
	    lpDirectInputDevice, lpDIDevCaps);

	if (SUCCEEDED(ret) && lpDIDevCaps)
		LOG_INFO("{}\n", DIDEVCAPSToString(*lpDIDevCaps));

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInputDevice8GetCapabilitiesW(
    LPDIRECTINPUTDEVICE8W lpDirectInputDevice, LPDIDEVCAPS lpDIDevCaps) {
	LOG_PRE("lpDirectInputDevice: {}, lpDIDevCaps: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<void *>(lpDIDevCaps));

	HRESULT ret = RealDirectInputDevice8VtblW.GetCapabilities(
	    lpDirectInputDevice, lpDIDevCaps);

	if (SUCCEEDED(ret) && lpDIDevCaps)
		LOG_INFO("{}\n", DIDEVCAPSToString(*lpDIDevCaps));

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI
RoutedDirectInputDevice8GetPropertyA(LPDIRECTINPUTDEVICE8A lpDirectInputDevice,
                                     REFGUID rguidProp, LPDIPROPHEADER pdiph) {
	LOG_PRE("lpDirectInputDevice: {}, rguidProp: {}, pdiph: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<const void *>(&rguidProp), static_cast<void *>(pdiph));

	LOG_INFO("rguidProp: {}\n", DIPROPToString(rguidProp));

	HRESULT ret = RealDirectInputDevice8VtblA.GetProperty(lpDirectInputDevice,
	                                                      rguidProp, pdiph);

	if (SUCCEEDED(ret))
		LOG_INFO("pdiph: {}\n", DIPROPHEADERToString(rguidProp, *pdiph));

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI
RoutedDirectInputDevice8GetPropertyW(LPDIRECTINPUTDEVICE8W lpDirectInputDevice,
                                     REFGUID rguidProp, LPDIPROPHEADER pdiph) {

	LOG_PRE("lpDirectInputDevice: {}, rguidProp: {}, pdiph: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<const void *>(&rguidProp), static_cast<void *>(pdiph));

	LOG_INFO("rguidProp: {}\n", DIPROPToString(rguidProp));

	HRESULT ret = RealDirectInputDevice8VtblW.GetProperty(lpDirectInputDevice,
	                                                      rguidProp, pdiph);

	if (SUCCEEDED(ret))
		LOG_INFO("pdiph: {}\n", DIPROPHEADERToString(rguidProp, *pdiph));

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI
RoutedDirectInputDevice8SetPropertyA(LPDIRECTINPUTDEVICE8A lpDirectInputDevice,
                                     REFGUID rguidProp, LPCDIPROPHEADER pdiph) {
	LOG_PRE("lpDirectInputDevice: {}, rguidProp: {}, pdiph: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<const void *>(&rguidProp),
	        static_cast<const void *>(pdiph));

	LOG_INFO("rguidProp: {}\n", DIPROPToString(rguidProp));

	LOG_INFO("pdiph: {}\n", DIPROPHEADERToString(rguidProp, *pdiph));

	HRESULT ret = RealDirectInputDevice8VtblA.SetProperty(lpDirectInputDevice,
	                                                      rguidProp, pdiph);
	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI
RoutedDirectInputDevice8SetPropertyW(LPDIRECTINPUTDEVICE8W lpDirectInputDevice,
                                     REFGUID rguidProp, LPCDIPROPHEADER pdiph) {
	LOG_PRE("lpDirectInputDevice: {}, rguidProp: {}, pdiph: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<const void *>(&rguidProp),
	        static_cast<const void *>(pdiph));

	LOG_INFO("rguidProp: {}\n", DIPROPToString(rguidProp));

	LOG_INFO("pdiph: {}\n", DIPROPHEADERToString(rguidProp, *pdiph));

	HRESULT ret = RealDirectInputDevice8VtblW.SetProperty(lpDirectInputDevice,
	                                                      rguidProp, pdiph);
	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInputDevice8SetDataFormatA(
    LPDIRECTINPUTDEVICE8A lpDirectInputDevice, LPCDIDATAFORMAT lpdf) {
	LOG_PRE("lpDirectInputDevice: {}, lpdf: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<const void *>(lpdf));

	LOG_INFO("lpdf: {}\n", DIDATAFORMATToString(lpdf));

	HRESULT ret =
	    RealDirectInputDevice8VtblA.SetDataFormat(lpDirectInputDevice, lpdf);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInputDevice8SetDataFormatW(
    LPDIRECTINPUTDEVICE8W lpDirectInputDevice, LPCDIDATAFORMAT lpdf) {
	LOG_PRE("lpDirectInputDevice: {}, lpdf: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<const void *>(lpdf));

	LOG_INFO("lpdf: {}\n", DIDATAFORMATToString(lpdf));

	HRESULT ret =
	    RealDirectInputDevice8VtblW.SetDataFormat(lpDirectInputDevice, lpdf);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInputDevice8SetEventNotificationA(
    LPDIRECTINPUTDEVICE8A lpDirectInputDevice, HANDLE hEvent) {
	LOG_PRE("lpDirectInputDevice: {}, hEvent: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<void *>(hEvent));

	HRESULT ret = RealDirectInputDevice8VtblA.SetEventNotification(
	    lpDirectInputDevice, hEvent);

	LOG_POST("ret: {}\n", ret);

	return ret;
}

HRESULT WINAPI RoutedDirectInputDevice8SetEventNotificationW(
    LPDIRECTINPUTDEVICE8W lpDirectInputDevice, HANDLE hEvent) {
	LOG_PRE("lpDirectInputDevice: {}, hEvent: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<void *>(hEvent));

	HRESULT ret = RealDirectInputDevice8VtblW.SetEventNotification(
	    lpDirectInputDevice, hEvent);

	LOG_POST("ret: {}\n", ret);

	return ret;
}

BOOL WINAPI EnumEffectsCallbackA(LPCDIEFFECTINFOA pdei, LPVOID pvRef) {
	LOG_PRE("pdei: {}, pvRef: {}\n", reinterpret_cast<const void *>(pdei),
	        pvRef);

	EnumEffectsCallbackDataA *data =
	    reinterpret_cast<EnumEffectsCallbackDataA *>(pvRef);

	LOG_INFO("pdei dwSize: {}", pdei->dwSize);
	if (pdei->dwSize >= offsetof(DIEFFECTINFOA, guid) + sizeof(pdei->guid))
		LOG(", guid: {}", guid_to_str(pdei->guid));
	if (pdei->dwSize
	    >= offsetof(DIEFFECTINFOA, dwEffType) + sizeof(pdei->dwEffType))
		LOG(", dwEffType: {}({:#x})", DIEFTToString(pdei->dwEffType),
		    pdei->dwEffType);
	if (pdei->dwSize >= offsetof(DIEFFECTINFOA, dwStaticParams)
	                        + sizeof(pdei->dwStaticParams))
		LOG(", dwStaticParams: {}({:#x})", DIEPToString(pdei->dwStaticParams),
		    pdei->dwStaticParams);
	if (pdei->dwSize >= offsetof(DIEFFECTINFOA, dwDynamicParams)
	                        + sizeof(pdei->dwDynamicParams))
		LOG(", dwDynamicParams: {}({:#x})", DIEPToString(pdei->dwDynamicParams),
		    pdei->dwDynamicParams);
	if (pdei->dwSize
	    >= offsetof(DIEFFECTINFOA, tszName) + sizeof(pdei->tszName))
		LOG(", tszName: {}", pdei->tszName);

	LOG("\n");

	if (data == nullptr || data->realCallback == nullptr) {
		return DIENUM_CONTINUE;
	}

	BOOL ret = data->realCallback(pdei, data->realData);

	LOG_POST("ret: {}\n", ret);

	return ret;
}

BOOL WINAPI EnumEffectsCallbackW(LPCDIEFFECTINFOW pdei, LPVOID pvRef) {
	LOG_PRE("pdei: {}, pvRef: {}\n", reinterpret_cast<const void *>(pdei),
	        pvRef);

	EnumEffectsCallbackDataW *data =
	    reinterpret_cast<EnumEffectsCallbackDataW *>(pvRef);

	LOG_INFO("pdei dwSize: {}", pdei->dwSize);
	if (pdei->dwSize >= offsetof(DIEFFECTINFOW, guid) + sizeof(pdei->guid))
		LOG(", guid: {}", guid_to_str(pdei->guid));
	if (pdei->dwSize
	    >= offsetof(DIEFFECTINFOW, dwEffType) + sizeof(pdei->dwEffType))
		LOG(", dwEffType: {}({:#x})", DIEFTToString(pdei->dwEffType),
		    pdei->dwEffType);
	if (pdei->dwSize >= offsetof(DIEFFECTINFOW, dwStaticParams)
	                        + sizeof(pdei->dwStaticParams))
		LOG(", dwStaticParams: {}({:#x})", DIEPToString(pdei->dwStaticParams),
		    pdei->dwStaticParams);
	if (pdei->dwSize >= offsetof(DIEFFECTINFOW, dwDynamicParams)
	                        + sizeof(pdei->dwDynamicParams))
		LOG(", dwDynamicParams: {}({:#x})", DIEPToString(pdei->dwDynamicParams),
		    pdei->dwDynamicParams);
	if (pdei->dwSize
	    >= offsetof(DIEFFECTINFOW, tszName) + sizeof(pdei->tszName))
		LOG(", tszName: {}", wstring_to_string(pdei->tszName));

	LOG("\n");

	if (data == nullptr || data->realCallback == nullptr) {
		return DIENUM_CONTINUE;
	}

	BOOL ret = data->realCallback(pdei, data->realData);

	LOG_POST("ret: {}\n", ret);

	return ret;
}

HRESULT WINAPI RoutedDirectInputDevice8EnumEffectsA(
    LPDIRECTINPUTDEVICE8A lpDirectInputDevice,
    LPDIENUMEFFECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwEffType) {
	LOG_PRE("lpDirectInputDevice: {}, lpCallback: {}, pvRef: {}, "
	        "dwEffType: {:x}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        reinterpret_cast<void *>(lpCallback), pvRef, dwEffType);

	LOG_INFO("dwEffType: {}\n", DIEFTToString(dwEffType));

	EnumEffectsCallbackDataA data{
	    .realCallback = lpCallback,
	    .realData = pvRef,
	};

	HRESULT ret = RealDirectInputDevice8VtblA.EnumEffects(
	    lpDirectInputDevice, EnumEffectsCallbackA, &data, dwEffType);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInputDevice8EnumEffectsW(
    LPDIRECTINPUTDEVICE8W lpDirectInputDevice,
    LPDIENUMEFFECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwEffType) {
	LOG_PRE("lpDirectInputDevice: {}, lpCallback: {}, pvRef: {}, "
	        "dwEffType: {:x}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        reinterpret_cast<void *>(lpCallback), pvRef, dwEffType);

	LOG_INFO("dwEffType: {}\n", DIEFTToString(dwEffType));

	EnumEffectsCallbackDataW data{
	    .realCallback = lpCallback,
	    .realData = pvRef,
	};

	HRESULT ret = RealDirectInputDevice8VtblW.EnumEffects(
	    lpDirectInputDevice, EnumEffectsCallbackW, &data, dwEffType);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInputDevice8GetDeviceInfoA(
    LPDIRECTINPUTDEVICE8A lpDirectInputDevice, LPDIDEVICEINSTANCEA pdidi) {
	LOG_PRE("lpDirectInputDevice: {}, pdidi: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<void *>(pdidi));

	HRESULT ret =
	    RealDirectInputDevice8VtblA.GetDeviceInfo(lpDirectInputDevice, pdidi);

	if (SUCCEEDED(ret)) {
		LOG_INFO("pdidi dwSize: {}", pdidi->dwSize);

		if (pdidi->dwSize >= offsetof(DIDEVICEINSTANCEA, guidInstance)
		                         + sizeof(pdidi->guidInstance))
			LOG(", guidInstance: {}", guid_to_str(pdidi->guidInstance));

		if (pdidi->dwSize >= offsetof(DIDEVICEINSTANCEA, guidProduct)
		                         + sizeof(pdidi->guidProduct))
			LOG(", guidProduct: {}", guid_to_str(pdidi->guidProduct));

		if (pdidi->dwSize >= offsetof(DIDEVICEINSTANCEA, dwDevType)
		                         + sizeof(pdidi->dwDevType))
			LOG(", dwDevType: {:#x}", pdidi->dwDevType);

		if (pdidi->dwSize >= offsetof(DIDEVICEINSTANCEA, tszInstanceName)
		                         + sizeof(pdidi->tszInstanceName))
			LOG(", tszInstanceName: {}", pdidi->tszInstanceName);

		if (pdidi->dwSize >= offsetof(DIDEVICEINSTANCEA, tszProductName)
		                         + sizeof(pdidi->tszProductName))
			LOG(", tszProductName: {}", pdidi->tszProductName);

		if (pdidi->dwSize >= offsetof(DIDEVICEINSTANCEA, guidFFDriver)
		                         + sizeof(pdidi->guidFFDriver))
			LOG(", guidFFDriver: {}", guid_to_str(pdidi->guidFFDriver));

		if (pdidi->dwSize >= offsetof(DIDEVICEINSTANCEA, wUsagePage)
		                         + sizeof(pdidi->wUsagePage))
			LOG(", wUsagePage: {:#x}", pdidi->wUsagePage);

		if (pdidi->dwSize
		    >= offsetof(DIDEVICEINSTANCEA, wUsage) + sizeof(pdidi->wUsage))
			LOG(", wUsage: {:#x}", pdidi->wUsage);

		LOG("\n");
	}

	return ret;
}

HRESULT WINAPI RoutedDirectInputDevice8GetDeviceInfoW(
    LPDIRECTINPUTDEVICE8W lpDirectInputDevice, LPDIDEVICEINSTANCEW pdidi) {
	LOG_PRE("lpDirectInputDevice: {}, pdidi: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<void *>(pdidi));

	HRESULT ret =
	    RealDirectInputDevice8VtblW.GetDeviceInfo(lpDirectInputDevice, pdidi);

	if (SUCCEEDED(ret)) {
		LOG_INFO("pdidi dwSize: {}", pdidi->dwSize);

		if (pdidi->dwSize >= offsetof(DIDEVICEINSTANCEW, guidInstance)
		                         + sizeof(pdidi->guidInstance))
			LOG(", guidInstance: {}", guid_to_str(pdidi->guidInstance));

		if (pdidi->dwSize >= offsetof(DIDEVICEINSTANCEW, guidProduct)
		                         + sizeof(pdidi->guidProduct))
			LOG(", guidProduct: {}", guid_to_str(pdidi->guidProduct));

		if (pdidi->dwSize >= offsetof(DIDEVICEINSTANCEW, dwDevType)
		                         + sizeof(pdidi->dwDevType))
			LOG(", dwDevType: {:#x}", pdidi->dwDevType);

		if (pdidi->dwSize >= offsetof(DIDEVICEINSTANCEW, tszInstanceName)
		                         + sizeof(pdidi->tszInstanceName))
			LOG(", tszInstanceName: {}",
			    wstring_to_string(pdidi->tszInstanceName));

		if (pdidi->dwSize >= offsetof(DIDEVICEINSTANCEW, tszProductName)
		                         + sizeof(pdidi->tszProductName))
			LOG(", tszProductName: {}",
			    wstring_to_string(pdidi->tszProductName));

		if (pdidi->dwSize >= offsetof(DIDEVICEINSTANCEW, guidFFDriver)
		                         + sizeof(pdidi->guidFFDriver))
			LOG(", guidFFDriver: {}", guid_to_str(pdidi->guidFFDriver));

		if (pdidi->dwSize >= offsetof(DIDEVICEINSTANCEW, wUsagePage)
		                         + sizeof(pdidi->wUsagePage))
			LOG(", wUsagePage: {:#x}", pdidi->wUsagePage);

		if (pdidi->dwSize
		    >= offsetof(DIDEVICEINSTANCEW, wUsage) + sizeof(pdidi->wUsage))
			LOG(", wUsage: {:#x}", pdidi->wUsage);

		LOG("\n");
	}

	return ret;
}

HRESULT WINAPI RoutedDirectInputDevice8GetForceFeedbackStateA(
    LPDIRECTINPUTDEVICE8A lpDirectInputDevice, LPDWORD pdwOut) {
	LOG_PRE("lpDirectInputDevice: {}, pdwOut: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<void *>(pdwOut));

	HRESULT ret = RealDirectInputDevice8VtblA.GetForceFeedbackState(
	    lpDirectInputDevice, pdwOut);

	if (SUCCEEDED(ret) && pdwOut)
		DIGFFSToString(*pdwOut);

	LOG_POST("ret: {}\n", ret);

	return ret;
}

HRESULT WINAPI RoutedDirectInputDevice8GetForceFeedbackStateW(
    LPDIRECTINPUTDEVICE8W lpDirectInputDevice, LPDWORD pdwOut) {
	LOG_PRE("lpDirectInputDevice: {}, pdwOut: {}\n",
	        static_cast<void *>(lpDirectInputDevice),
	        static_cast<void *>(pdwOut));

	HRESULT ret = RealDirectInputDevice8VtblW.GetForceFeedbackState(
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
static void CollectDeviceInfoA(const LPDIRECTINPUTDEVICE8A lpDirectInputDevice,
                               const GUID rguid) {
	if (seenDevicesA.contains(rguid))
		return;

	LOG_PRE("Collecting device info for: {}\n", guid_to_str(rguid));

	seenDevicesA.insert(rguid);

	DIDEVICEINSTANCEA deviceInfo = {};
	deviceInfo.dwSize = sizeof(DIDEVICEINSTANCEA);
	lpDirectInputDevice->lpVtbl->GetDeviceInfo(lpDirectInputDevice,
	                                           &deviceInfo);

	DIDEVCAPS devCaps = {};
	devCaps.dwSize = sizeof(DIDEVCAPS);
	lpDirectInputDevice->lpVtbl->GetCapabilities(lpDirectInputDevice, &devCaps);

	RealDirectInputDevice8VtblA.EnumEffects(
	    lpDirectInputDevice, EnumEffectsCallbackA, nullptr, DIEFT_ALL);

	LOG_POST("Collection ended for: {}\n", guid_to_str(rguid));
}

/*
 * Collect device information for a DirectInput device.
 * This must be called after IDirectInputDevice8 is hooked
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 */
static void CollectDeviceInfoW(const LPDIRECTINPUTDEVICE8W lpDirectInputDevice,
                               const GUID rguid) {
	if (seenDevicesW.contains(rguid))
		return;

	LOG_PRE("Collecting device info for: {}\n", guid_to_str(rguid));

	seenDevicesW.insert(rguid);

	DIDEVICEINSTANCEW deviceInfo = {};
	deviceInfo.dwSize = sizeof(DIDEVICEINSTANCEW);
	lpDirectInputDevice->lpVtbl->GetDeviceInfo(lpDirectInputDevice,
	                                           &deviceInfo);

	DIDEVCAPS devCaps = {};
	devCaps.dwSize = sizeof(DIDEVCAPS);
	lpDirectInputDevice->lpVtbl->GetCapabilities(lpDirectInputDevice, &devCaps);

	RealDirectInputDevice8VtblW.EnumEffects(
	    lpDirectInputDevice, EnumEffectsCallbackW, nullptr, DIEFT_ALL);

	LOG_POST("Collection ended for: {}\n", guid_to_str(rguid));
}

LONG DirectInputDevice8DetourAttachA(LPDIRECTINPUTDEVICE8A lpDirectInputDevice,
                                     REFGUID rguid) {
	LONG ret = NO_ERROR;

	if (RealDirectInputDevice8VtblA.AddRef == nullptr && lpDirectInputDevice) {
		RealDirectInputDevice8VtblA = *lpDirectInputDevice->lpVtbl;

		ret = DetourTransaction([]() {
			DetourAttach(&RealDirectInputDevice8VtblA.CreateEffect,
			             RoutedDirectInputDevice8CreateEffectA);
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
			DetourAttach(&RealDirectInputDevice8VtblA.GetForceFeedbackState,
			             RoutedDirectInputDevice8GetForceFeedbackStateA);
		});
	}

	if (ret == NO_ERROR)
		CollectDeviceInfoA(lpDirectInputDevice, rguid);

	return ret;
}

LONG DirectInputDevice8DetourAttachW(LPDIRECTINPUTDEVICE8W lpDirectInputDevice,
                                     REFGUID rguid) {
	LONG ret = NO_ERROR;

	if (RealDirectInputDevice8VtblW.AddRef == nullptr && lpDirectInputDevice) {
		RealDirectInputDevice8VtblW = *lpDirectInputDevice->lpVtbl;

		ret = DetourTransaction([]() {
			DetourAttach(&RealDirectInputDevice8VtblW.CreateEffect,
			             RoutedDirectInputDevice8CreateEffectW);
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
			DetourAttach(&RealDirectInputDevice8VtblW.GetForceFeedbackState,
			             RoutedDirectInputDevice8GetForceFeedbackStateW);
		});
	}

	if (ret == NO_ERROR)
		CollectDeviceInfoW(lpDirectInputDevice, rguid);

	return ret;
}

LONG DirectInputDevice8DetourDetachA(
    LPDIRECTINPUTDEVICE8A lpDirectInputDevice) {
	LONG ret = ERROR_INVALID_OPERATION;

	if (RealDirectInputDevice8VtblA.AddRef != nullptr && lpDirectInputDevice) {
		ret = DetourTransaction([]() {
			DetourDetach(&RealDirectInputDevice8VtblA.CreateEffect,
			             RoutedDirectInputDevice8CreateEffectA);
			DetourDetach(&RealDirectInputDevice8VtblA.GetCapabilities,
			             RoutedDirectInputDevice8GetCapabilitiesA);
			DetourDetach(&RealDirectInputDevice8VtblA.GetProperty,
			             RoutedDirectInputDevice8GetPropertyA);
			DetourDetach(&RealDirectInputDevice8VtblA.SetProperty,
			             RoutedDirectInputDevice8SetPropertyA);
			DetourDetach(&RealDirectInputDevice8VtblA.SetDataFormat,
			             RoutedDirectInputDevice8SetDataFormatA);
			DetourDetach(&RealDirectInputDevice8VtblA.SetEventNotification,
			             RoutedDirectInputDevice8SetEventNotificationA);
			DetourDetach(&RealDirectInputDevice8VtblA.EnumEffects,
			             RoutedDirectInputDevice8EnumEffectsA);
			DetourDetach(&RealDirectInputDevice8VtblA.GetDeviceInfo,
			             RoutedDirectInputDevice8GetDeviceInfoA);
			DetourDetach(&RealDirectInputDevice8VtblA.GetForceFeedbackState,
			             RoutedDirectInputDevice8GetForceFeedbackStateA);
		});

		memset(&RealDirectInputDevice8VtblA, 0,
		       sizeof(RealDirectInputDevice8VtblA));
	}

	return ret;
}

LONG DirectInputDevice8DetourDetachW(
    LPDIRECTINPUTDEVICE8W lpDirectInputDevice) {
	LONG ret = ERROR_INVALID_OPERATION;

	if (RealDirectInputDevice8VtblW.AddRef != nullptr && lpDirectInputDevice) {
		ret = DetourTransaction([]() {
			DetourDetach(&RealDirectInputDevice8VtblW.CreateEffect,
			             RoutedDirectInputDevice8CreateEffectW);
			DetourDetach(&RealDirectInputDevice8VtblW.GetCapabilities,
			             RoutedDirectInputDevice8GetCapabilitiesW);
			DetourDetach(&RealDirectInputDevice8VtblW.GetProperty,
			             RoutedDirectInputDevice8GetPropertyW);
			DetourDetach(&RealDirectInputDevice8VtblW.SetProperty,
			             RoutedDirectInputDevice8SetPropertyW);
			DetourDetach(&RealDirectInputDevice8VtblW.SetDataFormat,
			             RoutedDirectInputDevice8SetDataFormatW);
			DetourDetach(&RealDirectInputDevice8VtblW.SetEventNotification,
			             RoutedDirectInputDevice8SetEventNotificationW);
			DetourDetach(&RealDirectInputDevice8VtblW.EnumEffects,
			             RoutedDirectInputDevice8EnumEffectsW);
			DetourDetach(&RealDirectInputDevice8VtblW.GetDeviceInfo,
			             RoutedDirectInputDevice8GetDeviceInfoW);
			DetourDetach(&RealDirectInputDevice8VtblW.GetForceFeedbackState,
			             RoutedDirectInputDevice8GetForceFeedbackStateW);
		});

		memset(&RealDirectInputDevice8VtblW, 0,
		       sizeof(RealDirectInputDevice8VtblW));
	}

	return ret;
}
