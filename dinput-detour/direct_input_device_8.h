#pragma once

#include <dinput.h>

#include "direct_input_8.h"

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
