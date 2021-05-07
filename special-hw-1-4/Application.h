#pragma once

#include "Screen.h"

#include <string>

struct Messages {
	static string collide() { static string message = "collide"; return message; }
	static string nearTheBorder() { static string message = "near-the-border"; return message; }
};

struct Application {
	static IScreen* screen;
	static IScreen* getScreen() {
		return screen;
	}
	static void setScreen(IScreen* screen) {
		Application::screen = screen;
	}
};