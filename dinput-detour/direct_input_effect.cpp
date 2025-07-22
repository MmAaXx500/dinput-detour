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

static HRESULT WINAPI DirectInputEffectGetParameters(
    LPDIRECTINPUTEFFECT lpDiEffect, LPDIEFFECT peff, DWORD dwFlags) {
	LOG_PRE("lpDiEffect: {}, peff: {}, dwFlags: {} ({:#x})\n",
	        static_cast<void *>(lpDiEffect), static_cast<void *>(peff),
	        DIEPToString(dwFlags), dwFlags);

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

static HRESULT WINAPI DirectInputEffectSetParameters(
    LPDIRECTINPUTEFFECT lpDiEffect, LPCDIEFFECT peff, DWORD dwFlags) {
	LOG_PRE("lpDiEffect: {}, peff: {}, dwFlags: {} ({:#x})\n",
	        static_cast<void *>(lpDiEffect), static_cast<const void *>(peff),
	        DIEPToString(dwFlags), dwFlags);

	GUID effGuid;
	RealDirectInputEffectVtbl.GetEffectGuid(lpDiEffect, &effGuid);

	DWORD effType = GetEffectTypeInfo(effGuid);
	LOG_INFO("peff: {}\n", DIEFFECTToString(peff, effType));

	HRESULT ret =
	    RealDirectInputEffectVtbl.SetParameters(lpDiEffect, peff, dwFlags);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

static HRESULT WINAPI DirectInputEffectStart(LPDIRECTINPUTEFFECT lpDiEffect,
                                             DWORD dwIterations,
                                             DWORD dwFlags) {
	LOG_PRE("lpDiEffect: {}, dwIterations: {}, dwFlags: {} ({:#x})\n",
	        static_cast<void *>(lpDiEffect), dwIterations,
	        DIESToString(dwFlags), dwFlags);

	HRESULT ret =
	    RealDirectInputEffectVtbl.Start(lpDiEffect, dwIterations, dwFlags);
	LOG_POST("ret: {}\n", ret);
	return ret;
}

static HRESULT WINAPI DirectInputEffectStop(LPDIRECTINPUTEFFECT lpDiEffect) {
	LOG_PRE("lpDiEffect: {}\n", static_cast<void *>(lpDiEffect));

	HRESULT ret = RealDirectInputEffectVtbl.Stop(lpDiEffect);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

static HRESULT WINAPI DirectInputEffectGetEffectStatus(
    LPDIRECTINPUTEFFECT lpDiEffect, LPDWORD pdwFlags) {
	LOG_PRE("lpDiEffect: {}, pdwFlags: {}\n", static_cast<void *>(lpDiEffect),
	        static_cast<void *>(pdwFlags));

	HRESULT ret =
	    RealDirectInputEffectVtbl.GetEffectStatus(lpDiEffect, pdwFlags);

	if (SUCCEEDED(ret) && pdwFlags)
		LOG_INFO("pdwFlags: {} ({:#x})\n", DIEGESToString(*pdwFlags),
		         *pdwFlags);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

static HRESULT WINAPI
DirectInputEffectDownload(LPDIRECTINPUTEFFECT lpDiEffect) {
	LOG_PRE("lpDiEffect: {}\n", static_cast<void *>(lpDiEffect));

	HRESULT ret = RealDirectInputEffectVtbl.Download(lpDiEffect);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

static HRESULT WINAPI DirectInputEffectUnload(LPDIRECTINPUTEFFECT lpDiEffect) {
	LOG_PRE("lpDiEffect: {}\n", static_cast<void *>(lpDiEffect));

	HRESULT ret = RealDirectInputEffectVtbl.Unload(lpDiEffect);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

static HRESULT WINAPI DirectInputEffectEscape(LPDIRECTINPUTEFFECT lpDiEffect,
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
			             DirectInputEffectGetParameters);
			DetourAttach(&RealDirectInputEffectVtbl.SetParameters,
			             DirectInputEffectSetParameters);
			DetourAttach(&RealDirectInputEffectVtbl.Start,
			             DirectInputEffectStart);
			DetourAttach(&RealDirectInputEffectVtbl.Stop,
			             DirectInputEffectStop);
			DetourAttach(&RealDirectInputEffectVtbl.GetEffectStatus,
			             DirectInputEffectGetEffectStatus);
			DetourAttach(&RealDirectInputEffectVtbl.Download,
			             DirectInputEffectDownload);
			DetourAttach(&RealDirectInputEffectVtbl.Unload,
			             DirectInputEffectUnload);
			DetourAttach(&RealDirectInputEffectVtbl.Escape,
			             DirectInputEffectEscape);
		});
	}

	return ret;
}

LONG DirectInputEffectDetourDetach(LPDIRECTINPUTEFFECT lpDiEffect) {
	LONG ret = NO_ERROR;

	if (RealDirectInputEffectVtbl.AddRef != nullptr && lpDiEffect) {
		ret = DetourTransaction([]() {
			DetourDetach(&RealDirectInputEffectVtbl.GetParameters,
			             DirectInputEffectGetParameters);
			DetourDetach(&RealDirectInputEffectVtbl.SetParameters,
			             DirectInputEffectSetParameters);
			DetourDetach(&RealDirectInputEffectVtbl.Start,
			             DirectInputEffectStart);
			DetourDetach(&RealDirectInputEffectVtbl.Stop,
			             DirectInputEffectStop);
			DetourDetach(&RealDirectInputEffectVtbl.GetEffectStatus,
			             DirectInputEffectGetEffectStatus);
			DetourDetach(&RealDirectInputEffectVtbl.Download,
			             DirectInputEffectDownload);
			DetourDetach(&RealDirectInputEffectVtbl.Unload,
			             DirectInputEffectUnload);
			DetourDetach(&RealDirectInputEffectVtbl.Escape,
			             DirectInputEffectEscape);
		});

		RealDirectInputEffectVtbl = {};
	}

	return ret;
}