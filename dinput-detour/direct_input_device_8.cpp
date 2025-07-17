#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_set>

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

constexpr static array<pair<DWORD, string_view>, 7> DIEFTTypeStringPairs = {{
    {DIEFT_ALL, "DIEFT_ALL"},
    {DIEFT_CONSTANTFORCE, "DIEFT_CONSTANTFORCE"},
    {DIEFT_RAMPFORCE, "DIEFT_RAMPFORCE"},
    {DIEFT_PERIODIC, "DIEFT_PERIODIC"},
    {DIEFT_CONDITION, "DIEFT_CONDITION"},
    {DIEFT_CUSTOMFORCE, "DIEFT_CUSTOMFORCE"},
    {DIEFT_HARDWARE, "DIEFT_HARDWARE"},
}};

constexpr static array<pair<DWORD, string_view>, 7> DIEFTMaskStringPairs = {{
    {DIEFT_FFATTACK, "DIEFT_FFATTACK"},
    {DIEFT_FFFADE, "DIEFT_FFFADE"},
    {DIEFT_SATURATION, "DIEFT_SATURATION"},
    {DIEFT_POSNEGCOEFFICIENTS, "DIEFT_POSNEGCOEFFICIENTS"},
    {DIEFT_POSNEGSATURATION, "DIEFT_POSNEGSATURATION"},
    {DIEFT_DEADBAND, "DIEFT_DEADBAND"},
    {DIEFT_STARTDELAY, "DIEFT_STARTDELAY"},
}};

constexpr static array<pair<DWORD, string_view>, 2> DIEPValueStringPairs = {{
    {DIEP_ALLPARAMS_DX5, "DIEP_ALLPARAMS_DX5"},
    {DIEP_ALLPARAMS, "DIEP_ALLPARAMS"},
}};

constexpr static array<pair<DWORD, string_view>, 13> DIEPMaskStringPairs = {{
    {DIEP_DURATION, "DIEP_DURATION"},
    {DIEP_SAMPLEPERIOD, "DIEP_SAMPLEPERIOD"},
    {DIEP_GAIN, "DIEP_GAIN"},
    {DIEP_TRIGGERBUTTON, "DIEP_TRIGGERBUTTON"},
    {DIEP_TRIGGERREPEATINTERVAL, "DIEP_TRIGGERREPEATINTERVAL"},
    {DIEP_AXES, "DIEP_AXES"},
    {DIEP_DIRECTION, "DIEP_DIRECTION"},
    {DIEP_ENVELOPE, "DIEP_ENVELOPE"},
    {DIEP_TYPESPECIFICPARAMS, "DIEP_TYPESPECIFICPARAMS"},
    {DIEP_STARTDELAY, "DIEP_STARTDELAY"},
    {DIEP_START, "DIEP_START"},
    {DIEP_NORESTART, "DIEP_NORESTART"},
    {DIEP_NODOWNLOAD, "DIEP_NODOWNLOAD"},
}};

constexpr static array<pair<DWORD, string_view>, 15> DIDCStringPairs = {{
    {DIDC_ATTACHED, "DIDC_ATTACHED"},
    {DIDC_POLLEDDEVICE, "DIDC_POLLEDDEVICE"},
    {DIDC_EMULATED, "DIDC_EMULATED"},
    {DIDC_POLLEDDATAFORMAT, "DIDC_POLLEDDATAFORMAT"},
    {DIDC_FORCEFEEDBACK, "DIDC_FORCEFEEDBACK"},
    {DIDC_FFATTACK, "DIDC_FFATTACK"},
    {DIDC_FFFADE, "DIDC_FFFADE"},
    {DIDC_SATURATION, "DIDC_SATURATION"},
    {DIDC_POSNEGCOEFFICIENTS, "DIDC_POSNEGCOEFFICIENTS"},
    {DIDC_POSNEGSATURATION, "DIDC_POSNEGSATURATION"},
    {DIDC_DEADBAND, "DIDC_DEADBAND"},
    {DIDC_STARTDELAY, "DIDC_STARTDELAY"},
    {DIDC_ALIAS, "DIDC_ALIAS"},
    {DIDC_PHANTOM, "DIDC_PHANTOM"},
    {DIDC_HIDDEN, "DIDC_HIDDEN"},
}};

