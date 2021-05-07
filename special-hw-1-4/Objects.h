#pragma once

#include "Shapes.h"
#include "Application.h"
#include "utils.h"

#include <vector>
#include <memory>
#include <map>
#include <string>
#include <windows.h>
//#include <sysinfoapi.h>
#include <functional>
#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>

using namespace std;

class IView;

using ShapeCollection = shapes::ShapeCollection;
using Point = shapes::Point;
using IShape = shapes::IShape;
using Line = shapes::Line;
using CollisionHandler = function<void(IView*, string)>;

struct IObject;

struct IObjectCollection {
	virtual size_t add(shared_ptr<IObject> obj) = 0;
	virtual void remove(shared_ptr<IObject> obj) = 0;
	virtual shared_ptr<IObject> get(size_t index) = 0;
	virtual size_t size() = 0;
};

struct ICollectionTree {
	virtual shared_ptr<IObject> getParent() = 0;
	virtual void setParent(shared_ptr<IObject> parent) = 0;
	virtual shared_ptr<IObjectCollection> getChildren() = 0;
	virtual void setChildren(shared_ptr<IObjectCollection> position) = 0;
};

struct IObject {//: public ICollectionTree {
	virtual size_t getId() = 0;
	virtual shared_ptr<IObject> getParent() = 0;
	virtual void setParent(shared_ptr<IObject> parent) = 0;
	virtual shared_ptr<IObjectCollection> getChildren() = 0;
	virtual void setChildren(shared_ptr<IObjectCollection> position) = 0;
	virtual void Tick() = 0;
	virtual void TickHandler(unsigned long long deltaTime) = 0;
};

struct IView {//: public ICollectionTree {
	virtual Coords getPosition() = 0;
	virtual void setPosition(Coords position) = 0;
	virtual shared_ptr<ShapeCollection> getShapes() = 0;
	virtual void setShapes(shared_ptr<ShapeCollection> pShapes) = 0;
	virtual bool isCollide(IView& other) = 0;
	virtual shared_ptr<IShape> getFirstShape() = 0;
	virtual void message(IView* from, string message) = 0;
	virtual Coords getMoveVector() = 0;
	virtual void setMoveVector(Coords moveVector) = 0;
	virtual float getSpeed() = 0;
	virtual void setSpeed(float speed) = 0;
	virtual void applyMoveVector(float deltaTime) = 0;
	virtual unsigned long long getLastTick() = 0;
	virtual void setLastTick(unsigned long long) = 0;
};

struct CreateObjectParams {

};

// TODO make singleton
struct GlobalFactoryComponent {
	static map<size_t, IObject*> allViewObjects;
	static size_t genId() {
		static size_t nextId = 0;
		return nextId++;
	}
};

class ObjectCollection;

class Object : public IObject {
	shared_ptr<IObject> parent;
	shared_ptr<IObjectCollection> children;
protected:
	unsigned long long lastTick = 0;
  public:
	size_t id;
	function<void(unsigned long long)> fnTickHandler;
	Object(): 
		children{static_pointer_cast<IObjectCollection>(make_shared<ObjectCollection>())},
		id{ GlobalFactoryComponent::genId() }
	{
		static function<void(unsigned long long)> emptyFunction = [](unsigned long long) {};
		fnTickHandler = emptyFunction;
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
	void Tick() override {
		if (!lastTick) lastTick = GetTickCount64();
		unsigned long long newTick = GetTickCount64();
		unsigned long long deltaTime = newTick - lastTick;
		lastTick = newTick;
		fnTickHandler(deltaTime);
		TickHandler(deltaTime);
	}
	void TickHandler(unsigned long long deltaTime) override {
		
	}
};

class Root : public Object {
  public:
	Root(): Object() {}
};

//class IViewObject : public IObject, public IView {
//
//};
class ViewObject : public IView, public IViewParent, public Object {
	Coords position;
	shared_ptr<shapes::ShapeCollection> pShapes;
	uint8_t direction = 0;
	TranslationMatrix matrix;
	vector<CollisionHandler*> collisionHandlers;
	Coords moveVector{};
  public:
	Option<float> minX, maxX, minY, maxY;
	float speed = 50;
	TranslationMatrix& getMatrix() override { return matrix; }
	Coords applyAllMatrixes(Coords coords) override {
		auto res = getMatrix().apply(coords);
		auto pObj = getParent();
		auto pViewObj = dynamic_cast<ViewObject*>(pObj.get());
		if (pViewObj) {
			coords = pViewObj->applyAllMatrixes(coords);
		}
		return coords;
	}
	
