#pragma once

#include "../Screen.h"

//deprecated
#include "../inequalities.h"

#include "../Collision.h"
#include "../TransformMatrix.h"
#include "../shared-interface.h"
#include "../helpers/errors-handler.h"

#include <cmath>
#include <initializer_list>
#include <memory>
#include <array>

namespace shapes {

using namespace std;

struct IShape {
	virtual IViewParent* getParent() = 0;
	virtual void setParent(IViewParent* parent) = 0;
	virtual void render(IScreen& screen) = 0;
	virtual TranslationMatrix& getMatrix() {
		static TranslationMatrix matrix{};
		return matrix;
	}
	virtual void clear(IScreen& screen, TranslationMatrix& matrix) = 0;


	// Deprecated
	virtual shared_ptr<IInequality> getInequality() = 0;
	virtual void correctInequality() = 0;


	virtual void setMatrix(const TranslationMatrix& matrix) {}
	virtual void setPosition(Coords coords) = 0;
	virtual Coords getPosition() = 0;

	virtual void correctCollisionDetector() {}
	virtual bool isCollide(IShape* other) = 0;

	virtual void applyMatrix(const TranslationMatrix& matrix) = 0;

	virtual float getWidth() = 0;
	virtual float getHeight() = 0;
	virtual array<Coords, 4> getEnclosingRect() = 0;
};

class Shape;

struct ShapeCollection: public vector<shared_ptr<IShape>> {
	array<Coords, 4> getEnclosingRect(const vector<Coords>& aCoords, float& width, float& height) {
		return getWrapRect(aCoords, width, height);
	}
	array<Coords, 4> getEnclosingRect(const vector<array<Coords, 4>>& aEncRects, float& width, float& height) {
		vector<Coords> allPoints;
		for (auto& encRect : aEncRects) {
			for (auto& Coords : encRect) {
				allPoints.push_back(Coords);
			}
		}
		return getEnclosingRect(allPoints, width, height);
	}
	array<Coords, 4> getEnclosingRect(float& width, float& height) {
		if (empty()) showError("Bad shapes count - equal zero");
		vector<array<Coords, 4>> aEncRects;
		for (size_t i = 0; i < size(); ++i) {
			auto pShape = (*this)[i];
			aEncRects.push_back(pShape->getEnclosingRect());
		}
		return getEnclosingRect(aEncRects, width, height);
	}
	
};


class Shape : public IShape {
	/*shared_ptr<IViewObject> parent;*/
public:
	TranslationMatrix position;
	IViewParent* parent;
	Shape(): position{TranslationMatrix::getUnitMatrix()} {}
	void setParent(IViewParent* parent) override {
		this->parent = parent;
	}
	IViewParent* getParent() override {
		return parent;
	}
	bool color = true;
	//TranslationMatrix matrix;
	//TranslationMatrix& getMatrix() override {
	//	return matrix;
	//}

	//void setMatrix(const TranslationMatrix& matrix) override {
	//	this->matrix = matrix;
	//}
	//Coords applyMatrix(int16_t x, int16_t y) {
	//	return applyMatrix(x, y, matrix);
	//}
	//Coords applyMatrix(Coords coords) {
	//	return applyMatrix(coords.x, coords.y, matrix);
	//}
	//Coords applyMatrix(Coords coords, TranslationMatrix& matrix) {
	//	return applyMatrix(coords.x, coords.y, matrix);
	//}
	//
	//Coords applyMatrix(int16_t x, int16_t y, TranslationMatrix& matrix) {
	//	Coords res;
	//	res.x = x + matrix[2];
	//	res.y = y + matrix[5];
	//	return res;
	//}
	void clear(IScreen& screen, TranslationMatrix& matrix) override {
		bool oldColor = color;
		color = false;
		//render(screen, matrix);
		render(screen);
		color = oldColor;
	}
	void setMatrixPosition(Coords coords) {
		auto& matrix = getMatrix();

		matrix[2] = coords.x;
		matrix[5] = coords.y;
	}

