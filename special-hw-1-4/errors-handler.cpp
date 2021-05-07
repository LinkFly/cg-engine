#include "errors-handler.h"

void showError(const string& str) {
	cerr << str;
	exit(-1);
}