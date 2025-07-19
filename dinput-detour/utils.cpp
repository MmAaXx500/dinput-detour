#include "utils.h"

using namespace std;

LONG DetourTransaction(void (*func)(void)) {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	func();

	return DetourTransactionCommit();
}

bool operator==(const DIDATAFORMAT &lhs, const DIDATAFORMAT &rhs) {
	if (lhs.dwSize != rhs.dwSize || lhs.dwObjSize != rhs.dwObjSize
	    || lhs.dwFlags != rhs.dwFlags || lhs.dwDataSize != rhs.dwDataSize
	    || lhs.dwNumObjs != rhs.dwNumObjs)
		return false;

	return memcmp(lhs.rgodf, rhs.rgodf, lhs.dwNumObjs * lhs.dwObjSize);
}
