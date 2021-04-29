#pragma once

#include "Screen.h"
#include "inequalities.h"

#include <cmath>
#include <initializer_list>
#include <memory>
#include <array>

namespace shapes {

using namespace std;

struct TranslationMatrix : array<float, 3 * 3> {
	TranslationMatrix() {
		float ar[] = {
			1, 0, 0,
			0, 1, 0,
			0, 0, 1
		};
		for (size_t i = 0; i < sizeof(ar) / sizeof(ar[0]); ++i)
			(*this)[i] = ar[i];
	}
};

struct IShape {
	virtual void render(IScreen& screen) = 0;
	/*virtual void render(IScreen& screen, TranslationMatrix& matrix) = 0;*/
	virtual TranslationMatrix& getMatrix() = 0;
	virtual void clear(IScreen& screen, TranslationMatrix& matrix) = 0;
	virtual shared_ptr<Inequality> getInequality() = 0;
	virtual void correctInequality() = 0;
	virtual void setMatrix(const TranslationMatrix& matrix) = 0;
	virtual void setPosition(Coords coords) = 0;
	virtual Coords getPosition() = 0;
};

struct ShapeCollection: public vector<shared_ptr<IShape>> {};

class Shape : public IShape {
public:
	//shared_ptr<Inequality> inequality;
	bool color = true;
	TranslationMatrix matrix;
	TranslationMatrix& getMatrix() override {
		return matrix;
	}
	void setMatrix(const TranslationMatrix& matrix) override {
		this->matrix = matrix;
	}
	Coords applyMatrix(int16_t x, int16_t y) {
		return applyMatrix(x, y, matrix);
	}
	Coords applyMatrix(Coords coords) {
		return applyMatrix(coords.x, coords.y, matrix);
	}
	Coords applyMatrix(Coords coords, TranslationMatrix& matrix) {
		return applyMatrix(coords.x, coords.y, matrix);
	}
	Coords applyMatrix(int16_t x, int16_t y, TranslationMatrix& matrix) {
		Coords res;
		res.x = x + matrix[2];
		res.y = y + matrix[5];
		return res;
	}
	//void render(IScreen& screen, TranslationMatrix& matrix) override {
	//	//render(screen, matrix);
	//}
	/*void render(IScreen& screen) override {
		render(screen, matrix);
	}*/

