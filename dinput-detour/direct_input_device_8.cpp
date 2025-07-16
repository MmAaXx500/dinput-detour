#include <cstddef>
#include <string>
#include <unordered_set>

#include "direct_input_8.h"
#include "direct_input_device_8.h"
#include "log.h"
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

static string DIEFTToString(DWORD dwEffType) {
	string ret;

	switch (DIEFT_GETTYPE(dwEffType)) {
	case DIEFT_ALL:
		ret = "DIEFT_ALL ";
		break;
	case DIEFT_CONSTANTFORCE:
		ret = "DIEFT_CONSTANTFORCE ";
		break;
	case DIEFT_RAMPFORCE:
		ret = "DIEFT_RAMPFORCE ";
		break;
	case DIEFT_PERIODIC:
		ret = "DIEFT_PERIODIC ";
		break;
	case DIEFT_CONDITION:
		ret = "DIEFT_CONDITION ";
		break;
	case DIEFT_CUSTOMFORCE:
		ret = "DIEFT_CUSTOMFORCE ";
		break;
	case DIEFT_HARDWARE:
		ret = "DIEFT_HARDWARE ";
		break;
	}

	if (dwEffType & DIEFT_FFATTACK)
		ret += "DIEFT_FFATTACK ";
	if (dwEffType & DIEFT_FFFADE)
		ret += "DIEFT_FFFADE ";
	if (dwEffType & DIEFT_SATURATION)
		ret += "DIEFT_SATURATION ";
	if (dwEffType & DIEFT_POSNEGCOEFFICIENTS)
		ret += "DIEFT_POSNEGCOEFFICIENTS ";
	if (dwEffType & DIEFT_POSNEGSATURATION)
		ret += "DIEFT_POSNEGSATURATION ";
	if (dwEffType & DIEFT_DEADBAND)
		ret += "DIEFT_DEADBAND ";
	if (dwEffType & DIEFT_STARTDELAY)
		ret += "DIEFT_STARTDELAY ";

	if (ret.length() == 0)
		return format("Unknown DIEFT: {:#x}", dwEffType);

	return ret;
}

static string DIEPToString(DWORD dwStaticParams) {
	string ret;

	if (dwStaticParams == DIEP_ALLPARAMS_DX5)
		return "DIEP_ALLPARAMS_DX5";
	if (dwStaticParams == DIEP_ALLPARAMS)
		return "DIEP_ALLPARAMS";

	if (dwStaticParams & DIEP_DURATION)
		ret += "DIEP_DURATION ";
	if (dwStaticParams & DIEP_SAMPLEPERIOD)
		ret += "DIEP_SAMPLEPERIOD ";
	if (dwStaticParams & DIEP_GAIN)
		ret += "DIEP_GAIN ";
	if (dwStaticParams & DIEP_TRIGGERBUTTON)
		ret += "DIEP_TRIGGERBUTTON ";
	if (dwStaticParams & DIEP_TRIGGERREPEATINTERVAL)
		ret += "DIEP_TRIGGERREPEATINTERVAL ";
	if (dwStaticParams & DIEP_AXES)
		ret += "DIEP_AXES ";
	if (dwStaticParams & DIEP_DIRECTION)
		ret += "DIEP_DIRECTION ";
	if (dwStaticParams & DIEP_ENVELOPE)
		ret += "DIEP_ENVELOPE ";
	if (dwStaticParams & DIEP_TYPESPECIFICPARAMS)
		ret += "DIEP_TYPESPECIFICPARAMS ";
	if (dwStaticParams & DIEP_STARTDELAY)
		ret += "DIEP_STARTDELAY ";
	if (dwStaticParams & DIEP_START)
		ret += "DIEP_START ";
	if (dwStaticParams & DIEP_NORESTART)
		ret += "DIEP_NORESTART ";
	if (dwStaticParams & DIEP_NODOWNLOAD)
		ret += "DIEP_NODOWNLOAD ";
	return ret;
}

