#include "inequalities.h"
#include "helpers/math-functions.h"

bool isCollide(TriangleInequality& other)
{
	return false;
}

template<class TThisType, class TInequalityDescendant>
bool Inequality::isCollideFor(IInequality* pInequalityDescendant, bool& bCheckResult)
{
	auto pConcreteIneq = dynamic_cast<TInequalityDescendant*>(pInequalityDescendant);
	if (pConcreteIneq) {
		bCheckResult = static_cast<TThisType>(this)->isCollide(*pConcreteIneq);
		return true;
	}
	else return false;
}

bool Inequality::isCollide(IInequality& inequality) {
	return isCollideByOther(inequality);
}

bool Inequality::isCollide(PointInequality& inequality) {
	return isCollideByOther(inequality);
}
bool Inequality::isCollide(LineInequality& inequality) {
	return isCollideByOther(inequality);
}

bool Inequality::isCollide(TriangleInequality& inequality) {
	return isCollideByOther(inequality);
}

bool Inequality::isCollide(RectangleInequality& inequality) {
	return isCollideByOther(inequality);
}

template<class TThisType>
bool Inequality::isCollideForThis(IInequality& inequality) {
	auto pIneq = &inequality;
	bool res;
	if (isCollideFor<TThisType, PointInequality>(pIneq, res)) return res;
	if (isCollideFor<TThisType, LineInequality>(pIneq, res)) return res;
	if (isCollideFor<TThisType, TriangleInequality>(pIneq, res)) return res;
	if (isCollideFor<TThisType, RectangleInequality>(pIneq, res)) return res;
}

template<class TDescendant>
bool Inequality::isCollideByOther(TDescendant& inequality) {
	/*return inequality.isCollide(*this);*/
	return false;
}

PointInequality::PointInequality(const Coords& coords) {
	init(coords);
}

void PointInequality::init(const Coords& coords) {
	x = coords.x;
	y = coords.y;
}

bool PointInequality::isCollide(IInequality& inequality) {
	return isCollideForThis<decltype(this)>(inequality);
}

bool PointInequality::isCollide(PointInequality& inequality) {
	return isEqual(x, inequality.x) && isEqual(y, inequality.y);
}

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
LineInequality::LineInequality(Coords pt1, Coords pt2) {
	init(pt1, pt2);
}
void LineInequality::init(Coords pt1, Coords pt2) {
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
	//arrangeMinMax<int>(minX, maxX);
}
bool LineInequality::isOnStraight(Coords pt) const {
	return isPointOnStraight(pt, a, b, c);
}
bool LineInequality::isOnLine(Coords pt) const {
	if (pt == pt1 || pt == pt2) return true;
	if (!isOnStraight(pt)) return false;
	return isValueInRange(pt.x, minX, maxX);
}
//bool LineInequality::isPointInXrange(float x, float minX, float maxX) const {
//	return x >= minX && x <= maxX;
//}
bool LineInequality::getStraightsCross(const LineInequality& other, Coords& cross) {
	return isStraightsCross(a, b, c, other.a, other.b, other.c, cross.x, cross.y);
}

bool LineInequality::getCross(const LineInequality& other, Coords& cross) {
	return getCrossPoint(pt1, pt2, other.pt1, other.pt2,
		a, b, c, other.a, other.b, other.c, cross);
}

bool LineInequality::getCross(const LineInequality& other) {
	Coords tmp;
	return getCross(other, tmp);
}

template<class T_ConcreteInequality>
T_ConcreteInequality* LineInequality::getConcreteInequality(IInequality& inequality) {
	return dynamic_cast<T_ConcreteInequality*>(&inequality);
}

//bool LineInequality::isCollide(IInequality& inequality)
//{
//	return isCollideForThis<decltype(this)>(inequality);
//}

//bool LineInequality::isCollide(PointInequality& inequality) {
//	return false;
//}
bool LineInequality::isCollide(LineInequality& inequality) {
	return getCross(inequality);
	/*return false;*/
}

TriangleInequality::TriangleInequality(Coords a, Coords b, Coords c) {
	init(a, b, c);
}

void TriangleInequality::init(Coords a, Coords b, Coords c) {
	this->a = a;
	this->b = b;
	this->c = c;
	this->ab = { a, b };
	this->bc = { b, c };
	this->ca = { c, a };
}

bool TriangleInequality::isVertexesOnLine(const LineInequality& line, const TriangleInequality& other) {
	return line.isOnLine(other.a) || line.isOnLine(other.b) || line.isOnLine(other.c);
}

bool TriangleInequality::isCollide(IInequality& inequality)
{
	return isCollideForThis<decltype(this)>(inequality);
}

bool TriangleInequality::isCollide(PointInequality& inequality) {
	//TODO Implement it
	return false;
}
bool TriangleInequality::isCollide(LineInequality& inequality) {
	return ab.isCollide(inequality) || bc.isCollide(inequality) || ca.isCollide(inequality);
}

bool TriangleInequality::isCollide(TriangleInequality& other) {
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

RectangleInequality::RectangleInequality(const TriangleInequality& triangle1, const TriangleInequality& triangle2) {
	init(triangle1, triangle2);
}
void RectangleInequality::init(const TriangleInequality& triangle1, const TriangleInequality& triangle2) {
	this->triangle1 = triangle1;
	this->triangle2 = triangle2;
}

bool RectangleInequality::isCollide(IInequality& inequality)
{
	return isCollideForThis<decltype(this)>(inequality);
}

bool RectangleInequality::isCollide(PointInequality& inequality) {
	return false;
}
bool RectangleInequality::isCollide(LineInequality& inequality) {
	return triangle1.isCollide(inequality) && triangle2.isCollide(inequality);
}
bool RectangleInequality::isCollide(TriangleInequality& inequality) {
	return triangle1.isCollide(inequality) && triangle2.isCollide(inequality);
}
bool RectangleInequality::isCollide(RectangleInequality& other) {
	return
		triangle1.isCollide(other.triangle1) || triangle1.isCollide(other.triangle2) ||
		triangle2.isCollide(other.triangle1) || triangle1.isCollide(other.triangle2);
}