	void clear(IScreen& screen, TranslationMatrix& matrix) override {
		bool oldColor = color;
		color = false;
		//render(screen, matrix);
		render(screen);
		color = oldColor;
	}
	//virtual shared_ptr<Inequality> getInequality() override {}
	//shared_ptr<Inequality> getInequality() override {
	//	return inequality;
	//}
	Coords getPosition() override {
		auto& matrix = getMatrix();
		return Coords{ matrix[2], matrix[5] };
	}
	void setMatrixPosition(Coords coords) {
		auto& matrix = getMatrix();

		matrix[2] = coords.x;
		matrix[5] = coords.y;
	}
};

struct Point : public Shape {
	//int16_t x, y;
	shared_ptr<Inequality> inequality;
public:
	Point() = default;
	Point(float x, float y, bool color = true) : inequality{ make_shared<PointInequality>(Coords{x, y}) } {
		this->color = color;
		setPosition({x, y});
	}
	Point(Coords coords): Point{coords.x, coords.y, true} {}
	//void render(IScreen& screen, TranslationMatrix& matrix) override 
	void render(IScreen& screen) override {
		Coords coords = getPosition();
		screen.set(coords.x, coords.y, color);
	}
	operator Coords() {
		return getPosition();
	}
	shared_ptr<Inequality> getInequality() override {
		return inequality;
	}
	void correctInequality() override {
		/*Coords coords = applyMatrix(x, y, matrix);
		static_pointer_cast<PointInequality>(inequality)->init(coords);*/
	}
	void setPosition(Coords coords) override {
		setMatrixPosition(coords);
		/*correctInequality();*/
		Coords newCoords = getPosition();
		//static_pointer_cast<PointInequality>(inequality)->init(newCoords);
	}
	//Coord applyMatrix() {
	//	return applyMatrix()
	//}
};

class Line : public Shape {
	Point a, b;
	shared_ptr<Inequality> inequality;
public:
	Line() = default;
	Line(Point a, Point b, bool color = true) : a{ a }, b{ b }, inequality{make_shared<LineInequality>(a, b)} {
		this->color = color;
	}
	Line(Coords a, Coords b, bool color = true) : a{ Point{a} }, b{ Point{b} }, 
			inequality{ make_shared<LineInequality>(Point{a}, Point{b}) }
	{
		this->color = color;
	}
	void correctInequality() override {
		/*Coords pt1 = applyMatrix(a.x, a.y);
		Coords pt2 = applyMatrix(b.x, b.y);
		static_pointer_cast<LineInequality>(inequality)->init(pt1, pt2);*/
	}
	void setPosition(Coords coords) override {
		setMatrixPosition(coords);
		/*correctInequality();*/
		/*Coords pt1 = applyMatrix(a.getPosition());
		Coords pt2 = applyMatrix(b.getPosition());*/
		//static_pointer_cast<LineInequality>(inequality)->init(pt1, pt2);
		
		/*a.setPosition(coords);
		b.setPosition(coords);*/
		
		/*a.applyMatrix*/
	}
	/*void render(IScreen& screen, TranslationMatrix& matrix) override {*/
	void render(IScreen& screen) override {
		Coords coords = getPosition();
		Coords aPos = a.getPosition();
		Coords bPos = b.getPosition();
		aPos = applyMatrix(a.getPosition(), matrix);
		bPos = applyMatrix(b.getPosition(), matrix);
		float width = abs(abs(bPos.x) - abs(aPos.x));
		float height = abs(abs(bPos.y) - abs(aPos.y));
		float maxLen = width > height ? width : height;
		float xDiff = width / maxLen;
		float yDiff = height / maxLen;
		//float xLast = bPos.x, yLast = bPos.y;
		auto nearEqual = [](float f1, float f2) {
			return abs(abs(f2) - abs(f1)) <= 0.1;
		};
		if (aPos.x > bPos.x) {
			xDiff = -xDiff;
		}
		if (aPos.y > bPos.y) {
			yDiff = -yDiff;
		}
		for (float xCur = aPos.x, yCur = aPos.y; !nearEqual(xCur, bPos.x) || !nearEqual(yCur, bPos.y); xCur += xDiff, yCur += yDiff) {
			int16_t x = static_cast<int16_t>(xCur);// || 1;
			int16_t y = static_cast<int16_t>(yCur);// || 1;
			screen.set(x, y, color);
		}
		//Coords a = this->a.getPosition();
		//Coords b = this->b.getPosition();
		//int16_t absX = abs(b.x - a.x), absY = abs(b.y - a.y);
		//int16_t width = absX ? absX : 1;
		//int16_t height = absY ? absY : 1;
		//double maxLen = width > height ? width : height;
		//double xDiff = width == 1 ? 0 : width / maxLen;
		//double yDiff = height == 1 ? 0 : height / maxLen;
		//// TODO evaluating values with signs
		//double nearX = a.x, nearY = a.y;
		////float fx = b.x, fy = b.y;
		//int16_t x, y;
		//do {
		//	Coords coords = this->applyMatrix(nearX, nearY, matrix);
		//	x = static_cast<int16_t>(coords.x);
		//	y = static_cast<int16_t>(coords.y);
		//	if (!(coords.x < 0 || coords.y < 0 || coords.x >= screen.getWidth() || coords.y >= screen.getHeight())) {
		//		screen.set(coords.x, coords.y, color);
		//	}
		//	if (xDiff == 0 && yDiff == 0) break;
		//	nearX += xDiff;
		//	nearY += yDiff;
		//} while ((nearX < b.x || nearY < b.y)/* && !(nearX > screen.getWidth() || nearY > screen.getHeight())*/);
	}
	shared_ptr<Inequality> getInequality() override {
		return inequality;
	}
};

class Rectangle : public Shape {
	Coords a, b, c, d;
	Line h1, h2, v1, v2;
	//vector<TriangleInequality> triangles;
	shared_ptr<Inequality> inequality;
public:
	Rectangle(Point a, Point b, Point c, Point d, bool color = true) {
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
	}
	/*void render(IScreen& screen, TranslationMatrix& matrix) override {*/
	void render(IScreen& screen) override {
		for (const Line& line : { h1, h2, v1, v2 }) {
			const_cast<Line&>(line).render(screen);
		}
	}
	shared_ptr<Inequality> getInequality() override {
		return inequality;
	}

	//void correctPositionByMatrix() {
	//	setPosition(applyMatrix(h1.getPosition()));
	//}
	//void correctInequality() override {

	//}

	void setPosition(Coords coords) override {
		Coords oldPos = getPosition();
		setMatrixPosition(coords);
		/*correctInequality();*/
		Coords ptA = applyMatrix(a.x, a.y);
		Coords ptB = applyMatrix(b.x, b.y);
		Coords ptC = applyMatrix(c.x, c.y);
		Coords ptD = applyMatrix(d.x, d.y);
		/*static_pointer_cast<RectangleInequality>(inequality)->init(
			TriangleInequality{ ptA, ptB, ptC }, TriangleInequality{ ptA, ptD, ptC }
		);*/
		h1 = Line{ ptA, ptB, color };
		h2 = Line{ ptD, ptC, color };
		v1 = Line{ ptB, ptC, color };
		v2 = Line{ ptA, ptD, color };
		//Coords deltaPos = Coords{ coords.x - oldPos.x, coords.y - oldPos.y };
		//auto movePos = [&deltaPos](Coords pos) {
		//	return Coords{ pos.x + deltaPos.x, pos.y + deltaPos.y };
		//};
		//auto moveLine = [&movePos, &coords](Line& line) {
		//	//line.setPosition(movePos(line.getPosition()));
		//	line.setPosition(coords);
		//};
		//moveLine(h1);
		//moveLine(h2);
		//moveLine(v1);
		//moveLine(v2);
	}

	void correctInequality() override {
		/*Coords coords = applyMatrix(x, y, matrix);
		static_pointer_cast<PointInequality>(inequality)->init(coords);*/
	}
};

} // namespace shapes