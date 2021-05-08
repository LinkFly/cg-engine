#pragma once

#include "Screen.h"

#include <string>

struct Message: public string {
	Message(const string& string) {
		this->clear();
		this->append(string);
	}
	Message(const char* msg) {
		*this = string{msg};
	}
};
struct Messages {
	static string collide() { static Message message = "collide"; return message; }
	static string nearTheBorder() { static Message message = "near-the-border"; return message; }
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