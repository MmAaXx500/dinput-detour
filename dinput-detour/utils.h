#pragma once

#include <dinput.h>
#include <windows.h>

#include <detours.h>

LONG DetourTransaction(void (*func)(void));

bool operator==(const DIDATAFORMAT &lhs, const DIDATAFORMAT &rhs);

/**
 * Copy a DIDATAFORMAT struct.
 * The structure contains pointers and must be freed using FreeDIDATAFORMAT
 * before the copy goes out of scope.
 *
 * @param dst pointer to the destination struct
 * @param src the source struct
 */
void CopyDIDATAFORMAT(DIDATAFORMAT *dst, const DIDATAFORMAT &src);

/**
 * Free a DIDATAFORMAT struct.
 * The structure must be a result of being copied using CopyDIDATAFORMAT before.
 *
 * @param diDf pointer to the struct to be freed
 */
void FreeDIDATAFORMAT(DIDATAFORMAT *diDf);
