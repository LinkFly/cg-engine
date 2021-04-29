#pragma once

#include <cstdint>
#include <cmath>
#include <array>

using namespace std;

struct Coords {
	float x, y;
	friend bool operator==(const Coords& coords1, const Coords& coords2);
};

//bool operator==(const Coords& coords1, const Coords& coords2) {
//	return (coords1.x == coords2.x) && (coords2.y == coords2.y);
//}

struct Inequality {
	virtual bool isCollide(Inequality& inequality) = 0;
	//virtual bool isCollide(Inequality& inequality) = 0;
	//virtual void init(Coords pt1, Coords pt2) = 0;
};

//struct Inequality: public IInequality {
//	//virtual bool isCollide(Inequality& inequality) = 0;
//
//};

struct TriangleInequalityBase : public Inequality {};
struct RectangleInequalityBase : public Inequality {};

struct PointInequality : Inequality {
	float x, y;
	PointInequality(const Coords& coords) {
		init(coords);
	}

	void init(const Coords& coords) {
		x = coords.x;
		y = coords.y;
	}
	bool isCollide(Inequality& inequality) override {
		// Not implemented
		return false;
	}
};

struct LineInequality: Inequality {
	LineInequality() = default;
	Coords pt1, pt2;
	int minX, maxX;
	float a, b, c;
/*
y - y1      x - x1
-------- = ---------
y2 - y1    x2 - x1

(y - y1) * (x2 - x1) - ((x - x1)(y2 - y1)) = 0;
y(x2 - x1) - y1(x2 - x1) - (x(y2 - y1) - x1(y2 - y1)) = 0
y(x2 - x1) - y1(x2 - x1) - x*(y2 - y1) + x1(y2 - y1) = 0
y(x2 - x1) - x*(y2 - y1) + x1(y2 - y1) - y1(x2 - x1) = 0
-x*(y2 - y1) + y(x2 - x1) + x1*y2 - x1*y1 - y1*x2 + y1*x1 = 0
x*(y1 - y2) + y(x2 - x1) + x1*y2 - y1*x2 = 0
a = y1 - y2
b = x2 - x1
c = x1*y2 - y1*x2

a = y1 - y2
b = x2 - x1
c = x1*y2 - x2*y1

*/
	LineInequality(Coords pt1, Coords pt2) {
		init(pt1, pt2);
	}
	void init(Coords pt1, Coords pt2) {
		this->pt1 = pt1;
		this->pt2 = pt2;
		a = pt1.y - pt2.y;
		b = pt2.x - pt1.x;
		c = pt1.x * pt2.y - pt2.x * pt1.y;
		//a = pt2.y - pt1.y;
		//b = pt1.x - pt2.x;
		//c = pt1.x*pt2.y - pt2.x*pt1.y;
		minX = pt1.x;
		maxX = pt2.x;
		if (minX > maxX) {
			auto tmp = minX;
			minX = maxX;
			maxX = tmp;
		}
	}
	bool isOnStraight(Coords pt) const {
		return a * pt.x + b * pt.y + c == 0;
	}
	bool isOnLine(Coords pt) const {
		if (pt == pt1 || pt == pt2) return true;
		if (!isOnStraight(pt)) return false;
		return isPointInXrange(pt);
	}
	bool isPointInXrange(Coords pt) const {
		return pt.x >= minX && pt.x <= maxX;
	}
	bool getStraightsCross(const LineInequality& other, Coords& cross) {
		float a1 = a;
		float a2 = other.a;
		float b1 = b;
		float b2 = other.b;
		float c1 = c;
		float c2 = other.c;
		/*
		a1*x + b1*y + c1 = 0;
		a2*x + b2*y + c2 = 0;

		a = 0, b = 0 false
		a != 0, b = 0
					a2 = 0, b2 = 0 false
					a2 != 0, b2 = 0 false
					a2 = 0, b2 != 0
					a2 != 0, b2 != 0
		a = 0, b != 0
					a2 = 0, b2 = 0 false
					a2 != 0, b2 = 0
					a2 = 0, b2 != 0 false
					a2 != 0, b2 != 0
		a != 0, b != 0
					a2 = 0, b2 = 0 false
					a2 != 0, b2 = 0
					a2 = 0, b2 != 0
					a2 != 0, b2 != 0 OK
		----------------------------
		a1*b2 - a2*b1 = 0
		a1*b2 = a2*b1
		
		*/
		using nzMatrix = array<bool, 4>;
		static nzMatrix a1_b1_a2_b2 =	{ 1, 1, 1, 1};
		static nzMatrix a1_b1_a2 =		{ 1, 1, 1, 0 };
		static nzMatrix a1_b1_b2 =		{ 1, 1, 0, 1 };

		static nzMatrix a1_a2_b2 =		{ 1, 0, 1, 1 };
		static nzMatrix a1_b2 =			{ 1, 0, 0, 1 };

		static nzMatrix b1_a2_b2 =		{ 0, 1, 1, 1 };
		static nzMatrix b1_a2 =			{ 0, 1, 1, 0};

		nzMatrix ab_nz = {
			a1 != 0, b1 != 0,
			a2 != 0, b2 != 0
		};

		float x, y;

		if (ab_nz == a1_b1_a2_b2) goto a1_b1_a2_b2;
		if (ab_nz == a1_b1_a2) goto a1_b1_a2;
		if (ab_nz == a1_b1_b2) goto a1_b1_b2;

		if (ab_nz == a1_a2_b2) goto a1_a2_b2;
		if (ab_nz == a1_b2) goto a1_b2;

		if (ab_nz == b1_a2_b2) goto b1_a2_b2;
		if (ab_nz == b1_a2) goto b1_a2;
		return false;

		a1_b1_a2_b2:
/*		
		a1*x + b1*y + c1 = 0;
		a2*x + b2*y + c2 = 0;

		x = (-c1 - b1*y) / a1

		a2 * ((-c1 - b1*y) / a1) + b2*y + c2 = 0
		(a2 * (-c1) - a2 * b1*y + a1 * b2*y) / a1 = -c2
		(y * (a1*b2 - a2*b1) - a2*c1) / a1 = -c2
		(y * (a1*b2 - a2*b1)) / a1 = -c2 + (a2*c1) / a1
		y * (a1*b2 - a2*b1) = -c2*a1 + (a2*c1)
		y = (a2*c1 - a1*c2) / (a1*b2 - a2*b1)

		x = (-c1 - b1*y) / a1
		x = (-c1 - (b1*((a2*c1 - a1*c2) / (a1*b2 - a2*b1))) / a1
		x = (-c1 - (a2*b1*c1 - a1*b1*c2) / (a1*b2 - a2*b1)) / a1
		x = (-c1(a1*b2 - a2*b1) - (a2*b1*c1 - a1*b1*c2) / (a1*b2 - a2*b1)) / a1
		x = ((-a1*b2*c1 + a2*b1*c1 - a2*b1*c1 + a1*b1*c2) / (a1*b2 - a2*b1)) / a1
		x = ((-a1*b2*c1 + a1*b1*c2) / (a1*b2 - a2*b1)) / a1
		x = a1*(a1*b1*c2 - a1*b2*c1) / (a1*b2 - a2*b1)
		x = (a1*a1*b1*c2 - a1*a1*b2*c1) / (a1*b2 - a2*b1)
		------------- bad
		x = (-c1 - b1y) / a1
		x = (b1*y + c1) / -a1
		x = (b1 * ((a2*c1 - a1*c2) / (a1*b2 - a2*b1)) + c1) / -a1
		x = ((a2*b1*c1 - a1*b1*c2) / (a1*b2 - a2*b1) + c1) / -a1
		x = ((a2*b1*c1 - a1*b1*c2) + c1*(a1*b2 - a2*b1)) / (a1*b2 - a2*b1)) / -a1
		x = ((a2*b1*c1 - a1*b1*c2 + a1*b2*c1 - a2*b1*c1) / (a1*b2 - a2*b1)) / -a1
		x = ((-a1*b1*c2 + a1*b2*c1) / (a1*b2 - a2*b1)) / -a1
		x = (a1^2*b1*c2 - a1^2*b2*c1) / (a1*b2 - a2*b1))
		------------------------- bad
*/
		//x = (a1 * a1 * b1 * c2 - a1 * a1 * b2 * c1) / (a1 * b2 - a2 * b1);
		//x = (a1 * a1 * b1 * c2 - a1 * a1 * b2 * c1) / (a1 * b2 - a2 * b1);
		y = (a2 * c1 - a1 * c2) / (a1 * b2 - a2 * b1);
		// x = (-c1 - b1*y) / a1
		x = (-c1 - b1*y) / a1;
		goto mEnd;

		a1_b1_a2:
/*	
		a1*x + b1*y + c1 = 0;
		a2*x + b2*y + c2 = 0;

		a1*x + b1*y = -c1
		a2*x = -c2

		y = (-c1 - a1*(-c2 / a2)) / b1
		x = -c2 / a2
		
		y = (-c1 + a1*c2 / a2) / b1
		y = ((-c1*a2 + a1*c2) / a2) / b1
		y = (a1*b1*c2 - a2*b1*c1) / a2
*/
		x = (-c2) / a2;
		y = (a1 * b1 * c2 - a2 * b1 * c1) / a2;
		goto mEnd;

		a1_b1_b2:
/*
		a1*x + b1*y + c1 = 0;
		a2*x + b2*y + c2 = 0;

		a1*x + b1*y = -c1
		b2*y = -c2

		x = (-c1 - b1*(-c2 / b2)) / a1
		y = -c2 / b2

		x = ((-c1*b2 + b1*c2) / b2) / a1
		x = (a1*b1*c2 - a1*b2*c2) / b2
*/
		x = (a1*b1*c2 - a1*b2*c2) / b2;
		y = -c2 / b2;
		goto mEnd;

		a1_a2_b2:
		/*
		a1*x + b1*y + c1 = 0;
		a2*x + b2*y + c2 = 0;

		a1*x + c1 = 0;
		a2*x + b2*y + c2 = 0;

		x = -c1 / a1
		a2 * (-c1 / a1) + b2*y = -c2
		b2*y - a2*c1 / a1 = -c2
		y = (a2*c1 - c2) / a1*b2
		*/
		x = (-c1) / a1;
		y = (a2 * c1 - c2) / a1 * b2;
		goto mEnd;

		a1_b2:
		/*
* 		a1*x + b1*y + c1 = 0;
		a2*x + b2*y + c2 = 0;

		a1*x + c1 = 0
		b2*y + c2 = 0;
		x = -c1 / a1
		y = -c2 / b2
		*/
		x = (-c1) / a1;
		y = (-c2) / b2;
		goto mEnd;

		b1_a2_b2:
		/*
* 		a1*x + b1*y + c1 = 0;
		a2*x + b2*y + c2 = 0;

		b1*y + c1 = 0
		y = -c1 / b1
		a2*x + b2*y + c2 = 0;
		a2*x + b2*(-c1 / b1) = -c2
		a2*x - b2*c1 / b1 = -c2
		a2*x = -c2 + b2*c1 / b1
		a2*x = (-c2*b1 + b2*c1) / b1
		x = (b2*c1 - b1*c2) / b1*a2
		*/
		x = (b2*c1 - b1*c2) / (b1*a2);
		y = -c1 / b1;
		goto mEnd;

		b1_a2:
		/*
		a1*x + b1*y + c1 = 0;
		a2*x + b2*y + c2 = 0;

		b1*y + c1 = 0
		a2*x + c2 = 0

		y = -c1 / b1
		x = -c1 / a2
		*/
		x = static_cast<float>(-c1) / a2;
		y = static_cast<float>(-c1) / b1;

		mEnd:
		cross.x = x;
		cross.y = y;
		return true;
	}

