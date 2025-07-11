#include "utils.h"

using namespace std;

string wstring_to_string(const wstring &wstr) {
	if (wstr.empty())
		return string();

	int size_needed =
	    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), nullptr,
	                        0, nullptr, nullptr);
	string ret(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), ret.data(),
	                    size_needed, nullptr, nullptr);
	return ret;
}

LONG DetourTransaction(void (*func)(void)) {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	func();

	return DetourTransactionCommit();
}
