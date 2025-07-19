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

void CopyDIDATAFORMAT(DIDATAFORMAT *dst, const DIDATAFORMAT &src) {
	*dst = src;

	if (src.dwNumObjs == 0 || dst->dwSize < sizeof(DIDATAFORMAT))
		return;

	if (!src.rgodf)
		return;

	dst->rgodf = new DIOBJECTDATAFORMAT[src.dwNumObjs];

	for (DWORD i = 0; i < src.dwNumObjs; i++) {
		dst->rgodf[i] = src.rgodf[i];

		if (src.rgodf[i].pguid)
			dst->rgodf[i].pguid = new GUID(*src.rgodf[i].pguid);
	}
}

void FreeDIDATAFORMAT(DIDATAFORMAT *diDf) {
	if (diDf->dwNumObjs == 0 || diDf->dwSize < sizeof(DIDATAFORMAT))
		return;

	for (DWORD i = 0; i < diDf->dwNumObjs; i++) {
		delete diDf->rgodf[i].pguid;
	}

	delete diDf->rgodf;
}