const static array<pair<REFGUID, string_view>, 25> DIPROPStringPairs = {{
    {DIPROP_BUFFERSIZE, "DIPROP_BUFFERSIZE"},
    {DIPROP_AXISMODE, "DIPROP_AXISMODE"},
    {DIPROP_GRANULARITY, "DIPROP_GRANULARITY"},
    {DIPROP_RANGE, "DIPROP_RANGE"},
    {DIPROP_DEADZONE, "DIPROP_DEADZONE"},
    {DIPROP_SATURATION, "DIPROP_SATURATION"},
    {DIPROP_FFGAIN, "DIPROP_FFGAIN"},
    {DIPROP_FFLOAD, "DIPROP_FFLOAD"},
    {DIPROP_AUTOCENTER, "DIPROP_AUTOCENTER"},
    {DIPROP_CALIBRATIONMODE, "DIPROP_CALIBRATIONMODE"},
    {DIPROP_CALIBRATION, "DIPROP_CALIBRATION"},
    {DIPROP_GUIDANDPATH, "DIPROP_GUIDANDPATH"},
    {DIPROP_INSTANCENAME, "DIPROP_INSTANCENAME"},
    {DIPROP_PRODUCTNAME, "DIPROP_PRODUCTNAME"},
    {DIPROP_JOYSTICKID, "DIPROP_JOYSTICKID"},
    {DIPROP_GETPORTDISPLAYNAME, "DIPROP_GETPORTDISPLAYNAME"},
    {DIPROP_PHYSICALRANGE, "DIPROP_PHYSICALRANGE"},
    {DIPROP_LOGICALRANGE, "DIPROP_LOGICALRANGE"},
    {DIPROP_KEYNAME, "DIPROP_KEYNAME"},
    {DIPROP_CPOINTS, "DIPROP_CPOINTS"},
    {DIPROP_APPDATA, "DIPROP_APPDATA"},
    {DIPROP_SCANCODE, "DIPROP_SCANCODE"},
    {DIPROP_VIDPID, "DIPROP_VIDPID"},
    {DIPROP_USERNAME, "DIPROP_USERNAME"},
    {DIPROP_TYPENAME, "DIPROP_TYPENAME"},
}};

constexpr static array<pair<DWORD, string_view>, 4> DIPHStringPairs = {{
    {DIPH_DEVICE, "DIPH_DEVICE"},
    {DIPH_BYOFFSET, "DIPH_BYOFFSET"},
    {DIPH_BYID, "DIPH_BYID"},
    {DIPH_BYUSAGE, "DIPH_BYUSAGE"},
}};

constexpr static array<pair<DWORD, string_view>, 12> DIGFFSStringPairs = {{
    {DIGFFS_EMPTY, "DIGFFS_EMPTY"},
    {DIGFFS_STOPPED, "DIGFFS_STOPPED"},
    {DIGFFS_PAUSED, "DIGFFS_PAUSED"},
    {DIGFFS_ACTUATORSON, "DIGFFS_ACTUATORSON"},
    {DIGFFS_ACTUATORSOFF, "DIGFFS_ACTUATORSOFF"},
    {DIGFFS_POWERON, "DIGFFS_POWERON"},
    {DIGFFS_POWEROFF, "DIGFFS_POWEROFF"},
    {DIGFFS_SAFETYSWITCHON, "DIGFFS_SAFETYSWITCHON"},
    {DIGFFS_SAFETYSWITCHOFF, "DIGFFS_SAFETYSWITCHOFF"},
    {DIGFFS_USERFFSWITCHON, "DIGFFS_USERFFSWITCHON"},
    {DIGFFS_USERFFSWITCHOFF, "DIGFFS_USERFFSWITCHOFF"},
    {DIGFFS_DEVICELOST, "DIGFFS_DEVICELOST"},
}};

