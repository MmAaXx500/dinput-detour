#pragma once

#include <windows.h>

#include <detours.h>

LONG DetourTransaction(void (*func)(void));