	Coords applyFullMatrixes(Coords coords) {
		auto res = position.apply(coords);
		return getParent()->applyAllMatrixes(res);
	}
};

struct Point : public Shape {
	shared_ptr<IInequality> inequality;
	PointCollision ptCollision{0, 0};
public:
	Coords coords{};
	Point() = default;
	Point(float x, float y, bool color = true): Shape{}, ptCollision{x, y}
			//inequality{ make_shared<PointInequality>(Coords{x, y}) } 
	{
		this->color = color;
		/*setPosition({x, y});*/
		coords = {x, y};
	}
	Point(Coords coords): Point{coords.x, coords.y, true} {}

	template<class TCollision>
	bool isCollide(IShape& shape, TCollision& collision) {
		return ptCollision.isCollide(collision);
	}

	//void render(IScreen& screen, TranslationMatrix& matrix) override 
	void render(IScreen& screen) override {
		Coords coords = getPosition();
		coords = position.apply(coords);
		coords = getParent()->applyAllMatrixes(coords);
		screen.set(coords.x, coords.y, color);
	}
	operator Coords() {
		return getPosition();
	}
	shared_ptr<IInequality> getInequality() override {
		return inequality;
	}
	void correctInequality() override {
		/*Coords coords = applyMatrix(x, y, matrix);
		static_pointer_cast<PointInequality>(inequality)->init(coords);*/
	}
	void correctCollisionDetector() override {
		/*ptCollision.applyMatrix(matrix);*/
	}
	//void setPosition(Coords coords) override {
	//	setMatrixPosition(coords);
	//	/*correctInequality();*/
	//	correctCollisionDetector();
	//	//Coords newCoords = getPosition();
	//	//static_pointer_cast<PointInequality>(inequality)->init(newCoords);
	//}
	//Coord applyMatrix() {
	//	return applyMatrix()
	//}
	bool isCollide(IShape* other) override; 

	void applyMatrix(const TranslationMatrix& matrix) override {
		coords = matrix.apply(coords);
		ptCollision.applyMatrix(matrix);
	}
	void setPosition(Coords coords) override {
		//applyMatrix(TranslationMatrix::getPositionMatrix(coords));
		this->coords = coords;
		auto parent = getParent();
		position.setMovePosition(coords);
		Coords handledCoords = position.apply(coords);
		ptCollision.init(parent->applyAllMatrixes(handledCoords));
	}
	Coords getPosition() override {
		return coords;
	}

	float getWidth() override { return 1; }
	float getHeight() override { return 1; }
	array<Coords, 4> getEnclosingRect() override { 
		return array<Coords, 4>{{{1, 1}, {1, 1}, {1, 1}, {1, 1}}};
	}
};

class Line : public Shape {

	// deprecated
	shared_ptr<IInequality> inequality;
	
  public:
  
	Point a, b;
	LineCollision lineCollision;
	Line() = default;
	Line(Point a, Point b, bool color = true) : 
			Shape(),
			a{ a }, b{ b }, 
			inequality{make_shared<LineInequality>(a, b)},
			lineCollision{a.getPosition(), b.getPosition()}
	{
		this->color = color;
		/*setPosition({0, 0});*/
		
	}
	Line(Coords a, Coords b, bool color = true) : a{ Point{a} }, b{ Point{b} }, 
			inequality{ make_shared<LineInequality>(Point{a}, Point{b}) },
			lineCollision{ a, b }
	{
		this->color = color;
		/*setPosition({ 0, 0 });*/
	}

	//deprecated
	void correctInequality() override {
		/*Coords pt1 = applyMatrix(a.x, a.y);
		Coords pt2 = applyMatrix(b.x, b.y);
		static_pointer_cast<LineInequality>(inequality)->init(pt1, pt2);*/
	}

	Coords getAGlobalPosition() {
		return applyFullMatrixes(a.getPosition());
	}
	Coords getBGlobalPosition() {
		return applyFullMatrixes(b.getPosition());
	}