	unsigned long long lastTick = 0;
	void setDirection(uint8_t newVal) { 
		direction = newVal;
	}
	uint8_t getDirection() { return direction; }
	ViewObject(): 
			pShapes{make_shared<shapes::ShapeCollection>()},
			position{0, 0}, 
			matrix{TranslationMatrix::getUnitMatrix()}
	{
		GlobalFactoryComponent::allViewObjects[id] = this;
	}
	~ViewObject() {
		GlobalFactoryComponent::allViewObjects.erase(id);
	}
	Coords getPosition() override {
		
		return matrix.apply(position);
		/*return position;*/
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
	Coords getNewPosition(float deltaX, float deltaY) {
		Coords pos = getPosition();
		pos.x += deltaX;
		pos.y += deltaY;
		return pos;
	}
	void move(IScreen& screen, float deltaX, float deltaY) {
		/*return;*/
		Coords pos = getPosition();
		//pos.x += deltaX;// + 0.01f;// + 1;
		//cout << endl << "pos.x: " << pos.x;
		//pos.y += deltaY;
		auto correctCoord = [](float val, const Option<float>& minOpt, const Option<float>& maxOpt) {
			if (!minOpt.bExistsVal) {
				if (!maxOpt.bExistsVal) 
					return val;
				else return max(val, maxOpt.getVal());
			}
			else if (!maxOpt.bExistsVal)
				return min(val, minOpt.getVal());
			else return clamp<float>(val, minOpt.getVal(), maxOpt.getVal());
		};
		pos.x += deltaX;
		pos.y += deltaY;
		pos.x = correctCoord(pos.x, minX, maxX);
		pos.y = correctCoord(pos.y, minY, maxY);
		
		setPosition(pos);
		render(screen);
		screen.draw();
	}
	shared_ptr<shapes::ShapeCollection> getShapes() override {
		return pShapes;
	}
	void setShapes(shared_ptr<shapes::ShapeCollection> pShapes) override {
		for (auto&& pShape : *pShapes) {
			auto pViewParent = static_cast<IViewParent*>(this);
			pShape->setParent(pViewParent);
		}
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
				/*auto inequality = otherShape->getInequality();
				if (shape->getInequality()->isCollide(*inequality))
					return true;*/
				if (shape->isCollide(otherShape.get()))
					return true;
			}
		}
		return false;
	}

	void message(IView* object, string message) override {
		//if (object != this) {
			for (auto& handler : collisionHandlers) {
				(*handler)(object, message);
			}
		//}
	}
	void addCollisionHandler(CollisionHandler* callback) {
		collisionHandlers.push_back(callback);
	}

	shared_ptr<IShape> getFirstShape() override {
		auto pShapes = getShapes();
		if (pShapes->empty()) return nullptr;
		return (*pShapes)[0];
	}

	template<class T>
	T* getFirstConcreteShape() {
		shared_ptr<IShape> pShape = getFirstShape();
		auto pConcreteShape = dynamic_cast<T*>(pShape.get());
		if (!pConcreteShape) return nullptr;
		return pConcreteShape;
	}

	Coords getMoveVector() override {
		return moveVector;
	}
	void setMoveVector(Coords moveVector) override {
		this->moveVector = moveVector;
	}

	float getSpeed() override {
		return speed;
	}
	void setSpeed(float speed) override {
		this->speed = speed;
	}
	void applyMoveVector(float deltaTime) override {
		//lastTick
	}
	unsigned long long getLastTick() override { return lastTick; }
	void setLastTick(unsigned long long lastTick) override { this->lastTick = lastTick; }

	array<Coords, 4> getEnclosingRect(float& width, float& height) {
		auto pShapes = getShapes();
		return pShapes->getEnclosingRect(width, height);
	}

	void TickHandler(unsigned long long deltaTime) override {
		// Check near the border
		Coords pos = getPosition();
		float width, height;
		getEnclosingRect(width, height);
		if (pos.x == 0 || pos.x == Application::getScreen()->getWidth() - width
			|| pos.y == 0 || pos.y == Application::getScreen()->getHeight() - height) {
			message(this, Messages::nearTheBorder());
		}
		////////

		if (moveVector.x != 0 || moveVector.y != 0) {
			float len = moveVector.getLength();
			float moveDist = (deltaTime / 1000.0) * speed * len;
			move(*Application::screen, moveVector.x * moveDist, moveVector.y * moveDist);
		}
	}

	void setMinMaxByScreen() {
		minX = 0;
		minY = 0;
		float width, height;
		getEnclosingRect(width, height);
		IScreen* pScreen = Application::getScreen();
		maxX = pScreen->getWidth() - width;
		maxY = pScreen->getHeight() - height;
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
		shared_ptr<shapes::IShape> pRect = make_shared<shapes::Rectangle>(this, a, b, c, d);
		shared_ptr<shapes::ShapeCollection> pShapes = make_shared<shapes::ShapeCollection>();
		pShapes->push_back(pRect);
		//auto myObj = make_shared<ViewObject>();
		/*myObj->setShapes(pShapes);*/
		setShapes(pShapes);
		//return myObj;
	}

	RectangleObject() : RectangleObject(Point{}, Point{}, Point{}, Point{}) {

	}
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