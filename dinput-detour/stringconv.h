#pragma once

#include <string>

#include <dinput.h>

#include "utils.h"

// DWORDs

std::string DIEFTToString(DWORD dwEffType);

std::string DIEPToString(DWORD dwStaticParams);

std::string DIDCToString(DWORD dwFlags);

std::string DIPHHowToString(DWORD dwHow);

std::string DIGFFSToString(DWORD ffstate);

std::string DIEFFToString(DWORD dwFlags);

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

// strings

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