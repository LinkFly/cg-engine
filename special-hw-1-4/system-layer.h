#pragma once

#include <cstdint>
#include <windows.h>

using namespace std;

unsigned long long getTimeSinceRun();
bool showConsoleCursor(bool bShow);
void clearConsole();
void pause();