	bool getCross(const LineInequality& other, Coords& cross) {
		Coords straightsCross;
		bool bStraightCross = getStraightsCross(other, straightsCross);
		if (!bStraightCross) return false;
		if (isPointInXrange(straightsCross) && other.isPointInXrange(straightsCross)) {
			cross.x = straightsCross.x;
			cross.y = straightsCross.y;
			return true;
		}
		return false;
	}

	bool getCross(const LineInequality& other) {
		Coords tmp;
		return getCross(other, tmp);
	}
	 
//  protected:
	template<class T_ConcreteInequality>
	T_ConcreteInequality* getConcreteInequality(Inequality& inequality) {
		return dynamic_cast<T_ConcreteInequality*>(&inequality);
	}

	bool isCollide(Inequality& inequality) override {
		auto pLineIneq = getConcreteInequality<LineInequality>(inequality);
		if (pLineIneq) return isCollide(*pLineIneq);
		auto pTrigIneq = getConcreteInequality<TriangleInequalityBase>(inequality);
		if (pTrigIneq) return isCollide(*pTrigIneq);
		auto pRectIneq = getConcreteInequality<RectangleInequalityBase>(inequality);
		if (pRectIneq) return isCollide(*pRectIneq);
	}
	bool isCollide(LineInequality& inequality) {
		return getCross(inequality);
	}

