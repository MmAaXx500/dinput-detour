#include <array>
#include <format>

#include "log.h"
#include "stringconv.h"
#include "utils.h"

using namespace std;

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

constexpr static array<pair<DWORD, string_view>, 2> DIEFFTriggerStringPairs = {{
    {DIEFF_OBJECTIDS, "DIEFF_OBJECTIDS"},
    {DIEFF_OBJECTOFFSETS, "DIEFF_OBJECTOFFSETS"},
}};

constexpr static array<pair<DWORD, string_view>, 3> DIEFFDirectionStringPairs =
    {{
        {DIEFF_CARTESIAN, "DIEFF_CARTESIAN"},
        {DIEFF_POLAR, "DIEFF_POLAR"},
        {DIEFF_SPHERICAL, "DIEFF_SPHERICAL"},
    }};

constexpr static array<pair<DWORD, string_view>, 2> DIESStringPairs = {{
    {DIES_SOLO, "DIES_SOLO"},
    {DIES_NODOWNLOAD, "DIES_NODOWNLOAD"},
}};

constexpr static array<pair<DWORD, string_view>, 2> DIEGESStringPairs = {{
    {DIEGES_PLAYING, "DIEGES_PLAYING"},
    {DIEGES_EMULATED, "DIEGES_EMULATED"},
}};

constexpr static array<pair<DWORD, string_view>, 1> DIGDDStringPairs = {{
    {DIGDD_PEEK, "DIGDD_PEEK"},
}};

constexpr static array<pair<DWORD, string_view>, 13> DIJOFSStringPairs = {{
    {DIJOFS_X, "DIJOFS_X"},
    {DIJOFS_Y, "DIJOFS_Y"},
    {DIJOFS_Z, "DIJOFS_Z"},
    {DIJOFS_RX, "DIJOFS_RX"},
    {DIJOFS_RY, "DIJOFS_RY"},
    {DIJOFS_RZ, "DIJOFS_RZ"},
    {DIJOFS_SLIDER(0), "DIJOFS_SLIDER0"},
    {DIJOFS_SLIDER(1), "DIJOFS_SLIDER1"},
    {DIJOFS_POV(0), "DIJOFS_POV0"},
    {DIJOFS_POV(1), "DIJOFS_POV1"},
    {DIJOFS_POV(2), "DIJOFS_POV2"},
    {DIJOFS_POV(3), "DIJOFS_POV3"},
    // Buttons handled separately
}};

constexpr static array<pair<DWORD, string_view>, 10> DIDFTTypeStringPairs = {{
    {DIDFT_ALL, "DIDFT_ALL"},
    {DIDFT_AXIS, "DIDFT_AXIS"}, // REL + ABS
    {DIDFT_RELAXIS, "DIDFT_RELAXIS"},
    {DIDFT_ABSAXIS, "DIDFT_ABSAXIS"},
    {DIDFT_BUTTON, "DIDFT_BUTTON"}, // PSH + TGL
    {DIDFT_PSHBUTTON, "DIDFT_PSHBUTTON"},
    {DIDFT_TGLBUTTON, "DIDFT_TGLBUTTON"},
    {DIDFT_POV, "DIDFT_POV"},
    {DIDFT_COLLECTION, "DIDFT_COLLECTION"},
    {DIDFT_NODATA, "DIDFT_NODATA"},
}};

constexpr static array<pair<DWORD, string_view>, 6> DIDFTFlagStringPairs = {{
    {DIDFT_FFACTUATOR, "DIDFT_FFACTUATOR"},
    {DIDFT_FFEFFECTTRIGGER, "DIDFT_FFEFFECTTRIGGER"},
    {DIDFT_OUTPUT, "DIDFT_OUTPUT"},
    {DIDFT_VENDORDEFINED, "DIDFT_VENDORDEFINED"},
    {DIDFT_ALIAS, "DIDFT_ALIAS"},
    {DIDFT_OPTIONAL, "DIDFT_OPTIONAL"},
}};

