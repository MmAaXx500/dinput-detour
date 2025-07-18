#include "utils.h"

using namespace std;

LONG DetourTransaction(void (*func)(void)) {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	func();

	return DetourTransactionCommit();
}