static bool operator==(const DIDATAFORMAT &lhs, const DIDATAFORMAT &rhs) {
	if (lhs.dwSize != rhs.dwSize || lhs.dwObjSize != rhs.dwObjSize
	    || lhs.dwFlags != rhs.dwFlags || lhs.dwDataSize != rhs.dwDataSize
	    || lhs.dwNumObjs != rhs.dwNumObjs)
		return false;

	return memcmp(lhs.rgodf, rhs.rgodf, lhs.dwNumObjs * lhs.dwObjSize);
}

static string DIEFTToString(DWORD dwEffType) {
	for (auto &&pair : DIEFTTypeStringPairs) {
		if (DIEFT_GETTYPE(dwEffType) == pair.first)
			return string(pair.second);
	}

	string str;
	for (auto &&pair : DIEFTMaskStringPairs) {
		if (dwEffType & pair.first) {
			if (!str.empty())
				str += " | ";
			str += pair.second;
		}
	}

	if (!str.empty())
		return str;

	return format("Unknown DIEFT: {:#x}", dwEffType);
}

static string DIEPToString(DWORD dwStaticParams) {
	for (auto &&pair : DIEPValueStringPairs) {
		if (dwStaticParams == pair.first)
			return string(pair.second);
	}

	string str;
	for (auto &&pair : DIEPMaskStringPairs) {
		if (dwStaticParams & pair.first) {
			if (!str.empty())
				str += " | ";
			str += pair.second;
		}
	}

	if (!str.empty())
		return str;

	return format("Unknown DIEP: {:#x}", dwStaticParams);
}

