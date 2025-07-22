#pragma once

#include <string>

#include "direct_input_8.h"

// DWORDs

std::string DIEFTToString(DWORD dwEffType);

std::string DIEPToString(DWORD dwStaticParams);

std::string DIDCToString(DWORD dwFlags);

std::string DIPHHowToString(DWORD dwHow);

std::string DIGFFSToString(DWORD ffstate);

std::string DIEFFToString(DWORD dwFlags);

std::string DIESToString(DWORD dwFlags);

std::string DIEGESToString(DWORD dwFlags);

std::string DIGDDToString(DWORD dwFlags);

std::string DIJOFSToString(DWORD dwOfs, const DIDATAFORMAT &lpdf);

std::string DIDFTToString(DWORD dwType);

std::string DIDOIToString(DWORD dwFlags);

std::string DISFFCToString(DWORD dwFlags);

std::string DI8DEVTYPEOrCLASSToString(DWORD dwDevType);

std::string DIEDFLToString(DWORD dwFlags);

std::string DurationToString(DWORD duration);

std::string TriggerToString(DWORD trigger);

// Structs

std::string DIPROPToString(REFGUID rguidProp);

std::string DIDEVCAPSToString(const DIDEVCAPS &lpDIDevCaps);

std::string DIDATAFORMATToString(LPCDIDATAFORMAT lpdf);

std::string DIPROPHEADERToString(REFGUID rguidProp, const DIPROPHEADER &diph);

std::string DICONSTANTFORCEToString(const DICONSTANTFORCE &eff);

std::string DIRAMPFORCEToString(const DIRAMPFORCE &eff);

std::string DIPERIODICToString(const DIPERIODIC &eff);

std::string DICONDITIONToString(const DICONDITION &eff);

std::string DICUSTOMFORCEToString(const DICUSTOMFORCE &eff);

std::string DIENVELOPEToString(const DIENVELOPE &eff);

std::string DIEFFECTToString(LPCDIEFFECT lpeff, DWORD dwEffType);

std::string DIEFFESCAPEToString(const DIEFFESCAPE &lpesc);

std::string DIDEVICEOBJECTDATAToString(const DIDEVICEOBJECTDATA &rgdod,
                                       const DIDATAFORMAT &lpdf);

std::string DIJOYSTATEToString(const DIJOYSTATE &js);

std::string DIJOYSTATE2ToString(const DIJOYSTATE2 &js);

template <typename IDInput>
std::string DIDEVICEOBJECTINSTANCEToString(
    const typename DITraits<IDInput>::DIDeviceObjectInstance &doi);

template <typename IDInput>
std::string DIDEVICEINSTANCEToString(
    const typename DITraits<IDInput>::DIDeviceInstance &ddi);

// strings

std::string wstring_to_string(const std::wstring &wstr);

template <typename T> std::string ToString(const T &str) {
	if constexpr (std::is_same_v<std::decay_t<T>, std::string>
	              || std::is_convertible_v<T, std::string_view>)
		return str;
	else if constexpr (std::is_convertible_v<T, std::wstring>
	                   || std::is_convertible_v<T, std::wstring_view>)
		return wstring_to_string(str);
	else {
		static_assert(false, "Unsupported type");
	}
}
