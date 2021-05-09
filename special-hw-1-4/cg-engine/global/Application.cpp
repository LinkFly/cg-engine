#include "Application.h"

#include <iostream>
#include <conio.h>
namespace cgEngine {

using namespace std;

IScreen* Application::screen = nullptr;

IScreen* Application::getScreen() {
	return screen;
}

void Application::setScreen(IScreen* screen) {
	Application::screen = screen;
}

void Application::hideCursor() {
	showConsoleCursor(false);
}

void Application::pause() {
	//cgEngine::pause();
	while (true)
	{
		//char ch;
		//cin >> ch;
		//clearConsole();
		int ch = _getch();
	}
	//_getch();
}

}