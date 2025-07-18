#pragma once

#include <dinput.h>

#include "direct_input_8.h"

/**
 * Get a cached EffecInfo by GUID.
 * The cache is filled after a successfull attach.
 *
 * @param rguid GUID of the effect
 * @return DIEffectInfo structure, zeroed on error
 */
template <typename IDInput>
DITraits<IDInput>::DIEffectInfo GetCachedEffectInfo(REFGUID rguid);

/**
 * Attach to DirectInputDevice8 functions
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 * @param rguid GUID of the device
 * @return Result of DetourTransaction
 */
template <typename IDInput>
LONG DirectInputDevice8DetourAttach(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice, REFGUID rguid);

/**
 * Detach from DirectInputDevice8 functions
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 * @return Result of DetourTransaction
 */
template <typename IDInput>
LONG DirectInputDevice8DetourDetach(
    typename DITraits<IDInput>::DIDevice *lpDirectInputDevice);
