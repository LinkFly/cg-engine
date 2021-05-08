#pragma once

#include "math-functions.h"
#include "errors-handler.h"

#include <cmath>
#include <array>

using namespace std;

struct Coords {
	float x, y;
	Coords() = default;
	Coords(float x, float y): x{x}, y{y} {}
	Coords round() { return {std::round(x), std::round(y) }; }
	float getLength();
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
