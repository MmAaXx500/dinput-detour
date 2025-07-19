#pragma once

#include <dinput.h>
#include <windows.h>

#include <detours.h>

LONG DetourTransaction(void (*func)(void));

bool operator==(const DIDATAFORMAT &lhs, const DIDATAFORMAT &rhs);

