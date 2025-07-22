#pragma once

#include <dinput.h>

/**
 * Attach to DirectInputEffect functions
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 * @param rguid GUID of the device
 * @return Result of DetourTransaction
 */
LONG DirectInputEffectDetourAttach(LPDIRECTINPUTEFFECT lpDiEffect);

/**
 * Detach from DirectInputEffect functions
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 * @return Result of DetourTransaction
 */
LONG DirectInputEffectDetourDetach(LPDIRECTINPUTEFFECT lpDiEffect);