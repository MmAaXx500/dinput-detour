#pragma once

#include <string>

#include <windows.h>

#include <detours.h>

std::string wstring_to_string(const std::wstring &wstr);

LONG DetourTransaction(void (*func)(void));