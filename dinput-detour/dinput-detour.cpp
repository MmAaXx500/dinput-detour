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
#include "utils.h"

using namespace std;

static CHAR s_szDllPath[MAX_PATH];

typedef HRESULT(WINAPI DirectInput8CreateFn)(HINSTANCE hinst, DWORD dwVersion,
                                             REFIID riidltf, LPVOID *ppvOut,
                                             LPUNKNOWN punkOuter);

static auto RealDirectInput8Create = reinterpret_cast<DirectInput8CreateFn *>(
    GetProcAddress(GetModuleHandleA("dinput8.dll"), "DirectInput8Create"));

typedef BOOL(WINAPI CreateProcessWFn)(
    LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
    DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);

static auto RealCreateProcessW = reinterpret_cast<CreateProcessWFn *>(
    GetProcAddress(GetModuleHandleA("kernelbase.dll"), "CreateProcessW"));

typedef HRESULT WINAPI CoCreateInstanceFn(REFCLSID rclsid, LPUNKNOWN pUnkOuter,
                                          DWORD dwClsContext, REFIID riid,
                                          LPVOID *ppv);

static auto RealCoCreateInstance = reinterpret_cast<CoCreateInstanceFn *>(
    GetProcAddress(GetModuleHandleA("ole32.dll"), "CoCreateInstance"));

BOOL WINAPI RoutedCreateProcessW(
    LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
    DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation) {
	printf("CreateProcessW(%ls,%ls,%p,%p,%x,%lx,%p,%ls,%p,%p)\n",
	       lpApplicationName, lpCommandLine, lpProcessAttributes,
	       lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
	       lpCurrentDirectory, lpStartupInfo, lpProcessInformation);

	printf("Calling DetourCreateProcessWithDllExW(,%hs)\n", s_szDllPath);

	BOOL rv = 0;
	__try {
		rv = DetourCreateProcessWithDllExW(
		    lpApplicationName, lpCommandLine, lpProcessAttributes,
		    lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
		    lpCurrentDirectory, lpStartupInfo, lpProcessInformation,
		    s_szDllPath, RealCreateProcessW);
	} __finally {
		printf("CreateProcessW(,,,,,,,,,) -> %x\n", rv);
	};
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
		if (IsEqualIID(riidltf, IID_IDirectInput8A)
		    && RealDirectInput8VtblA.AddRef == nullptr) {
			LOG_INFO("Attach vtableA: {}\n",
			         static_cast<void *>(&((LPDIRECTINPUT8W)*ppvOut)->lpVtbl));
			RealDirectInput8VtblA = *((LPDIRECTINPUT8A)*ppvOut)->lpVtbl;

			DetourTransaction([]() {
				DetourAttach(&RealDirectInput8VtblA.CreateDevice,
				             RoutedDirectInput8CreateDeviceA);
			});
		} else if (IsEqualIID(riidltf, IID_IDirectInput8W)
		           && RealDirectInput8VtblW.AddRef == nullptr) {
			LOG_INFO("Attach vtableW: {}\n",
			         static_cast<void *>(&((LPDIRECTINPUT8W)*ppvOut)->lpVtbl));
			RealDirectInput8VtblW = *(((LPDIRECTINPUT8W)*ppvOut)->lpVtbl);

			DetourTransaction([]() {
				DetourAttach(&RealDirectInput8VtblW.CreateDevice,
				             RoutedDirectInput8CreateDeviceW);
			});
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
