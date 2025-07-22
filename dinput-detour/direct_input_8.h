#pragma once

#include <dinput.h>

template <typename IDInput> struct DITraits;

template <> struct DITraits<IDirectInput8A> {
	static constexpr char api_sfx = 'A';
	using DInput = IDirectInput8A;
	using DInputVtbl = IDirectInput8AVtbl;
	using DIDevice = IDirectInputDevice8A;
	using DIDeviceInstance = DIDEVICEINSTANCEA;
	using DIDeviceObjectInstance = DIDEVICEOBJECTINSTANCEA;
	using DIDeviceVtbl = IDirectInputDevice8AVtbl;
	using DIEffectInfo = DIEFFECTINFOA;
	using DIEnumDeviceObjectCallback = LPDIENUMDEVICEOBJECTSCALLBACKA;
	using DIEnumDevicesCallback = LPDIENUMDEVICESCALLBACKA;
	using DIEnumEffectsCallback = LPDIENUMEFFECTSCALLBACKA;
};

template <> struct DITraits<IDirectInput8W> {
	static constexpr char api_sfx = 'W';
	using DInput = IDirectInput8W;
	using DInputVtbl = IDirectInput8WVtbl;
	using DIDevice = IDirectInputDevice8W;
	using DIDeviceInstance = DIDEVICEINSTANCEW;
	using DIDeviceObjectInstance = DIDEVICEOBJECTINSTANCEW;
	using DIDeviceVtbl = IDirectInputDevice8WVtbl;
	using DIEffectInfo = DIEFFECTINFOW;
	using DIEnumDeviceObjectCallback = LPDIENUMDEVICEOBJECTSCALLBACKW;
	using DIEnumDevicesCallback = LPDIENUMDEVICESCALLBACKW;
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
