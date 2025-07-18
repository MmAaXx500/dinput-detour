#pragma once

#define CINTERFACE
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

template <typename IDInput> struct DITraits;

template <> struct DITraits<IDirectInput8A> {
	using DInput = IDirectInput8A;
	using DInputVtbl = IDirectInput8AVtbl;
	using DIDevice = IDirectInputDevice8A;
	using DIDeviceInstance = DIDEVICEINSTANCEA;
	using DIDeviceVtbl = IDirectInputDevice8AVtbl;
	using DIEffectInfo = DIEFFECTINFOA;
	using DIEnumEffectsCallback = LPDIENUMEFFECTSCALLBACKA;
};

template <> struct DITraits<IDirectInput8W> {
	using DInput = IDirectInput8W;
	using DInputVtbl = IDirectInput8WVtbl;
	using DIDevice = IDirectInputDevice8W;
	using DIDeviceInstance = DIDEVICEINSTANCEW;
	using DIDeviceVtbl = IDirectInputDevice8WVtbl;
	using DIEffectInfo = DIEFFECTINFOW;
	using DIEnumEffectsCallback = LPDIENUMEFFECTSCALLBACKW;
};

/**
 * Attach to DirectInput8 functions
 *
 * @param lpDI Pointer to the DirectInput device.
 * @return Result of DetourTransaction
 */
template <typename IDInput>
LONG DirectInput8DetourAttach(typename DITraits<IDInput>::DInput *lpDI);

/**
 * Detach from DirectInput8 functions
 *
 * @param lpDI Pointer to the DirectInput device.
 * @return Result of DetourTransaction
 */
template <typename IDInput>
LONG DirectInput8DetourDetach(typename DITraits<IDInput>::DInput *lpDI);
