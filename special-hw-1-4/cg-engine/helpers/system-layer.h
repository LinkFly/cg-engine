#pragma once

#include <cstdint>

#include <windows.h>

namespace cgEngine {

using namespace std;

unsigned long long getTimeSinceRun();
bool showConsoleCursor(bool bShow);
void clearConsole();
void pause();

}