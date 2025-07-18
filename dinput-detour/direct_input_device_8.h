#pragma once

#define CINTERFACE
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

/**
 * Attach to DirectInputDevice8 (A) functions
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 * @param rguid GUID of the device
 * @return Result of DetourTransaction
 */
LONG DirectInputDevice8DetourAttachA(LPDIRECTINPUTDEVICE8A lpDirectInputDevice,
                                     REFGUID rguid);

/**
 * Attach to DirectInputDevice8 (W) functions
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 * @param rguid GUID of the device
 * @return Result of DetourTransaction
 */
LONG DirectInputDevice8DetourAttachW(LPDIRECTINPUTDEVICE8W lpDirectInputDevice,
                                     REFGUID rguid);

/**
 * Detach from DirectInputDevice8 (A) functions
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 * @return Result of DetourTransaction
 */
LONG DirectInputDevice8DetourDetachA(LPDIRECTINPUTDEVICE8A lpDirectInputDevice);

/**
 * Detach from DirectInputDevice8 (W) functions
 *
 * @param lpDirectInputDevice Pointer to the DirectInput device.
 * @return Result of DetourTransaction
 */
LONG DirectInputDevice8DetourDetachW(LPDIRECTINPUTDEVICE8W lpDirectInputDevice);
