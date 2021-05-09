#include "errors-handler.h"

namespace cgEngine {

using namespace std;

void showError(const string& str) {
	cerr << str;
	exit(-1);
}

}