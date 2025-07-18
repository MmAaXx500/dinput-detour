#pragma once

#include <dinput.h>

HRESULT WINAPI RoutedDirectInputEffectGetParameters(
    LPDIRECTINPUTEFFECT lpDiEffect, LPDIEFFECT peff, DWORD dwFlags);

HRESULT WINAPI RoutedDirectInputEffectSetParameters(
    LPDIRECTINPUTEFFECT lpDiEffect, LPCDIEFFECT peff, DWORD dwFlags);

HRESULT WINAPI RoutedDirectInputEffectStart(LPDIRECTINPUTEFFECT lpDiEffect,
                                            DWORD dwIterations, DWORD dwFlags);

HRESULT WINAPI RoutedDirectInputEffectStop(LPDIRECTINPUTEFFECT lpDiEffect);

HRESULT WINAPI RoutedDirectInputEffectGetEffectStatus(
    LPDIRECTINPUTEFFECT lpDiEffect, LPDWORD pdwFlags);

HRESULT WINAPI RoutedDirectInputEffectDownload(LPDIRECTINPUTEFFECT lpDiEffect);

HRESULT WINAPI RoutedDirectInputEffectUnload(LPDIRECTINPUTEFFECT lpDiEffect);

HRESULT WINAPI RoutedDirectInputEffectEscape(LPDIRECTINPUTEFFECT lpDiEffect,
                                             LPDIEFFESCAPE pesc);

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