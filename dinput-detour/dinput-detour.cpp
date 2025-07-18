// dinput-detour.cpp : Defines the entry point for the application.
//

// #include "dinput-detour.h"

#define CINTERFACE

#include <filesystem>
#include <format>

#include <windows.h>
#include <winerror.h>

#include <detours.h>

#include "direct_input_8.h"
#include "log.h"
#include "stringconv.h"
#include "utils.h"

using namespace std;

static CHAR s_szDllPath[MAX_PATH];

typedef HRESULT WINAPI DirectInput8CreateFn(HINSTANCE hinst, DWORD dwVersion,
                                            REFIID riidltf, LPVOID *ppvOut,
                                            LPUNKNOWN punkOuter);

static auto RealDirectInput8Create =
    reinterpret_cast<DirectInput8CreateFn *>(reinterpret_cast<void *>(
        GetProcAddress(GetModuleHandleA("dinput8.dll"), "DirectInput8Create")));

typedef BOOL WINAPI CreateProcessWFn(
    LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
    DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);

static auto RealCreateProcessW =
    reinterpret_cast<CreateProcessWFn *>(reinterpret_cast<void *>(
        GetProcAddress(GetModuleHandleA("kernelbase.dll"), "CreateProcessW")));

typedef HRESULT WINAPI CoCreateInstanceFn(REFCLSID rclsid, LPUNKNOWN pUnkOuter,
                                          DWORD dwClsContext, REFIID riid,
                                          LPVOID *ppv);

static auto RealCoCreateInstance =
    reinterpret_cast<CoCreateInstanceFn *>(reinterpret_cast<void *>(
        GetProcAddress(GetModuleHandleA("ole32.dll"), "CoCreateInstance")));

BOOL WINAPI RoutedCreateProcessW(
    LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
    DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation) {
	LOG_PRE(
	    "lpApplicationName: {}, lpCommandLine: {}, lpProcessAttributes: {}, "
	    "lpThreadAttributes: {}, bInheritHandles: {}, dwCreationFlags: {:x}, "
	    "lpEnvironment: {}, lpCurrentDirectory: {}, lpStartupInfo: {}, "
	    "lpProcessInformation: {}\n",
	    static_cast<const void *>(lpApplicationName),
	    static_cast<const void *>(lpCommandLine),
	    static_cast<void *>(lpProcessAttributes),
	    static_cast<void *>(lpThreadAttributes), bInheritHandles,
	    dwCreationFlags, static_cast<void *>(lpEnvironment),
	    static_cast<const void *>(lpCurrentDirectory),
	    static_cast<void *>(lpStartupInfo),
	    static_cast<void *>(lpProcessInformation));

	LOG_INFO("Calling DetourCreateProcessWithDllExW lpApplicationName: {},"
	         "lpCommandLine: {}, lpCurrentDirectory: {}, lpDllName: {}\n",
	         wstring_to_string(lpApplicationName),
	         wstring_to_string(lpCommandLine),
	         wstring_to_string(lpCurrentDirectory), s_szDllPath);

	BOOL rv = DetourCreateProcessWithDllExW(
	    lpApplicationName, lpCommandLine, lpProcessAttributes,
	    lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
	    lpCurrentDirectory, lpStartupInfo, lpProcessInformation, s_szDllPath,
	    RealCreateProcessW);

	LOG_POST("rv: {}\n", rv);
	return rv;
}

HRESULT WINAPI RoutedCoCreateInstance(REFIID rclsid, LPUNKNOWN pUnkOuter,
                                      DWORD dwClsContext, REFIID riid,
                                      LPVOID *ppv) {
	LOG_PRE(
	    "rclsid: {}, pUnkOuter: {}, dwClsContext: {:x}, riid: {}, ppv: {}\n",
	    guid_to_str(rclsid), static_cast<void *>(pUnkOuter), dwClsContext,
	    guid_to_str(riid), static_cast<void *>(ppv));

	HRESULT ret =
	    RealCoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);

	LOG_POST("ret: {}\n", ret);
	return ret;
}

HRESULT WINAPI RoutedDirectInput8Create(HINSTANCE hinst, DWORD dwVersion,
                                        REFIID riidltf, LPVOID *ppvOut,
                                        LPUNKNOWN punkOuter) {
	LOG_PRE(
	    "hinst: {} dwVersion: {:x}, riidltf: {}, ppvOut: {} punkOuter: {}\n",
	    static_cast<void *>(hinst), dwVersion, guid_to_str(riidltf),
	    static_cast<void *>(ppvOut), static_cast<void *>(punkOuter));

	DWORD ret =
	    RealDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);

	if (ret == DI_OK) {
		if (IsEqualIID(riidltf, IID_IDirectInput8A)) {
			DirectInput8DetourAttach<IDirectInput8A>((IDirectInput8A *)*ppvOut);
		} else if (IsEqualIID(riidltf, IID_IDirectInput8W)) {
			DirectInput8DetourAttach<IDirectInput8W>((IDirectInput8W *)*ppvOut);
		} else {
			LOG_ERR("Unknown IID: {}\n", guid_to_str(riidltf));
		}
	}

	LOG_POST("ret: {}\n", ret);

	return ret;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
	LONG error;
	(void)hinst;
	(void)reserved;

	if (DetourIsHelperProcess()) {
		return TRUE;
	}

	GetModuleFileName(hinst, s_szDllPath, ARRAYSIZE(s_szDllPath));

	if (dwReason == DLL_PROCESS_ATTACH) {
		DetourRestoreAfterWith();

		// Create logfile
		// format: some/path/<process name>-<PID>-dinput.log

		CHAR procname[MAX_PATH] = {0};
		GetModuleFileName(GetModuleHandle(nullptr), procname, MAX_PATH);
		DWORD procid = GetCurrentProcessId();

		if (logger == nullptr) {
			logger = new Logger(
			    format("{}-{}-dinput.log",
			           filesystem::path(procname).filename().string(), procid));
		}

		// Detour functions

		error = DetourTransaction([]() {
			// DetourAttach(&(PVOID &)TrueCoCreateInstance,
			// RoutedCoCreateInstance);
			DetourAttach(&RealDirectInput8Create, RoutedDirectInput8Create);
			// DetourAttach(&(PVOID &)Real_CreateProcessW, Mine_CreateProcessW);
		});

		if (error != NO_ERROR) {
			LOG_ERR("Attach DetourTransaction failed: {}\n", error);
		}
	} else if (dwReason == DLL_PROCESS_DETACH) {
		error = DetourTransaction([]() {
			// DetourDetach(&(PVOID &)TrueCoCreateInstance,
			// RoutedCoCreateInstance);
			DetourDetach(&RealDirectInput8Create, RoutedDirectInput8Create);
			// DetourDetach(&(PVOID &)Real_CreateProcessW, Mine_CreateProcessW);
		});

		if (error != NO_ERROR) {
			LOG_ERR("Detach DetourTransaction failed: {}\n", error);
		}
	}
	return TRUE;
}
