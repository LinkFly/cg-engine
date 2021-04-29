#pragma once

#include "Shapes.h"

#include <vector>
#include <memory>
#include <map>
#include <string>
#include <windows.h>
//#include <sysinfoapi.h>
#include <functional>
#include <iostream>
#include <cmath>

using namespace std;
using ShapeCollection = shapes::ShapeCollection;
using Point = shapes::Point;
using IShape = shapes::IShape;
using Line = shapes::Line;


struct IObject;

struct IObjectCollection {
	virtual size_t add(shared_ptr<IObject> obj) = 0;
	virtual void remove(shared_ptr<IObject> obj) = 0;
	virtual shared_ptr<IObject> get(size_t index) = 0;
	virtual size_t size() = 0;
};

struct IObject {
	virtual size_t getId() = 0;
	virtual shared_ptr<IObject> getParent() = 0;
	virtual void setParent(shared_ptr<IObject> parent) = 0;
	virtual shared_ptr<IObjectCollection> getChildren() = 0;
	virtual void setChildren(shared_ptr<IObjectCollection> position) = 0;
};

struct IView {
	virtual Coords getPosition() = 0;
	virtual void setPosition(Coords position) = 0;
	virtual shared_ptr<ShapeCollection> getShapes() = 0;
	virtual void setShapes(shared_ptr<ShapeCollection> pShapes) = 0;
	virtual bool isCollide(IView& other) = 0;
	virtual void message(IView* from, string message) = 0;
};

struct CreateObjectParams {

};

// TODO make singleton
struct GlobalFactoryComponent {
	static map<size_t, IView*> allViewObjects;
	static size_t genId() {
		static size_t nextId = 0;
		return nextId++;
	}
};

class ObjectCollection;

class Object : public IObject {
	shared_ptr<IObject> parent;
	shared_ptr<IObjectCollection> children;
  public:
	size_t id;

	Object(): 
		children{static_pointer_cast<IObjectCollection>(make_shared<ObjectCollection>())},
		id{ GlobalFactoryComponent::genId() }
	{

	}

	~Object() {
		
	}

	size_t getId() override {
		return id;
	}
	shared_ptr<IObject> getParent() override {
		return parent;
	}
	void setParent(shared_ptr<IObject> parent) override {
		this->parent = parent;
	}
	shared_ptr<IObjectCollection> getChildren() override {
		return children;
	}
	void setChildren(shared_ptr<IObjectCollection> children) override {
		this->children = children;
	}
	// Virtual methods
	virtual void TickHandler(unsigned long long deltaTime) {
		
	}
};

class Root : public Object {
  public:
	Root(): Object() {}
};

//class IViewObject : public IObject, public IView {
//
//};
class ViewObject : public Object, public IView {
	Coords position;
	shared_ptr<shapes::ShapeCollection> pShapes;
	uint8_t direction = 0;
  public:
	function<void(unsigned long long)> fnTickHandler = [](unsigned long long){};
	void Tick() {
		if (!lastTick) lastTick = GetTickCount64();
		unsigned long long newTick = GetTickCount64();
		unsigned long long deltaTime = newTick - lastTick;
		lastTick = newTick;
		TickHandler(deltaTime);
		fnTickHandler(deltaTime);
	}
	float speed = 0;
	
	unsigned long long lastTick = 0;
	void setDirection(uint8_t newVal) { 
		direction = newVal;
	}
	uint8_t getDirection() { return direction; }
	ViewObject(): pShapes{make_shared<shapes::ShapeCollection>()}, position{0, 0} {
		GlobalFactoryComponent::allViewObjects[id] = this;
	}
	~ViewObject() {
		GlobalFactoryComponent::allViewObjects.erase(id);
	}
	Coords getPosition() override {
		return position;
	}
	void setPosition(Coords position) override {
		this->position = position;
		auto shapes = getShapes();
		for (auto& shape : *shapes) {
			shape->setPosition(position);
			/*auto& matrix = shape->getMatrix();
			matrix[2] = position.x;
			matrix[5] = position.y;*/

		}
	}
	void move(Screen& screen, float deltaX, float deltaY) {
		Coords pos = getPosition();
		//pos.x += deltaX;// + 0.01f;// + 1;
		//cout << endl << "pos.x: " << pos.x;
		//pos.y += deltaY;
		auto correctCoord = [](float val) {
			if (abs(val) > 0.01 && abs(val) < 1)
				return signbit(val) ? -1.0f : 1.0f;
			return val;
		};
		pos.x += correctCoord(deltaX);
		pos.y += correctCoord(deltaY);
		//pos.x < 1 && (pos.x = 1.0);
		//pos.y < 1 && (pos.y = 1.0);
		/*if (pos.x < 0) pos.x = 0;
		if (pos.x > screen.width) pos.x = screen.width - 1;*/
		/*if (pos.y < 0) pos.y = 0;
		if (pos.y > screen.height) pos.y = screen.height - 1;*/
		/*auto shapes = getShapes();
		for (auto& shape : *shapes) {
			shape->clear(screen, shape->getMatrix());
		}*/
		setPosition(pos);
		render(screen);
		screen.draw();

	}
	shared_ptr<shapes::ShapeCollection> getShapes() override {
		return pShapes;
	}
	void setShapes(shared_ptr<shapes::ShapeCollection> pShapes) override {
		this->pShapes = pShapes;
	}

