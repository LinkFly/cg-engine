#include "math.h"
#include "errors-handler.h"

#include <vector>
#include <cstdint>
#include <array>

namespace cgEngine {

using namespace std;

bool operator==(const Coords& coords1, const Coords& coords2)
{
	return static_cast<int16_t>(coords1.x) == static_cast<int16_t>(coords2.x)
		&& static_cast<int16_t>(coords1.y) == static_cast<int16_t>(coords2.y);
}

bool nearZero(float val) {
	static float customEpsilon = 0.01;
	if (val >= 0) {
		return val < customEpsilon;
	}
	else return val > customEpsilon;
}
bool isEqual(float x, float y) {
	auto diff = x > y ? x - y : y - x;
	return nearZero(diff);
}

bool isValueInRange(float x, float minX, float maxX) {
	return x >= minX && x <= maxX;
}

template<class T>
void arrangeMinMax(T& val1, T& val2) {
	if (val1 > val2) {
		auto tmp = val1;
		val1 = val2;
		val2 = tmp;
	}
}

bool isStraightsCross(float a1, float b1, float c1, float a2, float b2, float c2, float& x, float& y) {
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
	static nzMatrix a1_b1_a2_b2 = { 1, 1, 1, 1 };
	static nzMatrix a1_b1_a2 = { 1, 1, 1, 0 };
	static nzMatrix a1_b1_b2 = { 1, 1, 0, 1 };

	static nzMatrix a1_a2_b2 = { 1, 0, 1, 1 };
	static nzMatrix a1_b2 = { 1, 0, 0, 1 };

	static nzMatrix b1_a2_b2 = { 0, 1, 1, 1 };
	static nzMatrix b1_a2 = { 0, 1, 1, 0 };

	nzMatrix ab_nz = {
		a1 != 0, b1 != 0,
		a2 != 0, b2 != 0
	};

	/*float x, y;*/

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
	x = (-c1 - b1 * y) / a1;
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
	y = (-c1 - a1 * x) / b1;
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
	x = (a1 * b1 * c2 - a1 * b2 * c2) / b2;
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
	x = (b2 * c1 - b1 * c2) / (b1 * a2);
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
	return true;
}

bool isPointInRange(Coords pt, Coords pt1, Coords pt2) {
	float minX = pt1.x;
	float maxX = pt2.x;
	arrangeMinMax(minX, maxX);
	float minY = pt1.y;
	float maxY = pt2.y;
	arrangeMinMax(minY, maxY);
	return pt.x >= minX && pt.x <= maxX && pt.y >= minY && pt.y <= maxY;
}

bool getCrossPoint(
	const Coords& pt1, const Coords& pt2, const Coords& otherPt1, const Coords& otherPt2,
	float a1, float b1, float c1, float a2, float b2, float c2, Coords& cross)
{
	Coords straightsCross;
	bool bStraightCross = isStraightsCross(a1, b1, c1, a2, b2, c2, straightsCross.x, straightsCross.y);
	if (!bStraightCross) return false;

	//// Min, Max
	//float minX = pt1.x;
	//float maxX = pt2.x;
	//arrangeMinMax(minX, maxX);
	//float oMinX = otherPt1.x;
	//float oMaxX = otherPt2.x;
	//arrangeMinMax(oMinX, oMaxX);

	/*if (isValueInRange(straightsCross.x, minX, maxX) && isValueInRange(straightsCross.x, oMinX, oMaxX)) {*/
	if (isPointInRange(straightsCross, pt1, pt2) && isPointInRange(straightsCross, otherPt1, otherPt2)) {
		cross.x = straightsCross.x;
		cross.y = straightsCross.y;
		return true;
	}
	return false;
}
bool isPointOnStraight(Coords pt, float a, float b, float c) {
	return isEqual(a * pt.x + b * pt.y + c, .0f);
}

bool isPointOnLine(Coords pt, Coords pt1, Coords pt2, float a, float b, float c) {
	if (pt == pt1 || pt == pt2) return true;
	if (!isPointOnStraight(pt, a, b, c)) return false;
	return isPointInRange(pt, pt1, pt2);
	//float minX = pt1.x, maxX = pt2.x;
	//arrangeMinMax(minX, maxX);
	//return isValueInRange(pt.x, minX, maxX);
}

float getDelta(float coord1, float coord2) {
	return abs(abs(coord1) - abs(coord2));
}
float getVectorLength(Coords pt1, Coords pt2) {
	auto deltaX = getDelta(pt2.x, pt1.x);
	auto deltaY = getDelta(pt2.y, pt1.y);
	return sqrt(pow(deltaX, 2) + pow(deltaY, 2));
}

float Coords::getLength() {
	return getVectorLength({ 0, 0 }, { x, y });
}

//template<class TCoordsCollection>
//void minMaxPointsByX(const TCoordsCollection& aCoords, Coords& ptMinX, Coords& ptMaxX) {
//
//}
//
//template<class TCoordsCollection>
//void minMaxPointsByY(const TCoordsCollection& aCoords, Coords& ptMinY, Coords& ptMaxY) {
//
//}

//template<class TCoordsCollection>
//array<Coords, 4> getWrapRect(const TCoordsCollection& aCoords, float& width, float& height)
//{
//
//}
//
//template<class TCoordsCollection>
//array<Coords, 4> getWrapRect(const TCoordsCollection& aCoords)
//{
//	float width, height;
//	return getWrapRect(aCoords, width, height);
//}

///////////////////////////////////////////////////
//template<class TCoordsCollection>
//array<Coords, 4> getWrapRect(const TCoordsCollection& aCoords, float& width, float& height) {
//	Coords minPtX, maxPtX, minPtY, maxPtY;
//	minMaxPointsByX(aCoords, minPtX, maxPtX);
//	minMaxPointsByY(aCoords, minPtY, maxPtY);
//	array<Coords, 4> res;
//	res[0] = { minPtX.x, minPtY.y };
//	res[1] = { minPtX.x, maxPtY.y };
//	res[2] = { maxPtX.x, maxPtY.y };
//	res[0] = { maxPtX.x, minPtY.y };
//	width = getDelta(maxPtX.x, minPtX.x);
//	height = getDelta(maxPtY.y, minPtY.y);
//	return res;
//}
//
//template<class TCoordsCollection>
//array<Coords, 4> getWrapRect(const TCoordsCollection& aCoords) {
//	float width, height;
//	return getWrapRect(aCoords, width, height);
//}

}