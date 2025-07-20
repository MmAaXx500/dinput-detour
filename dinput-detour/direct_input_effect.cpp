#include "direct_input_effect.h"
#include "direct_input_8.h"
#include "direct_input_device_8.h"
#include "log.h"
#include "stringconv.h"
#include "utils.h"
#include <winerror.h>

using namespace std;

static IDirectInputEffectVtbl RealDirectInputEffectVtbl = {};

static DWORD GetEffectTypeInfo(REFGUID rguid) {
	DIEFFECTINFOA dieffa = GetCachedEffectInfo<IDirectInput8A>(rguid);
	if (dieffa.dwSize != 0)
		return dieffa.dwEffType;

	return GetCachedEffectInfo<IDirectInput8W>(rguid).dwEffType;
}

HRESULT WINAPI RoutedDirectInputEffectGetParameters(
    LPDIRECTINPUTEFFECT lpDiEffect, LPDIEFFECT peff, DWORD dwFlags) {
	LOG_PRE("lpDiEffect: {}, peff: {}, dwFlags: {}\n",
	        static_cast<void *>(lpDiEffect), static_cast<void *>(peff),
	        DIEPToString(dwFlags));

	LOG_INFO("dwFlags: {}\n", DIEPToString(dwFlags));

	HRESULT ret =
	    RealDirectInputEffectVtbl.GetParameters(lpDiEffect, peff, dwFlags);

	if (SUCCEEDED(ret)) {
		GUID effGuid;
		RealDirectInputEffectVtbl.GetEffectGuid(lpDiEffect, &effGuid);

		DWORD effType = GetEffectTypeInfo(effGuid);
		LOG_INFO("peff: {}\n", DIEFFECTToString(peff, effType));
	}

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInputEffectSetParameters(
    LPDIRECTINPUTEFFECT lpDiEffect, LPCDIEFFECT peff, DWORD dwFlags) {
	LOG_PRE("lpDiEffect: {}, peff: {}, dwFlags: {:x}\n",
	        static_cast<void *>(lpDiEffect), static_cast<const void *>(peff),
	        dwFlags);

	LOG_INFO("dwFlags: {}\n", DIEPToString(dwFlags));

	GUID effGuid;
	RealDirectInputEffectVtbl.GetEffectGuid(lpDiEffect, &effGuid);

	DWORD effType = GetEffectTypeInfo(effGuid);
	LOG_INFO("peff: {}\n", DIEFFECTToString(peff, effType));

	HRESULT ret =
	    RealDirectInputEffectVtbl.SetParameters(lpDiEffect, peff, dwFlags);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInputEffectStart(LPDIRECTINPUTEFFECT lpDiEffect,
                                            DWORD dwIterations, DWORD dwFlags) {
	LOG_PRE("lpDiEffect: {}, dwIterations: {}, dwFlags: {}\n",
	        static_cast<void *>(lpDiEffect), dwIterations,
	        DIESToString(dwFlags));

	HRESULT ret =
	    RealDirectInputEffectVtbl.Start(lpDiEffect, dwIterations, dwFlags);
	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInputEffectStop(LPDIRECTINPUTEFFECT lpDiEffect) {
	LOG_PRE("lpDiEffect: {}\n", static_cast<void *>(lpDiEffect));

	HRESULT ret = RealDirectInputEffectVtbl.Stop(lpDiEffect);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInputEffectGetEffectStatus(
    LPDIRECTINPUTEFFECT lpDiEffect, LPDWORD pdwFlags) {
	LOG_PRE("lpDiEffect: {}, pdwFlags: {}\n", static_cast<void *>(lpDiEffect),
	        static_cast<void *>(pdwFlags));

	HRESULT ret =
	    RealDirectInputEffectVtbl.GetEffectStatus(lpDiEffect, pdwFlags);

	if (SUCCEEDED(ret) && pdwFlags)
		LOG_INFO("pdwFlags: {}\n", DIEGESToString(*pdwFlags));

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInputEffectDownload(LPDIRECTINPUTEFFECT lpDiEffect) {
	LOG_PRE("lpDiEffect: {}\n", static_cast<void *>(lpDiEffect));

	HRESULT ret = RealDirectInputEffectVtbl.Download(lpDiEffect);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInputEffectUnload(LPDIRECTINPUTEFFECT lpDiEffect) {
	LOG_PRE("lpDiEffect: {}\n", static_cast<void *>(lpDiEffect));

	HRESULT ret = RealDirectInputEffectVtbl.Unload(lpDiEffect);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInputEffectEscape(LPDIRECTINPUTEFFECT lpDiEffect,
                                             LPDIEFFESCAPE pesc) {
	LOG_PRE("lpDiEffect: {}, pesc: {}\n", static_cast<void *>(lpDiEffect),
	        static_cast<void *>(pesc));

	if (pesc)
		LOG_INFO("pesc: {}\n", DIEFFESCAPEToString(*pesc));

	HRESULT ret = RealDirectInputEffectVtbl.Escape(lpDiEffect, pesc);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

LONG DirectInputEffectDetourAttach(LPDIRECTINPUTEFFECT lpDiEffect) {
	LONG ret = NO_ERROR;

	if (RealDirectInputEffectVtbl.AddRef == nullptr && lpDiEffect) {
		RealDirectInputEffectVtbl = *lpDiEffect->lpVtbl;

		ret = DetourTransaction([]() {
			DetourAttach(&RealDirectInputEffectVtbl.GetParameters,
			             RoutedDirectInputEffectGetParameters);
			DetourAttach(&RealDirectInputEffectVtbl.SetParameters,
			             RoutedDirectInputEffectSetParameters);
			DetourAttach(&RealDirectInputEffectVtbl.Start,
			             RoutedDirectInputEffectStart);
			DetourAttach(&RealDirectInputEffectVtbl.Stop,
			             RoutedDirectInputEffectStop);
			DetourAttach(&RealDirectInputEffectVtbl.GetEffectStatus,
			             RoutedDirectInputEffectGetEffectStatus);
			DetourAttach(&RealDirectInputEffectVtbl.Download,
			             RoutedDirectInputEffectDownload);
			DetourAttach(&RealDirectInputEffectVtbl.Unload,
			             RoutedDirectInputEffectUnload);
			DetourAttach(&RealDirectInputEffectVtbl.Escape,
			             RoutedDirectInputEffectEscape);
		});
	}

	return ret;
}

LONG DirectInputEffectDetourDetach(LPDIRECTINPUTEFFECT lpDiEffect) {
	LONG ret = NO_ERROR;

	if (RealDirectInputEffectVtbl.AddRef != nullptr && lpDiEffect) {
		ret = DetourTransaction([]() {
			DetourDetach(&RealDirectInputEffectVtbl.GetParameters,
			             RoutedDirectInputEffectGetParameters);
			DetourDetach(&RealDirectInputEffectVtbl.SetParameters,
			             RoutedDirectInputEffectSetParameters);
			DetourDetach(&RealDirectInputEffectVtbl.Start,
			             RoutedDirectInputEffectStart);
			DetourDetach(&RealDirectInputEffectVtbl.Stop,
			             RoutedDirectInputEffectStop);
			DetourDetach(&RealDirectInputEffectVtbl.GetEffectStatus,
			             RoutedDirectInputEffectGetEffectStatus);
			DetourDetach(&RealDirectInputEffectVtbl.Download,
			             RoutedDirectInputEffectDownload);
			DetourDetach(&RealDirectInputEffectVtbl.Unload,
			             RoutedDirectInputEffectUnload);
			DetourDetach(&RealDirectInputEffectVtbl.Escape,
			             RoutedDirectInputEffectEscape);
		});

		RealDirectInputEffectVtbl = {};
	}

	return ret;
}