constexpr static array<pair<DWORD, string_view>, 8> DIDOIStringPairs = {{
    {DIDOI_FFACTUATOR, "DIDOI_FFACTUATOR"},
    {DIDOI_FFEFFECTTRIGGER, "DIDOI_FFEFFECTTRIGGER"},
    {DIDOI_POLLED, "DIDOI_POLLED"},
    {DIDOI_ASPECTPOSITION, "DIDOI_ASPECTPOSITION"},
    {DIDOI_ASPECTVELOCITY, "DIDOI_ASPECTVELOCITY"},
    {DIDOI_ASPECTACCEL, "DIDOI_ASPECTACCEL"},
    {DIDOI_ASPECTFORCE, "DIDOI_ASPECTFORCE"},
    {DIDOI_GUIDISUSAGE, "DIDOI_GUIDISUSAGE"},
}};

constexpr static array<pair<DWORD, string_view>, 6> DISFFCStringPairs = {{
    {DISFFC_RESET, "DISFFC_RESET"},
    {DISFFC_STOPALL, "DISFFC_STOPALL"},
    {DISFFC_PAUSE, "DISFFC_PAUSE"},
    {DISFFC_CONTINUE, "DISFFC_CONTINUE"},
    {DISFFC_SETACTUATORSON, "DISFFC_SETACTUATORSON"},
    {DISFFC_SETACTUATORSOFF, "DISFFC_SETACTUATORSOFF"},
}};

