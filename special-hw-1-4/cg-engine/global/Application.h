#pragma once

#include "Screen.h"
#include "../helpers/errors-handler.h"
#include "../helpers/system-layer.h"
#include "../entities/messages.h"

namespace cgEngine {

struct Application {
	static IScreen* screen;
	static void setScreen(IScreen*);
	static IScreen* getScreen();
	static void hideCursor();
	static void pause();
};

}