	void render(IScreen& screen) {
		auto shapes = getShapes();
		for (auto& shape : *shapes) {
			shape->render(screen);
			shape->render(*screen.addGetBuffer());
		}
	}

	bool isCollide(IView& other) override {
		auto shapes = this->getShapes();
		auto otherShapes = other.getShapes();
		for (auto& shape : *shapes) {
			for (auto& otherShape : *otherShapes) {
				//if (shape->)
				auto inequality = otherShape->getInequality();
				if (shape->getInequality()->isCollide(*inequality))
					return true;
			}
		}
		return false;
	}

	void message(IView* object, string message) override {

	}
};

class ObjectCollection : public IObjectCollection {
	map<size_t, shared_ptr<IObject>> objects;
  public:
	size_t add(shared_ptr<IObject> obj) override {
		size_t id = obj->getId();
		objects[id] = obj;
		return id;
	}
	void remove(shared_ptr<IObject> obj) override {
		objects.erase(obj->getId());
	}
	shared_ptr<IObject> get(size_t index) override {
		size_t curIdx = 0;
		for (auto it = objects.begin(); it != objects.end(); ++it) {
			if (curIdx == index)
				return it->second;
			else ++curIdx;
		}
		return nullptr;
		/*auto it = objects.find(index);
		if (it != objects.end()) {
			return it->second;
		} else return nullptr;*/
	}
	size_t size() override {
		return objects.size();
	}
};

class RectangleObject : public ViewObject {
  public:
	using Point = shapes::Point;
	RectangleObject(Point a, Point b, Point c, Point d) {
		shared_ptr<shapes::IShape> pRect = make_shared<shapes::Rectangle>(a, b, c, d);
		shared_ptr<shapes::ShapeCollection> pShapes = make_shared<shapes::ShapeCollection>();
		pShapes->push_back(pRect);
		//auto myObj = make_shared<ViewObject>();
		/*myObj->setShapes(pShapes);*/
		setShapes(pShapes);
		//return myObj;
	}

	RectangleObject() : RectangleObject(Point{}, Point{}, Point{}, Point{}) {

	}

	//void TickHandler(int tick) override {
	//	
	//}
};

template<class T = Object>
struct IFactory {
	virtual shared_ptr<T> createObject() = 0;
	virtual shared_ptr<T> createObject(CreateObjectParams& params) = 0;
};

template<class T = Object>
class Factory : public IFactory<T> {
	GlobalFactoryComponent globalFactory;
  public:
	shared_ptr<T> createObject() override {
		auto pObj = make_shared<T>();
		//pObj->id = globalFactory.genId();
		return pObj;
	}
	virtual shared_ptr<T> createObject(CreateObjectParams& params) {
		return createObject();
	}
};

class Builder {
  public:
	static shared_ptr<ViewObject> createLineObject(Point a, Point b) {
		shared_ptr<IShape> newLine = make_shared<Line>(a, b);
		shared_ptr<ShapeCollection> pShapes = make_shared<ShapeCollection>();
		pShapes->push_back(newLine);
		auto myObj = make_shared<ViewObject>();
		myObj->setShapes(pShapes);
		return myObj;
	}

	//template<class T>
	//static shared_ptr<T> createViewObject(Point a, Point b) {
	//	
	//	shared_ptr<IShape> newLine = make_shared<Line>(a, b);
	//	shared_ptr<ShapeCollection> pShapes = make_shared<ShapeCollection>();
	//	pShapes->push_back(newLine);
	//	auto myObj = make_shared<ViewObject>();
	//	myObj->setShapes(pShapes);
	//	return myObj;
	//}
};