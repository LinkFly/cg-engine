#pragma once

#include "math.h"
#include "errors-handler.h"

#include <cmath>
#include <array>

namespace cgEngine {

using namespace std;

struct Coords {
	float x, y;
	Coords() = default;
	Coords(float x, float y): x{x}, y{y} {}
	Coords round() { return {std::round(x), std::round(y) }; }
	float getLength();
	Coords operator*(float factor) {
		return {x * factor, y * factor};
	}
	Coords applyNewBasis(Coords newBasisI, Coords newBasisJ) {
	/*
	x*i^ +^ y*j^
	(x*ix, x*iy) +^ (y*jx, y*jy)
	= (x*ix + y*jx, x*iy + y*jy
	*/
		return {x * newBasisI.x + y * newBasisJ.x, x * newBasisI.y + y * newBasisJ.y};
	}
	friend bool operator==(const Coords& coords1, const Coords& Coordss2);
};

bool nearZero(float val);
bool isEqual(float x, float y);
bool isValueInRange(float x, float minX, float maxX);
template<class T>
void arrangeMinMax(T& val1, T& val2);
bool isStraightsCross(float a1, float b1, float c1, float a2, float b2, float c2, float& x, float& y);
bool getCrossPoint(
	const Coords& pt1, const Coords& pt2, const Coords& otherPt1, const Coords& otherPt2,
	float a1, float b1, float c1, float a2, float b2, float c2, Coords& cross);
bool isPointOnStraight(Coords pt, float a, float b, float c);
bool isPointOnLine(Coords pt, Coords pt1, Coords pt2, float a, float b, float c);
float getDelta(float coord1, float coord2);
float getVectorLength(Coords pt1, Coords pt2);

template<class TCoordsCollection>
void minMaxPointsByX(const TCoordsCollection& aCoords, Coords& ptMinX, Coords& ptMaxX) {
	if (aCoords.size() < 2) {
		showError("Bad points array");
	}
	ptMinX = aCoords[0];
	ptMaxX = aCoords[0];
	for (auto& coords : aCoords) {
		if (coords.x < ptMinX.x) ptMinX = coords;
		if (coords.x > ptMaxX.x) ptMaxX = coords;
	}
}

template<class TCoordsCollection>
void minMaxPointsByY(const TCoordsCollection& aCoords, Coords& ptMinY, Coords& ptMaxY) {
	if (aCoords.size() < 2) {
		showError("Bad points array");
	}
	ptMinY = aCoords[0];
	ptMaxY = aCoords[0];
	for (auto& coords : aCoords) {
		if (coords.y < ptMinY.y) ptMinY = coords;
		if (coords.y > ptMaxY.y) ptMaxY = coords;
	}
}

template<class TCoordsCollection>
array<Coords, 4> getWrapRect(const TCoordsCollection& aCoords, float& width, float& height) {
	Coords minPtX, maxPtX, minPtY, maxPtY;
	minMaxPointsByX(aCoords, minPtX, maxPtX);
	minMaxPointsByY(aCoords, minPtY, maxPtY);
	array<Coords, 4> res;
	res[0] = { minPtX.x, minPtY.y };
	res[1] = { minPtX.x, maxPtY.y };
	res[2] = { maxPtX.x, maxPtY.y };
	res[3] = { maxPtX.x, minPtY.y };
	width = getDelta(maxPtX.x, minPtX.x);
	height = getDelta(maxPtY.y, minPtY.y);
	//width || (width = 1);
	//height || (height = 1);
	++width;
	++height;
	return res;
	/*return array<Coords, 4>{};*/
}

template<class TCoordsCollection>
array<Coords, 4> getWrapRect(const TCoordsCollection& aCoords) {
	float width, height;
	return getWrapRect(aCoords, width, height);
	/*return array<Coords, 4>{};*/
}

struct TransformMatrix : array<float, 3 * 3> {
	template<class T>
	static void fillMatrix(TransformMatrix& matrix, T ar[3 * 3]) {
		for (size_t i = 0; i < sizeof(T[3 * 3]) / sizeof(ar[0]); ++i)
			matrix[i] = ar[i];
	}
	static TransformMatrix getMoveMatrix(Coords coords) {
		float ar[] = {
		1, 0, coords.x,
		0, 1, coords.y,
		0, 0, 1
		};
		return TransformMatrix{ ar };
	}
	static TransformMatrix getPositionMatrix(Coords coords) {
		float ar[] = {
		1, 0, 0,
		0, 1, 0,
		coords.x, coords.y, 1
		};
		return TransformMatrix{ ar };
	}
	static TransformMatrix getUnitMatrix() {
		static float ar[] = {
			1, 0, 0,
			0, 1, 0,
			0, 0, 1
		};
		return TransformMatrix{ ar };
	}
	TransformMatrix() {
		*this = getUnitMatrix();
	}
	TransformMatrix(float ar[3 * 3]) {
		fillMatrix(*this, ar);
	}
	TransformMatrix(int ar[3 * 3]) {
		fillMatrix(*this, ar);
	}

	TransformMatrix(float deltaX, float deltaY) {
		*this = getMoveMatrix({ deltaX, deltaY });
	}

	TransformMatrix(Coords coords) {
		*this = getMoveMatrix(coords);
	}

	/*
		[a c e
		 b d f
		 g h 1]
	*/
	float a() const { return (*this)[0]; }
	float c() const { return (*this)[1]; }
	float e() const { return (*this)[2]; }
	void e(float x) { (*this)[2] = x; }
	float b() const { return (*this)[3]; }
	float d() const { return (*this)[4]; }
	float f() const { return (*this)[5]; }
	void f(float y) { (*this)[5] = y; }
	float g() const { return (*this)[6]; }
	float h() const { return (*this)[7]; }
	Coords getMovePosition() const {
		return { e(), f() };
	}
	void getMovePosition(float& x, float& y) const {
		x = e();
		y = f();
	}
	void setMovePosition(Coords coords) {
		e(coords.x);
		f(coords.y);
	}
	Coords getPosition() const {
		return { g(), h() };
	}
	void getPosition(float& x, float& y) const {
		x = g();
		y = h();
	}
	Coords apply(Coords pt) const {
		float globX = g() ? g() : pt.x;
		float globY = h() ? h() : pt.y;
		float x = a() * globX + c() * globY + e();
		float y = b() * globX + d() * globY + f();
		return { x, y };
	}
};

}