	void render(IScreen& screen) override {
		auto aPos = getAGlobalPosition();
		auto bPos = getBGlobalPosition();
		float x1 = round(aPos.x);
		float y1 = round(aPos.y);
		float x2 = round(bPos.x);
		float y2 = round(bPos.y);

		float width = abs(x2 - x1);
		float height = abs(y2 - y1);
		float maxLen = max(width, height);

		if (maxLen == 0) {
			screen.set(x1, y1, color);
			return;
		}

		float diffX = (x2 - x1) / maxLen;
		float diffY = (y2 - y1) / maxLen;

		float x = x1;
		float y = y1;
		maxLen++;
		while (maxLen--) {
			screen.set(x, y, color);
			x += diffX;
			y += diffY;
		}
	}
	//void render(IScreen& screen) override {
	//	//Coords coords = getPosition();
	//	//Coords aPos = a.getPosition();
	//	//Coords bPos = b.getPosition();
	//	//aPos = position.apply(aPos);
	//	//bPos = position.apply(bPos);
	//	//aPos = getParent()->applyAllMatrixes(aPos);
	//	//bPos = getParent()->applyAllMatrixes(bPos);
	//	auto aPos = getAGlobalPosition();
	//	auto bPos = getBGlobalPosition();
	//	float width = abs(abs(bPos.x) - abs(aPos.x));
	//	float height = abs(abs(bPos.y) - abs(aPos.y));
	//	float maxLen = width > height ? width : height;
	//	float xDiff = width / maxLen;
	//	float yDiff = height / maxLen;
	//	//float xLast = bPos.x, yLast = bPos.y;
	//	auto nearEqual = [](float f1, float f2) {
	//		return abs(abs(f2) - abs(f1)) <= 0.1;
	//	};
	//	if (aPos.x > bPos.x) {
	//		xDiff = -xDiff;
	//	}
	//	if (aPos.y > bPos.y) {
	//		yDiff = -yDiff;
	//	}

	//	//screen.set(x, y, color);
	//	for (float xCur = aPos.x, yCur = aPos.y; !nearEqual(xCur, bPos.x) || !nearEqual(yCur, bPos.y); xCur += xDiff, yCur += yDiff) {
	//		int16_t x = static_cast<int16_t>(xCur);// || 1;
	//		int16_t y = static_cast<int16_t>(yCur);// || 1;
	//		//int16_t x = static_cast<int16_t>(round(xCur));// || 1;
	//		//int16_t y = static_cast<int16_t>(round(yCur));// || 1;
	//		if (x < 0 || y < 0) return;
	//		screen.set(x, y, color);
	//	}
	//}
	shared_ptr<IInequality> getInequality() override {
		return inequality;
	}

	bool isCollide(IShape* other) override;

	void applyMatrix(const TranslationMatrix& matrix) override {
		a.applyMatrix(matrix);
		b.applyMatrix(matrix);
		lineCollision.applyMatrix(matrix);
	}
	void setPosition(Coords coords) override {
		position.setMovePosition(coords);
		auto newA = position.apply(a.getPosition());
		newA = getParent()->applyAllMatrixes(newA);
		auto newB = position.apply(b.getPosition());
		newB = getParent()->applyAllMatrixes(newB);
		lineCollision.init(newA, newB);
		//applyMatrix(TranslationMatrix::getPositionMatrix(coords));

	}
	Coords getPosition() override {
		return position.getPosition();
	}

	float getLength() {
		auto newA = applyFullMatrixes(a.getPosition());
		auto newB = applyFullMatrixes(b.getPosition());
		return getVectorLength(newA, newB);
	}

	float getWidth() override { 
		auto newA = applyFullMatrixes(a.getPosition());
		auto newB = applyFullMatrixes(b.getPosition());
		return getDelta(newA.x, newB.x) + 1; }
	float getHeight() override { 
		auto newA = applyFullMatrixes(a.getPosition());
		auto newB = applyFullMatrixes(b.getPosition());
		return getDelta(newA.y, newB.y) + 1;
	}
	array<Coords, 4> getEnclosingRect() override {
		auto newA = applyFullMatrixes(a.getPosition());
		auto newB = applyFullMatrixes(b.getPosition());
		return getWrapRect(array<Coords, 2>{newA, newB});
	}
};

class Rectangle : public Shape {
	Coords a, b, c, d;
	Line h1, h2, v1, v2;
	//vector<TriangleInequality> triangles;
	//deprecated
	shared_ptr<Inequality> inequality;
public:
	PolylineCollision polylineCollision;

