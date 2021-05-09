#pragma once

#include "Screen.h"
#include "../entities/Objects.h"
#include "Application.h"
#include "../entities/Timer.h"

#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <iostream>
#include <queue>
#include <map>
#include <set>
#include <array>
#include <cstdlib>
#include <ctime>

//////
#include <conio.h>
#include <windows.h>

using namespace std;

namespace cgEngine {

enum class Type {
	bytes, string
};

enum class EventType {
	tick
};

struct Data {
	Type type;
	vector<uint8_t> payload;
	Data() = default;
	Data(const string& str) {
		for (char ch : str) {
			payload.push_back(ch);
		}
		type = Type::string;
	}
	string toString() {
		string res;
		for (char ch : payload) {
			res += ch;
		}
		return res;
	}
};

class Event {
public:
	EventType eventType;
	Data data;
};

using EventCallback = function<bool(Event&)>;

struct IGame {

};

class Input {
	//map<char, set<function<void(char, short)>>> handlers;
	map<vector<char>, set<function<void(const vector<char>&, map<char, bool>)>*>> handlers;
  public:
	void add(char ch, function<void(const vector<char>&, map<char, bool>)>& handler) {
		auto& handlersSet = handlers[{ch}];
		handlersSet.insert(&handler);
	}
	void add(vector<char> chars, function<void(const vector<char>&, map<char, bool>)>& handler) {
		auto& handlersSet = handlers[chars];
		handlersSet.insert(&handler);
	}
	void remove(char ch, function<void(const vector<char>&, map<char, bool>)>& handler) {
		auto handlersSet = handlers[{ch}];
		handlersSet.erase(&handler);
	}
	void remove(vector<char> chars, function<void(const vector<char>&, map<char, bool>)>& handler) {
		auto handlersSet = handlers[chars];
		handlersSet.erase(&handler);
	}
	void process() {
		for (auto& handlersSetSpec : handlers) {
			auto& chars = handlersSetSpec.first;
			map<char, bool> states;
			for (char ch : chars) {
				auto state = GetKeyState(VkKeyScanA(ch));
				states[ch] = state & (1 << 7);
			}
			
			/*if (state & (1 << 7)) {*/
			for (auto& handler : handlersSetSpec.second)
				(*handler)(handlersSetSpec.first, states);
				/*(handler)(handlersSetSpec.first, states);*/
			//}
		}
	}
};

class Game : public IGame, public Object {
	queue<Event> events;
	map<EventType, vector<EventCallback>> eventsHandlers;
	Factory<Root> factory;
	Input& input;

	
	void sleep(int ms) {
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}
	void mainLoop() {
		while (!bStop) {
			while (!events.empty()) {
				Event event = events.front();
				events.pop();
				auto& handlers = eventsHandlers[event.eventType];
				for (auto& handler : handlers) {
					handler(event);
				}
			}
			addTickEvent();
			/*sleep(1);*/
		}
	}
	EventCallback tickHandler;
	Event createTickEvent() {
		Event evTick;
		evTick.eventType = EventType::tick;
		Data tickEventData("Hello GameEngine!");
		evTick.data = tickEventData;
		return evTick;
	}
	void addTickEvent() {
		events.push(createTickEvent());
	}
	void handlerTreeInner(IScreen& screen, shared_ptr<IObject> pObject) {
		if (!pObject) return;
		pObject->Tick();
		auto pObj = dynamic_cast<ViewObject*>(pObject.get());
		if (pObj) {
			auto shapes = pObj->getShapes();
			for (auto& shape : *shapes) {//auto it = shapes->begin(); it != shapes->end(); ++it) {
				// TODO RESTORE IT!!!
				//shape->render(*screen.addGetBuffer());
				shape->render(screen);
				//(*it)->render(screen);
			}
		}
		if (pObject && pObject->getChildren()) {
			/*auto lineObj = make_shared<ViewObject>();
			auto shapes = new ShapeCollection
			lineObj->setShapes();
			tree->getChildren()->add(make_shared<IObject>())*/
			auto size = pObject->getChildren()->size();
			for (decltype(size) i = 0; i < size; ++i) {
				handlerTreeInner(screen, pObject->getChildren()->get(i));
			}
		}
	}
	void handlerCollisions() {
		IObject* pObj = nullptr;
		//decltype(GlobalFactoryComponent::allViewObjects[0]) pObj();
		
		auto itCur = GlobalFactoryComponent::allViewObjects.begin();
		auto itEnd = GlobalFactoryComponent::allViewObjects.end();
		while (itCur != itEnd) {
			auto pObj = itCur->second;
			auto nestedCur = itCur;
			++nestedCur;
			while (nestedCur != itEnd) {
				auto pOtherObj = nestedCur->second;
				if (pOtherObj == pObj) continue;
				IView* pViewObj = dynamic_cast<IView*>(pObj);
				IView* pOtherViewObj = dynamic_cast<IView*>(nestedCur->second);
				if (pViewObj && pOtherViewObj) {
					if (pViewObj->isCollide(*pOtherViewObj)) {
						pViewObj->message(pOtherViewObj, Messages::collide());
						if (pOtherViewObj->getEnabledCollision())
							pOtherViewObj->message(pViewObj, Messages::collide());	
					}
				}
				nestedCur++;
			}
			itCur++;
		}
		//{
		//	if (pObj->getChildren()) {
		//		auto size = pObj->getChildren()->size();
		//		for (decltype(size) i = 0; i < size; ++i) {
		//			//auto curChild
		//			handlerCollisions(*pObj->getChildren()->get(i));
		//		}
		//	}
		//}
	}
	void handlerTree(IScreen& screen) {
		screen.clear();
		handlerTreeInner(screen, tree);
		/*screen.combineBuffers();*/
		handlerCollisions();
		screen.draw();
		//auto state = GetKeyState('a');
		/*array<uint8_t, 256> keys;*/
		//if (!GetKeyboardState(keys.data())) {
		//	cerr << "Error read keyboard input";
		//	exit(-1);
		//}
		//cout << "\n[Full input]: ";
		//int res = 0;
		//for (auto ch : keys) {
		//	res += ch;
		//	cout << static_cast<int>(ch) << ' ';
		//}
		//auto state = OemKeyScan('a');
		//cout << endl << "========" << res << "==========";
		// 
		//auto state = GetKeyState(VkKeyScanA('a'));
		//cout << endl << "========" << state << "==========";
		}
public:
	static Game* game;
	shared_ptr<Root> tree;
	bool bStop = false;
	Game(IScreen& screen, Input& input): input{input} {
		Application::hideCursor();
		Application::setScreen(&screen);
		initRandom();
		game = this;
		tickHandler = [this, &screen](Event& event) {
			//cout << "[Tick] " << event.data.toString() << endl;
			this->input.process();
			handlerTree(screen);
			return true;
		};
		addEventHandler(EventType::tick, tickHandler);
		tree = factory.createObject();
	}
	void initRandom() {
		srand(static_cast<unsigned int>(time(nullptr)));
	}
	shared_ptr<Timer> setTimer(int ms, function<void()> callback) {
		auto timer = make_shared<Timer>(ms, callback);
		add(timer);
		return timer;
	}
	void run() {
		mainLoop();
	}
	void addEventHandler(EventType eventType, EventCallback& callback) {
		eventsHandlers[eventType].push_back(callback);
	}
	void addEvent(Event& event) {
		events.push(event);
	}
	void add(shared_ptr<IObject> obj) {
		tree->getChildren()->add(obj);
	}
	void remove(shared_ptr<IObject> obj) override {
		tree->getChildren()->remove(obj);
	}
};

}