string DIEFTToString(DWORD dwEffType) {
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

string DIEPToString(DWORD dwStaticParams) {
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

string DIDCToString(DWORD dwFlags) {
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

string DIPHHowToString(DWORD dwHow) {
	for (auto &&pair : DIPHStringPairs) {
		if (dwHow == pair.first)
			return string(pair.second);
	}

	return format("Unknown DIPH: {:#x}", dwHow);
}

string DIGFFSToString(DWORD ffstate) {
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

string DIEFFToString(DWORD dwFlags) {
	string str;
	for (auto &&pair : DIEFFTriggerStringPairs) {
		if (dwFlags & pair.first) {
			str += pair.second;
			break;
		}
	}

	str += " | ";

	for (auto &&pair : DIEFFDirectionStringPairs) {
		if (dwFlags & pair.first) {
			str += pair.second;
			break;
		}
	}

	return str;
}

string DIESToString(DWORD dwFlags) {
	string str;
	for (auto &&pair : DIESStringPairs) {
		if (dwFlags & pair.first) {
			if (!str.empty())
				str += " | ";
			str += pair.second;
		}
	}

	if (!str.empty())
		return str;

	return format("Unknown DIES: {:#x}", dwFlags);
}

string DIEGESToString(DWORD dwFlags) {
	string str;
	for (auto &&pair : DIEGESStringPairs) {
		if (dwFlags & pair.first) {
			if (!str.empty())
				str += " | ";
			str += pair.second;
		}
	}

	if (!str.empty())
		return str;

	return format("Unknown DIEGES: {:#x}", dwFlags);
}

string DIGDDToString(DWORD dwFlags) {
	string str;

	// 0 is a valid value without an associated #define
	if (dwFlags == 0)
		return "0";

	for (auto &&pair : DIGDDStringPairs) {
		if (dwFlags & pair.first) {
			if (!str.empty())
				str += " | ";
			str += pair.second;
		}
	}

	return format("Unknown DIGDD: {:#x}", dwFlags);
}

string DIJOFSToString(DWORD dwOfs, const DIDATAFORMAT &lpdf) {
	if (lpdf == c_dfDIKeyboard)
		return "Not Implemented DIKeyboard Offset";
	else if (lpdf == c_dfDIMouse)
		return "Not Implemented DIMouse Offset";
	else if (lpdf == c_dfDIMouse2)
		return "Not Implemented DIMouse2 Offset";
	else if (lpdf != c_dfDIJoystick && lpdf != c_dfDIJoystick2) {
		return "Unsupported DIDATAFORMAT";
	}

	for (auto &&pair : DIJOFSStringPairs) {
		if (dwOfs == pair.first)
			return string(pair.second);
	}

	DWORD buttonMax = DIJOFS_BUTTON(0);
	if (lpdf == c_dfDIJoystick)
		buttonMax = DIJOFS_BUTTON31;
	else if (lpdf == c_dfDIJoystick2)
		buttonMax = DIJOFS_BUTTON(127);

	if (dwOfs >= DIJOFS_BUTTON0 && dwOfs <= buttonMax) {
		return format("DIJOFS_BUTTON{}", dwOfs - DIJOFS_BUTTON0);
	}

	return format("Unknown DIJOFS: {:#x}", dwOfs);
}

string DIDFTToString(DWORD dwType) {
	string str;
	for (auto &&pair : DIDFTTypeStringPairs) {
		if (DIDFT_GETTYPE(dwType) == pair.first) {
			str += pair.second;
			break;
		}
	}

	DWORD inst = DIDFT_GETINSTANCE(dwType & DIDFT_INSTANCEMASK);

	if (inst > 0 && inst < 0xffff)
		str += format(" | DIDFT_INSTANCE/DIDFT_COLLECTION({})", inst);
	else if (inst == 0xffff)
		str += " | DIDFT_ANYINSTANCE/DIDFT_NOCOLLECTION";

	for (auto &&pair : DIDFTFlagStringPairs) {
		if (dwType & pair.first) {
			if (!str.empty())
				str += " | ";
			str += pair.second;
		}
	}

	if (str.empty())
		return format("Unknown DIDFT: {:#x}", dwType);

	return str;
}

string DIDOIToString(DWORD dwFlags) {
	string str;
	for (auto &&pair : DIDOIStringPairs) {
		if ((dwFlags & pair.first) == pair.first) {
			if (!str.empty())
				str += " | ";
			str += pair.second;
		}
	}

	if (!str.empty())
		return str;

	return format("Unknown DIDOI: {:#x}", dwFlags);
}

string DISFFCToString(DWORD dwFlags) {
	string str;
	for (auto &&pair : DISFFCStringPairs) {
		if (dwFlags & pair.first) {
			if (!str.empty())
				str += " | ";
			str += pair.second;
		}
	}

	if (!str.empty())
		return str;

	return format("Unknown DISFFC: {:#x}", dwFlags);
}

string DurationToString(DWORD duration) {
	if (duration == INFINITE)
		return "INFINITE";

	return format("{}", duration);
}

string TriggerToString(DWORD trigger) {
	if (trigger == DIEB_NOTRIGGER)
		return "DIEB_NOTRIGGER";

	return format("{}", trigger);
}

string DIPROPToString(REFGUID rguidProp) {
	for (auto &&pair : DIPROPStringPairs) {
		if (&rguidProp == &pair.first)
			return string(pair.second);
	}

	return format("Unknown DIPROP: {}", static_cast<const void *>(&rguidProp));
}

string DIDEVCAPSToString(const DIDEVCAPS &lpDIDevCaps) {
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

string DIDATAFORMATToString(LPCDIDATAFORMAT lpdf) {
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

string DIPROPHEADERToString(REFGUID rguidProp, const DIPROPHEADER &diph) {
	string str =
	    format("dwSize: {}, dwHeaderSize: {}, dwObj: {}, dwHow: {} ({:#x}), ",
	           diph.dwSize, diph.dwHeaderSize, diph.dwObj,
	           DIPHHowToString(diph.dwHow), diph.dwHow);

	if (&rguidProp == &DIPROP_APPDATA && diph.dwSize == sizeof(DIPROPPOINTER)) {
		auto pdipp = reinterpret_cast<LPCDIPROPPOINTER>(&diph);
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
		auto pdidw = reinterpret_cast<LPCDIPROPDWORD>(&diph);
		str += format("dwData: {} ({:#x})", pdidw->dwData, pdidw->dwData);
	} else if (&rguidProp == &DIPROP_GUIDANDPATH
	           && diph.dwSize == sizeof(DIPROPGUIDANDPATH)) {
		auto pdiguid = reinterpret_cast<LPCDIPROPGUIDANDPATH>(&diph);
		str += format("guidClass: {}, wszPath: {}",
		              guid_to_str(pdiguid->guidClass),
		              wstring_to_string(pdiguid->wszPath));
	} else if ((&rguidProp == &DIPROP_LOGICALRANGE
	            || &rguidProp == &DIPROP_PHYSICALRANGE
	            || &rguidProp == &DIPROP_RANGE)
	           && diph.dwSize == sizeof(DIPROPRANGE)) {
		auto pdirange = reinterpret_cast<LPCDIPROPRANGE>(&diph);
		str += format("lMin: {}, lMax: {}", pdirange->lMin, pdirange->lMax);
	} else if ((&rguidProp == &DIPROP_GETPORTDISPLAYNAME //
	            || &rguidProp == &DIPROP_INSTANCENAME    //
	            || &rguidProp == &DIPROP_KEYNAME         //
	            || &rguidProp == &DIPROP_PRODUCTNAME     //
	            || &rguidProp == &DIPROP_TYPENAME        //
	            || &rguidProp == &DIPROP_USERNAME)       //
	           && diph.dwSize == sizeof(DIPROPSTRING)) {
		auto pdistr = reinterpret_cast<LPCDIPROPSTRING>(&diph);
		str += format("wsz: {}", wstring_to_string(pdistr->wsz));
	} else if (&rguidProp == &DIPROP_CALIBRATION
	           || &rguidProp == &DIPROP_CPOINTS) {
		str += format("Unsupported property: {}", DIPROPToString(rguidProp));
	} else {
		str += format("Unknown property: {}", guid_to_str(rguidProp));
	}

	return str;
}

string DICONSTANTFORCEToString(const DICONSTANTFORCE &eff) {
	return format("lMagnitude: {}", eff.lMagnitude);
}

string DIRAMPFORCEToString(const DIRAMPFORCE &eff) {
	return format("lStart: {}, lEnd: {}", eff.lStart, eff.lEnd);
}

string DIPERIODICToString(const DIPERIODIC &eff) {
	return format("dwMagnitude: {}, lOffset: {}, dwPhase: {}, dwPeriod: {}",
	              eff.dwMagnitude, eff.lOffset, eff.dwPhase, eff.dwPeriod);
}

string DICONDITIONToString(const DICONDITION &eff) {
	return format("lOffset: {}, lPositiveCoefficient: {}, "
	              "lNegativeCoefficient: {}, dwPositiveSaturation: {}, "
	              "dwNegativeSaturation: {}, lDeadBand: {}",
	              eff.lOffset, eff.lPositiveCoefficient,
	              eff.lNegativeCoefficient, eff.dwPositiveSaturation,
	              eff.dwNegativeSaturation, eff.lDeadBand);
}

string DICUSTOMFORCEToString(const DICUSTOMFORCE &eff) {
	return format(
	    "cChannels: {}, dwSamplePeriod: {}, cSamples: {}, rglForceData: {}",
	    eff.cChannels, eff.dwSamplePeriod, eff.cSamples,
	    static_cast<void *>(eff.rglForceData));
}

string DIENVELOPEToString(const DIENVELOPE &eff) {
	return format("lpEnvelope: dwSize: {}, dwAttackLevel: {}, "
	              "dwFadeLevel: {}, dwAttackTime: {}, "
	              "dwFadeTime: {}",
	              eff.dwSize, eff.dwAttackLevel, eff.dwFadeLevel,
	              eff.dwAttackTime, eff.dwFadeTime);
}

string DIEFFECTToString(LPCDIEFFECT lpeff, DWORD dwEffType) {
	string str =
	    format("dwSize: {}, dwFlags: {}, dwDuration: {}, "
	           "dwSamplePeriod: {}, dwGain: {}, dwTriggerButton: {}, "
	           "dwTriggerRepeatInterval: {}, cAxes: {}",
	           lpeff->dwSize, DIEFFToString(lpeff->dwFlags),
	           DurationToString(lpeff->dwDuration), lpeff->dwSamplePeriod,
	           lpeff->dwGain, TriggerToString(lpeff->dwTriggerButton),
	           lpeff->dwTriggerRepeatInterval, lpeff->cAxes);

	if (lpeff->rgdwAxes) {
		for (DWORD i = 0; i < lpeff->cAxes; ++i) {
			str += format(", rgdwAxes[{}]: {}", i, lpeff->rgdwAxes[i]);
		}
	} else {
		str += ", rgdwAxes: null";
	}

	if (lpeff->rglDirection) {
		for (DWORD i = 0; i < lpeff->cAxes; ++i) {
			str += format(", rglDirection[{}]: {}", i, lpeff->rglDirection[i]);
		}
	} else {
		str += ", rglDirection: null";
	}

	if (lpeff->lpEnvelope) {
		str += format(", lpEnvelope: {{{}}}",
		              DIENVELOPEToString(*lpeff->lpEnvelope));
	} else {
		str += ", lpEnvelope: null";
	}

	str += format(", cbTypeSpecificParams: {}", lpeff->cbTypeSpecificParams);

	if (lpeff->lpvTypeSpecificParams) {
		str += ", lpvTypeSpecificParams: ";

		if (LOBYTE(dwEffType) == DIEFT_CONSTANTFORCE) {
			auto effconst =
			    static_cast<LPCDICONSTANTFORCE>(lpeff->lpvTypeSpecificParams);

			str += format("constantForce: {{{}}}",
			              DICONSTANTFORCEToString(*effconst));
		} else if (LOBYTE(dwEffType) == DIEFT_RAMPFORCE) {
			auto efframp =
			    static_cast<LPCDIRAMPFORCE>(lpeff->lpvTypeSpecificParams);

			str += format("rampForce: {{{}}}", DIRAMPFORCEToString(*efframp));
		} else if (LOBYTE(dwEffType) == DIEFT_PERIODIC) {
			auto effper =
			    static_cast<LPCDIPERIODIC>(lpeff->lpvTypeSpecificParams);

			str += format("periodic: {{{}}}", DIPERIODICToString(*effper));
		} else if (LOBYTE(dwEffType) == DIEFT_CONDITION) {
			auto lpEffCond =
			    static_cast<LPCDICONDITION>(lpeff->lpvTypeSpecificParams);

			str += "{";
			for (DWORD i = 0;
			     i < lpeff->cbTypeSpecificParams / sizeof(DICONDITION); i++) {
				if (i > 0)
					str += ", ";
				str += format("condition[{}]: {{{}}}", i,
				              DICONDITIONToString(lpEffCond[i]));
			}
			str += "}";
		} else if (LOBYTE(dwEffType) == DIEFT_CUSTOMFORCE) {
			auto customForce =
			    static_cast<LPCDICUSTOMFORCE>(lpeff->lpvTypeSpecificParams);

			str += format("customForce: {{{}}}",
			              DICUSTOMFORCEToString(*customForce));
		} else {
			str += format("Unknown effect type: {:#x}", dwEffType);
		}
	}

	str += format(", dwStartDelay: {}", lpeff->dwStartDelay);

	return str;
}

string DIEFFESCAPEToString(const DIEFFESCAPE &lpesc) {
	return format("dwSize: {}, dwCommand: {}, lpvInBuffer: {}, cbInBuffer {}, "
	              "lpvOutBuffer: {}, cbOutBuffer: {}",
	              lpesc.dwSize, lpesc.dwCommand, lpesc.lpvInBuffer,
	              lpesc.cbInBuffer, lpesc.lpvOutBuffer, lpesc.cbOutBuffer);
}

static string DIDODdwDataToString(DWORD dwData, const DIDATAFORMAT &lpdf,
                                  DWORD dwOfs) {
	if (lpdf == c_dfDIJoystick || lpdf == c_dfDIJoystick2) {
		DWORD buttonMax = DIJOFS_BUTTON(0);
		if (lpdf == c_dfDIJoystick)
			buttonMax = DIJOFS_BUTTON31;
		else if (lpdf == c_dfDIJoystick2)
			buttonMax = DIJOFS_BUTTON(127);

		if (dwOfs >= DIJOFS_BUTTON0 && dwOfs <= buttonMax) {
			if (dwData & 0x8)
				return "Pressed";
			else
				return "Released";
		}
	}

	return format("{:#x}", dwData);
}

string DIDEVICEOBJECTDATAToString(const DIDEVICEOBJECTDATA &rgdod,
                                  const DIDATAFORMAT &lpdf) {
	return format("dwOfs: {}, dwData: {}, dwTimeStamp: {}, dwSequence: {}, "
	              "uAppData: {:#x}",
	              DIJOFSToString(rgdod.dwOfs, lpdf),
	              DIDODdwDataToString(rgdod.dwData, lpdf, rgdod.dwOfs),
	              rgdod.dwTimeStamp, rgdod.dwSequence, rgdod.uAppData);
}

string DIJOYSTATEToString(const DIJOYSTATE &js) {
	string str = format("lX: {}, lY: {}, lZ: {}, lRx: {}, lRy: {}, lRz: {}",
	                    js.lX, js.lY, js.lZ, js.lRx, js.lRy, js.lRz);

	for (DWORD i = 0; i < 2; i++) {
		str += format(", rglSlider[{}]: {}", i, js.rglSlider[i]);
	}

	for (DWORD i = 0; i < 4; i++) {
		str += format(", rgdwPOV[{}]: {}", i, js.rgdwPOV[i]);
	}

	for (DWORD i = 0; i < 32; i++) {
		str += format(", rgbButtons[{}]: {}", i,
		              js.rgbButtons[i] & 0x8 ? "Down" : "Up");
	}

	return str;
}

string DIJOYSTATE2ToString(const DIJOYSTATE2 &js) {
	string str = DIJOYSTATEToString(reinterpret_cast<const DIJOYSTATE &>(js));

	for (DWORD i = 32; i < 128; i++) {
		str += format(", rgbButtons[{}]: {}", i,
		              js.rgbButtons[i] & 0x8 ? "Down" : "Up");
	}

	str += format(", lVX: {}, lVY: {}, lVZ: {}, lVRx: {}, lVRy: {}, "
	              "lVRz: {}",
	              js.lVX, js.lVY, js.lVZ, js.lVRx, js.lVRy, js.lVRz);

	for (DWORD i = 0; i < 2; i++) {
		str += format(", rglVSlider[{}]: {}", i, js.rglVSlider[i]);
	}

	str += format(", lAX: {}, lAY: {}, lAZ: {}, lARx: {}, lARy: {}, "
	              "lARz: {}",
	              js.lAX, js.lAY, js.lAZ, js.lARx, js.lARy, js.lARz);

	for (DWORD i = 0; i < 2; i++) {
		str += format(", rglASlider[{}]: {}", i, js.rglASlider[i]);
	}

	str += format(", lFX: {}, lFY: {}, lFZ: {}, lFRx: {}, lFRy: {}, "
	              "lFRz: {}",
	              js.lFX, js.lFY, js.lFZ, js.lFRx, js.lFRy, js.lFRz);

	for (DWORD i = 0; i < 2; i++) {
		str += format(", rglFSlider[{}]: {}", i, js.rglFSlider[i]);
	}

	return str;
}

template <typename IDInput>
string DIDEVICEOBJECTINSTANCEToString(
    const typename DITraits<IDInput>::DIDeviceObjectInstance &doi) {
	using DIDeviceObjectInstance = DITraits<IDInput>::DIDeviceObjectInstance;

	string str = format("dwSize: {}", doi.dwSize);

	if (doi.dwSize
	    >= offsetof(DIDeviceObjectInstance, guidType) + sizeof(doi.guidType)) {
		str += format(", guidType: ");

		if (doi.guidType == GUID_XAxis)
			str += "GUID_XAxis";
		else if (doi.guidType == GUID_YAxis)
			str += "GUID_YAxis";
		else if (doi.guidType == GUID_ZAxis)
			str += "GUID_ZAxis";
		else if (doi.guidType == GUID_RxAxis)
			str += "GUID_RxAxis";
		else if (doi.guidType == GUID_RyAxis)
			str += "GUID_RyAxis";
		else if (doi.guidType == GUID_RzAxis)
			str += "GUID_RzAxis";
		else if (doi.guidType == GUID_Slider)
			str += "GUID_Slider";
		else if (doi.guidType == GUID_Button)
			str += "GUID_Button";
		else if (doi.guidType == GUID_Key)
			str += "GUID_Key";
		else if (doi.guidType == GUID_POV)
			str += "GUID_POV";
		else if (doi.guidType == GUID_Unknown)
			str += "GUID_Unknown";
		else
			str += format("Unknown {}", guid_to_str(doi.guidType));
	}
	if (doi.dwSize
	    >= offsetof(DIDeviceObjectInstance, dwOfs) + sizeof(doi.dwOfs))
		str += format(", dwOfs: {}", doi.dwOfs);
	if (doi.dwSize
	    >= offsetof(DIDeviceObjectInstance, dwType) + sizeof(doi.dwType))
		str += format(", dwType: {} ({:#x})", DIDFTToString(doi.dwType),
		              doi.dwType);
	if (doi.dwSize
	    >= offsetof(DIDeviceObjectInstance, dwFlags) + sizeof(doi.dwFlags))
		str += format(", dwFlags: {} ({:#x})", DIDOIToString(doi.dwFlags),
		              doi.dwFlags);
	if (doi.dwSize
	    >= offsetof(DIDeviceObjectInstance, tszName) + sizeof(doi.tszName))
		str += format(", tszName: {}", ToString(doi.tszName));
	if (doi.dwSize >= offsetof(DIDeviceObjectInstance, dwFFMaxForce)
	                      + sizeof(doi.dwFFMaxForce))
		str += format(", dwFFMaxForce: {}", doi.dwFFMaxForce);
	if (doi.dwSize >= offsetof(DIDeviceObjectInstance, dwFFForceResolution)
	                      + sizeof(doi.dwFFForceResolution))
		str += format(", dwFFForceResolution: {}", doi.dwFFForceResolution);
	if (doi.dwSize >= offsetof(DIDeviceObjectInstance, wCollectionNumber)
	                      + sizeof(doi.wCollectionNumber))
		str += format(", wCollectionNumber: {}", doi.wCollectionNumber);
	if (doi.dwSize >= offsetof(DIDeviceObjectInstance, wDesignatorIndex)
	                      + sizeof(doi.wDesignatorIndex))
		str += format(", wDesignatorIndex: {}", doi.wDesignatorIndex);
	if (doi.dwSize >= offsetof(DIDeviceObjectInstance, wUsagePage)
	                      + sizeof(doi.wUsagePage))
		str += format(", wUsagePage: {}", doi.wUsagePage);
	if (doi.dwSize
	    >= offsetof(DIDeviceObjectInstance, wUsage) + sizeof(doi.wUsage))
		str += format(", wUsage: {}", doi.wUsage);
	if (doi.dwSize >= offsetof(DIDeviceObjectInstance, dwDimension)
	                      + sizeof(doi.dwDimension))
		str += format(", dwDimension: {}", doi.dwDimension);
	if (doi.dwSize
	    >= offsetof(DIDeviceObjectInstance, wExponent) + sizeof(doi.wExponent))
		str += format(", wExponent: {}", doi.wExponent);
	if (doi.dwSize
	    >= offsetof(DIDeviceObjectInstance, wReportId) + sizeof(doi.wReportId))
		str += format(", wReportId: {}", doi.wReportId);

	return str;
}

string wstring_to_string(const wstring &wstr) {
	if (wstr.empty())
		return string();

	int size_needed =
	    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), nullptr,
	                        0, nullptr, nullptr);
	string ret(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), ret.data(),
	                    size_needed, nullptr, nullptr);
	return ret;
}

template string DIDEVICEOBJECTINSTANCEToString<IDirectInput8A>(
    const typename DITraits<IDirectInput8A>::DIDeviceObjectInstance &doi);

template string DIDEVICEOBJECTINSTANCEToString<IDirectInput8W>(
    const typename DITraits<IDirectInput8W>::DIDeviceObjectInstance &doi);