	Rectangle(IViewParent* parent, Point a, Point b, Point c, Point d, bool color = true): 
			Shape{},
			polylineCollision{{a.getPosition(), b.getPosition(), c.getPosition(), d.getPosition(), a.getPosition()}} {
		setParent(parent);
		init(a, b, c, d, color);
	}
	void init(Point a, Point b, Point c, Point d, bool color = true)/*: a{a}, b{b}, c{c}, d{d}*/ 
			/*h1{a, b, color}, h2{d, c, color}, v1{b, c, color}, v2{a, d, color},*/
		//inequality{make_shared<RectangleInequality>(TriangleInequality{a, b, c}, TriangleInequality{a, d, c})}
	{
		//color = this->color;
		this->a = a;
		this->b = b;
		this->c = c;
		this->d = d;
		this->color = color;
		h1 = Line{ a, b, color };
		h2 = Line{ d, c, color };
		v1 = Line{ b, c, color };
		v2 = Line{ a, d, color };
		//triangles.push_back(TriangleInequality{a, b, c});
		//triangles.push_back(TriangleInequality{a, d, c});
		inequality = make_shared<RectangleInequality>(TriangleInequality{a, b, c}, TriangleInequality{a, d, c});
		/*setPosition({ 0, 0 });*/
		auto pParent = getParent();
		h1.position = position;
		h2.position = position;
		v1.position = position;
		v2.position = position;
		h1.setParent(pParent);
		h2.setParent(pParent);
		v1.setParent(pParent);
		v2.setParent(pParent);
	}
	void render(IScreen& screen) override {
		for (const Line& line : { h1, h2, v1, v2 }) {
			const_cast<Line&>(line).render(screen);
		}
	}
	shared_ptr<IInequality> getInequality() override {
		return inequality;
	}

	void correctInequality() override {
	}

	bool isCollide(IShape* other) override;

	void applyMatrix(const TranslationMatrix& matrix) override {
		a = matrix.apply(a);
		b = matrix.apply(b);
		c = matrix.apply(c);
		d = matrix.apply(d);
		h1.applyMatrix(matrix);
		h2.applyMatrix(matrix);
		v1.applyMatrix(matrix);
		v2.applyMatrix(matrix);
		polylineCollision.applyMatrix(matrix);
	}
	void setPosition(Coords coords) override {
		//applyMatrix(TranslationMatrix::getPositionMatrix(coords));

		position.setMovePosition(coords);
		h1.setPosition(coords);
		h2.setPosition(coords);
		v1.setPosition(coords);
		v2.setPosition(coords);
		Coords na = getParent()->applyAllMatrixes(position.apply(a));
		Coords nb = getParent()->applyAllMatrixes(position.apply(b));
		Coords nc = getParent()->applyAllMatrixes(position.apply(c));
		Coords nd = getParent()->applyAllMatrixes(position.apply(d));
		polylineCollision.init({na, nb, nc, nd, na});
	}
	Coords getPosition() override {
		return position.getPosition();
	}

	vector<Coords> getAllLinePoints() {
		return {
			h1.getAGlobalPosition(), h1.getBGlobalPosition(), h2.getAGlobalPosition(), h2.getBGlobalPosition(),
			v1.getAGlobalPosition(), v1.getBGlobalPosition(), v2.getAGlobalPosition(), h2.getBGlobalPosition()
			};
	}
	/*void minMaxPointsByX(Coords ptA, Coords ptB, Coords ptC, Coords ptD) {*/
	array<Coords, 4> getEnclosingRect() override {
		return getWrapRect(getAllLinePoints());
	}
	float getWidth() override {
		float width, height;
		getWrapRect(getEnclosingRect(), width, height);
		return width;
	}
	float getHeight() override { 
		float width, height;
		getWrapRect(getEnclosingRect(), width, height);
		return height;
	}
};

} // namespace shapes