	bool isCollide(TriangleInequalityBase& inequality) {
		return inequality.isCollide(*this);
	}

	bool isCollide(RectangleInequalityBase& inequality) {
		return inequality.isCollide(*this);
	}
};

struct TriangleInequality: public TriangleInequalityBase {
	Coords a, b, c;
	LineInequality ab, bc, ca;
	TriangleInequality() = default;
	TriangleInequality(Coords a, Coords b, Coords c) {
		init(a, b, c);
	}
	void init(Coords a, Coords b, Coords c) {
		this->a = a;
		this->b = b;
		this->c = c;
		this->ab = {a, b};
		this->bc = {b, c};
		this->ca = {c, a};
	}

	bool isVertexesOnLine(const LineInequality& line, const TriangleInequality& other) {
		return line.isOnLine(other.a) || line.isOnLine(other.b) || line.isOnLine(other.c);
	}

	bool isCollide(Inequality& inequality) override {
		auto pLineIneq = dynamic_cast<LineInequality*>(&inequality);
		if (pLineIneq) return isCollide(*pLineIneq);
		auto pTrigIneq = dynamic_cast<TriangleInequality*>(&inequality);
		if (pTrigIneq) return isCollide(*pTrigIneq);
		auto pRectIneq = dynamic_cast<RectangleInequalityBase*>(&inequality);
		if (pRectIneq) return isCollide(*pRectIneq);

		// Not implemented
		return false;
	}

