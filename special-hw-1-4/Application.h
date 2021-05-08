#pragma once

#include "Screen.h"
#include "errors-handler.h"
#include "system-layer.h"

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

struct MessageData {
	template<class T>
	T getSuccessor() {
		auto pThis = dynamic_cast<T*>(this);
		if (!pThis)
			showError("Bad casting message data to this successor");
		return *pThis;
	}
  private:
    // Defining it as polymorphic type
	  virtual void stub() {}
};

enum class Border {
	none, left, right, top, bottom
};

struct BordersMessageData : MessageData {
	vector<Border> getBorders() {
		vector<Border> res;
		if (bLeft) res.push_back(Border::left);
		if (bRight) res.push_back(Border::right);
		if (bTop) res.push_back(Border::top);
		if (bBottom) res.push_back(Border::bottom);
		return res;
	}
	bool bLeft = false;
	bool bRight = false;
	bool bTop = false;
	bool bBottom = false;
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
	static void hideCursor() {
		ShowConsoleCursor(false);
	}
};