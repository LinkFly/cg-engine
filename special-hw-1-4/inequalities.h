#pragma once

#include "helpers/math-functions.h"

#include <cstdint>
#include <cmath>
#include <array>

using namespace std;

struct PointInequality;
struct LineInequality;
struct TriangleInequality;
struct RectangleInequality;

struct IInequality {
	virtual bool isCollide(IInequality& inequality) = 0;
	virtual bool isCollide(PointInequality& inequality) = 0;
	virtual bool isCollide(LineInequality& inequality) = 0;
	virtual bool isCollide(TriangleInequality& inequality) = 0;
	virtual bool isCollide(RectangleInequality& inequality) = 0;
};

struct Inequality : public IInequality {
	template<class TThisType, class TInequalityDescendant>
	bool isCollideFor(IInequality* pInequalityDescendant, bool& bCheckResult);
	template<class TThisType>
	bool isCollideForThis(IInequality& inequality);
	template<class TDescendant>
	bool isCollideByOther(TDescendant& inequality);

	bool isCollide(IInequality& inequality) override;
	bool isCollide(PointInequality& inequality) override;
	bool isCollide(LineInequality& inequality) override;
	bool isCollide(TriangleInequality& inequality) override;
	bool isCollide(RectangleInequality& inequality) override;
};

struct PointInequality : public Inequality {
	float x, y;
	PointInequality(const Coords& coords);
	void init(const Coords& coords);
	bool isCollide(IInequality& inequality) override;
	bool isCollide(PointInequality& inequality) override;
};

struct LineInequality: public Inequality {
	Coords pt1{}, pt2{};
	int minX, maxX;
	float a, b, c;
	LineInequality() = default;

	LineInequality(Coords pt1, Coords pt2);
	void init(Coords pt1, Coords pt2);
	bool isOnStraight(Coords pt) const;
	bool isOnLine(Coords pt) const;
	/*bool isPointInXrange(float x, float minX, float maxX) const;*/
	bool getStraightsCross(const LineInequality& other, Coords& cross);
	bool getCross(const LineInequality& other, Coords& cross);
	bool getCross(const LineInequality& other);

	template<class T_ConcreteInequality>
	T_ConcreteInequality* getConcreteInequality(IInequality& inequality);

	/*bool isCollide(IInequality& inequality) override;*/
	/*bool isCollide(PointInequality& inequality) override;*/
	bool isCollide(LineInequality& inequality) override;
};

struct TriangleInequality: public Inequality {
	Coords a{}, b{}, c{};
	LineInequality ab{}, bc{}, ca{};
	TriangleInequality() = default;
	TriangleInequality(Coords a, Coords b, Coords c);
	void init(Coords a, Coords b, Coords c);
	bool isVertexesOnLine(const LineInequality& line, const TriangleInequality& other);

	bool isCollide(IInequality& inequality) override;
	bool isCollide(PointInequality& inequality) override;
	bool isCollide(LineInequality& inequality) override;
	bool isCollide(TriangleInequality& other) override;
};

struct RectangleInequality: public Inequality {
	TriangleInequality triangle1, triangle2;
	RectangleInequality(const TriangleInequality& triangle1, const TriangleInequality& triangle2);
	void init(const TriangleInequality& triangle1, const TriangleInequality& triangle2);

	bool isCollide(IInequality& inequality) override;
	bool isCollide(PointInequality& inequality) override;
	bool isCollide(LineInequality& inequality) override;
	bool isCollide(TriangleInequality& inequality) override;
	bool isCollide(RectangleInequality& other) override;
};