	bool isCollide(LineInequality& inequality) {
		return ab.isCollide(inequality) || bc.isCollide(inequality) || ca.isCollide(inequality);
	}

	bool isCollide(TriangleInequality& other) {
		//if (a == other.a || a == other.b || a == other.c ||
		//	b == other.a || b == other.b || b == other.c ||
		//	c == other.a || c == other.b || c == other.c)
		//	return true;
		if (isVertexesOnLine(ab, other) || isVertexesOnLine(bc, other) || isVertexesOnLine(ca, other));
			return true;
		return 
			ab.getCross(other.ab) || ab.getCross(other.bc) || ab.getCross(other.ca) ||
			bc.getCross(other.ab) || bc.getCross(other.bc) || bc.getCross(other.ca) ||
			ca.getCross(other.ab) || ca.getCross(other.bc) || ca.getCross(other.ca);
	}

	bool isCollide(RectangleInequalityBase& inequality) {
		return inequality.isCollide(*this);
	}
};

struct RectangleInequality: public RectangleInequalityBase {
	TriangleInequality triangle1, triangle2;
	RectangleInequality(const TriangleInequality& triangle1, const TriangleInequality& triangle2) {
		init(triangle1, triangle2);
	}
	void init(const TriangleInequality& triangle1, const TriangleInequality& triangle2) {
		this->triangle1 = triangle1;
		this->triangle2 = triangle2;
	}
	bool isCollide(Inequality& inequality) override {
		auto pLinePtr = dynamic_cast<LineInequality*>(&inequality);
		if (pLinePtr) return isCollide(*pLinePtr);
		auto pTrigIneq = dynamic_cast<TriangleInequality*>(&inequality);
		if (pTrigIneq) return isCollide(*pTrigIneq);
		auto pRectPtr = dynamic_cast<RectangleInequality*>(&inequality);
		if (pRectPtr) return isCollide(*pRectPtr);

		// Not implemented
		return false;
	}
	bool isCollide(LineInequality& inequality) {
		return triangle1.isCollide(inequality) && triangle2.isCollide(inequality);
	}
	bool isCollide(TriangleInequality& inequality) {
		return triangle1.isCollide(inequality) && triangle2.isCollide(inequality);
	}
	bool isCollide(RectangleInequality& other) {
		return 
			triangle1.isCollide(other.triangle1) || triangle1.isCollide(other.triangle2) ||
			triangle2.isCollide(other.triangle1) || triangle1.isCollide(other.triangle2);
	}
};