#include "system-layer.h"

unsigned long long getTimeSinceRun() { return GetTickCount64(); }

bool ShowConsoleCursor(bool bShow)
{
	CONSOLE_CURSOR_INFO cci;
	HANDLE hStdOut;
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE)
		return false;
	if (!GetConsoleCursorInfo(hStdOut, &cci))
		return false;
	cci.bVisible = bShow;
	if (!SetConsoleCursorInfo(hStdOut, &cci))
		return false;
	return true;
}