static string DIDCToString(DWORD dwFlags) {
	string str;
	for (auto &&pair : DIDCStringPairs) {
		if (dwFlags & pair.first) {
			if (!str.empty())
				str += " | ";
			str += pair.second;
		}
	}

	if (!str.empty())
		return str;

	return format("Unknown DIDC: {:#x}", dwFlags);
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

static string DIPROPToString(REFGUID rguidProp) {
	for (auto &&pair : DIPROPStringPairs) {
		if (&rguidProp == &pair.first)
			return string(pair.second);
	}

	return format("Unknown DIPROP: {}", static_cast<const void *>(&rguidProp));
}

static string DIPHHowToString(DWORD dwHow) {
	for (auto &&pair : DIPHStringPairs) {
		if (dwHow == pair.first)
			return string(pair.second);
	}

	return format("Unknown DIPH: {:#x}", dwHow);
}

static string DIDATAFORMATToString(LPCDIDATAFORMAT lpdf) {
	string str;
	if (lpdf && *lpdf == c_dfDIKeyboard)
		str += format("c_dfDIKeyboard");
	else if (lpdf && *lpdf == c_dfDIMouse)
		str += format("c_dfDIMouse");
	else if (lpdf && *lpdf == c_dfDIMouse2)
		str += format("c_dfDIMouse2");
	else if (lpdf && *lpdf == c_dfDIJoystick)
		str += format("c_dfDIJoystick");
	else if (lpdf && *lpdf == c_dfDIJoystick2)
		str += format("c_dfDIJoystick2");
	else if (lpdf)
		str += format(
		    "Custom: dwSize: {}, dwObjsSize: {}, dwFlags: {:#x}, dwDataSize: "
		    "{}, dwNumObjs: {}",
		    lpdf->dwSize, lpdf->dwObjSize, lpdf->dwFlags, lpdf->dwDataSize,
		    lpdf->dwNumObjs);
	else
		str += format("{}", static_cast<const void *>(lpdf));

	return str;
}

static string DIPROPHEADERToString(REFGUID rguidProp,
                                   const DIPROPHEADER &diph) {
	string str =
	    format("dwSize: {}, dwHeaderSize: {}, dwObj: {}, dwHow: {} ({:#x}), ",
	           diph.dwSize, diph.dwHeaderSize, diph.dwObj,
	           DIPHHowToString(diph.dwHow), diph.dwHow);

	if (&rguidProp == &DIPROP_APPDATA && diph.dwSize == sizeof(DIPROPPOINTER)) {
		LPCDIPROPPOINTER pdipp = reinterpret_cast<LPCDIPROPPOINTER>(&diph);
		str += format("uData: {}", pdipp->uData);
	} else if ((&rguidProp == &DIPROP_AUTOCENTER         //
	            || &rguidProp == &DIPROP_AXISMODE        //
	            || &rguidProp == &DIPROP_BUFFERSIZE      //
	            || &rguidProp == &DIPROP_CALIBRATIONMODE //
	            || &rguidProp == &DIPROP_DEADZONE        //
	            || &rguidProp == &DIPROP_FFGAIN          //
	            || &rguidProp == &DIPROP_FFLOAD          //
	            || &rguidProp == &DIPROP_GRANULARITY     //
	            || &rguidProp == &DIPROP_JOYSTICKID      //
	            || &rguidProp == &DIPROP_SATURATION      //
	            || &rguidProp == &DIPROP_SCANCODE        //
	            || &rguidProp == &DIPROP_VIDPID)         //
	           && diph.dwSize == sizeof(DIPROPDWORD)) {
		LPCDIPROPDWORD pdidw = reinterpret_cast<LPCDIPROPDWORD>(&diph);
		str += format("dwData: {} ({:#x})", pdidw->dwData, pdidw->dwData);
	} else if (&rguidProp == &DIPROP_GUIDANDPATH
	           && diph.dwSize == sizeof(DIPROPGUIDANDPATH)) {
		LPCDIPROPGUIDANDPATH pdiguid =
		    reinterpret_cast<LPCDIPROPGUIDANDPATH>(&diph);
		str += format("guidClass: {}, wszPath: {}",
		              guid_to_str(pdiguid->guidClass),
		              wstring_to_string(pdiguid->wszPath));
	} else if ((&rguidProp == &DIPROP_LOGICALRANGE
	            || &rguidProp == &DIPROP_PHYSICALRANGE
	            || &rguidProp == &DIPROP_RANGE)
	           && diph.dwSize == sizeof(DIPROPRANGE)) {
		LPCDIPROPRANGE pdirange = reinterpret_cast<LPCDIPROPRANGE>(&diph);
		str += format("lMin: {}, lMax: {}", pdirange->lMin, pdirange->lMax);
	} else if ((&rguidProp == &DIPROP_GETPORTDISPLAYNAME //
	            || &rguidProp == &DIPROP_INSTANCENAME    //
	            || &rguidProp == &DIPROP_KEYNAME         //
	            || &rguidProp == &DIPROP_PRODUCTNAME     //
	            || &rguidProp == &DIPROP_TYPENAME        //
	            || &rguidProp == &DIPROP_USERNAME)       //
	           && diph.dwSize == sizeof(DIPROPSTRING)) {
		LPCDIPROPSTRING pdistr = reinterpret_cast<LPCDIPROPSTRING>(&diph);
		str += format("wsz: {}", wstring_to_string(pdistr->wsz));
	} else if (&rguidProp == &DIPROP_CALIBRATION
	           || &rguidProp == &DIPROP_CPOINTS) {
		str += format("Unsupported property: {}", DIPROPToString(rguidProp));
	} else {
		str += format("Unknown property: {}", guid_to_str(rguidProp));
	}

	return str;
}

static string DIGFFSToString(DWORD ffstate) {
	string str;
	for (auto &&pair : DIGFFSStringPairs) {
		if (ffstate & pair.first) {
			if (!str.empty())
				str += " | ";
			str += pair.second;
		}
	}

	if (!str.empty())
		return str;

	return format("Unknown DIGFFS: {:#x}", ffstate);
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