static string DIDCToString(DWORD dwFlags) {
	string ret;

	if (dwFlags & DIDC_ATTACHED)
		ret += "DIDC_ATTACHED ";
	if (dwFlags & DIDC_POLLEDDEVICE)
		ret += "DIDC_POLLEDDEVICE ";
	if (dwFlags & DIDC_EMULATED)
		ret += "DIDC_EMULATED ";
	if (dwFlags & DIDC_POLLEDDATAFORMAT)
		ret += "DIDC_POLLEDDATAFORMAT ";
	if (dwFlags & DIDC_FORCEFEEDBACK)
		ret += "DIDC_FORCEFEEDBACK ";
	if (dwFlags & DIDC_FFATTACK)
		ret += "DIDC_FFATTACK ";
	if (dwFlags & DIDC_FFFADE)
		ret += "DIDC_FFFADE ";
	if (dwFlags & DIDC_SATURATION)
		ret += "DIDC_SATURATION ";
	if (dwFlags & DIDC_POSNEGCOEFFICIENTS)
		ret += "DIDC_POSNEGCOEFFICIENTS ";
	if (dwFlags & DIDC_POSNEGSATURATION)
		ret += "DIDC_POSNEGSATURATION ";
	if (dwFlags & DIDC_DEADBAND)
		ret += "DIDC_DEADBAND ";
	if (dwFlags & DIDC_STARTDELAY)
		ret += "DIDC_STARTDELAY ";
	if (dwFlags & DIDC_ALIAS)
		ret += "DIDC_ALIAS ";
	if (dwFlags & DIDC_PHANTOM)
		ret += "DIDC_PHANTOM ";
	if (dwFlags & DIDC_HIDDEN)
		ret += "DIDC_HIDDEN ";

	if (ret.empty())
		return format("Unknown DIDC: {:#x}", dwFlags);

	return ret;
}

static string DIDEVCAPSToString(const DIDEVCAPS &lpDIDevCaps) {
	string str = format("lpDIDevCaps dwSize: {}", lpDIDevCaps.dwSize);

	if (lpDIDevCaps.dwSize
	    >= offsetof(DIDEVCAPS, dwFlags) + sizeof(lpDIDevCaps.dwFlags))
		str += format(", dwFlags: {}({:#x})", DIDCToString(lpDIDevCaps.dwFlags),
		              lpDIDevCaps.dwFlags);
	if (lpDIDevCaps.dwSize
	    >= offsetof(DIDEVCAPS, dwDevType) + sizeof(lpDIDevCaps.dwDevType))
		str += format(", dwDevType: {:#x}", lpDIDevCaps.dwDevType);
	if (lpDIDevCaps.dwSize
	    >= offsetof(DIDEVCAPS, dwAxes) + sizeof(lpDIDevCaps.dwAxes))
		str += format(", dwAxes: {}", lpDIDevCaps.dwAxes);
	if (lpDIDevCaps.dwSize
	    >= offsetof(DIDEVCAPS, dwButtons) + sizeof(lpDIDevCaps.dwButtons))
		str += format(", dwButtons: {}", lpDIDevCaps.dwButtons);
	if (lpDIDevCaps.dwSize
	    >= offsetof(DIDEVCAPS, dwPOVs) + sizeof(lpDIDevCaps.dwPOVs))
		str += format(", dwPOVs: {}", lpDIDevCaps.dwPOVs);
	if (lpDIDevCaps.dwSize >= offsetof(DIDEVCAPS, dwFFSamplePeriod)
	                              + sizeof(lpDIDevCaps.dwFFSamplePeriod))
		str += format(", dwFFSamplePeriod: {}", lpDIDevCaps.dwFFSamplePeriod);
	if (lpDIDevCaps.dwSize >= offsetof(DIDEVCAPS, dwFFMinTimeResolution)
	                              + sizeof(lpDIDevCaps.dwFFMinTimeResolution))
		str += format(", dwFFMinTimeResolution: {}",
		              lpDIDevCaps.dwFFMinTimeResolution);
	if (lpDIDevCaps.dwSize >= offsetof(DIDEVCAPS, dwFirmwareRevision)
	                              + sizeof(lpDIDevCaps.dwFirmwareRevision))
		str +=
		    format(", dwFirmwareRevision: {}", lpDIDevCaps.dwFirmwareRevision);
	if (lpDIDevCaps.dwSize >= offsetof(DIDEVCAPS, dwHardwareRevision)
	                              + sizeof(lpDIDevCaps.dwHardwareRevision))
		str +=
		    format(", dwHardwareRevision: {}", lpDIDevCaps.dwHardwareRevision);
	if (lpDIDevCaps.dwSize >= offsetof(DIDEVCAPS, dwFFDriverVersion)
	                              + sizeof(lpDIDevCaps.dwFFDriverVersion))
		str += format(", dwFFDriverVersion: {}", lpDIDevCaps.dwFFDriverVersion);

	return str;
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

	LOG_POST("ret: {}", ret);

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

	LOG_POST("ret: {}", ret);

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

void CollectDeviceInfoA(const LPDIRECTINPUTDEVICE8A lpDirectInputDevice,
                        const GUID rguid) {
	if (seenDevicesA.contains(rguid))
		return;

	LOG_PRE("Collecting device info for: {}", guid_to_str(rguid));

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

	LOG_POST("Collection ended for: {}", guid_to_str(rguid));
}

void CollectDeviceInfoW(const LPDIRECTINPUTDEVICE8W lpDirectInputDevice,
                        const GUID rguid) {
	if (seenDevicesW.contains(rguid))
		return;

	LOG_PRE("Collecting device info for: {}", guid_to_str(rguid));

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

	LOG_POST("Collection ended for: {}", guid_to_str(rguid));
}
