#pragma once

#include "math-functions.h"
#include "errors-handler.h"
#include "TransformMatrix.h"

#include <vector>
#include <string>
#include <algorithm>
#include <array>
#include <functional>

using namespace std;

struct IEquation {

};

struct PointEquation : public IEquation {
	float x, y;
	PointEquation() = default;
	PointEquation(float x, float y) {
		init(x, y);
	}
	PointEquation(Coords pt) {
		init(pt);
	}
	void init(float x, float y) {
		this->x = x;
		this->y = y;
	}
	void init(Coords coords) {
		this->x = coords.x;
		this->y = coords.y;
	}
	Coords getCoords() const {
		return Coords{x, y};
	}
};

struct LineEquation : public IEquation {
	Coords pt1{}, pt2{};
	float a{}, b{}, c{};
	LineEquation() = default;
	LineEquation(Coords pt1, Coords pt2) {
		init(pt1, pt2);
	}
	LineEquation(const PointEquation& pt1, const PointEquation& pt2) {
		init(pt1, pt2);
	}
	void init(const Coords& pt1, const Coords& pt2) {
		this->pt1 = pt1;
		this->pt2 = pt2;
		a = pt1.y - pt2.y;
		b = pt2.x - pt1.x;
		c = pt1.x * pt2.y - pt2.x * pt1.y;
	}
	void init(const PointEquation& pt1, const PointEquation& pt2) {
		init(pt1.getCoords(), pt2.getCoords());
	}
};

struct TriangleEquation : public IEquation {
	LineEquation ab, bc, ca;
	TriangleEquation() = default;
	TriangleEquation(Coords a, Coords b, Coords c) {
		init(a, b, c);
	}
	void init(Coords a, Coords b, Coords c) {
		ab = {a, b};
		bc = {b, c};
		ca = {c, a};
	}
};

//class Collision;

class PointCollision;
class LineCollision;
class TriangleCollision;
class PolylineCollision;

class BaseCollision {
//  public:
//	//static function<bool(BaseCollision& collision)> isDetectCollision;
//	//template<class TSuccessor>
//	//BaseCollision() {
//	//	isDetectCollision = [this](const BaseCollision& collision) {
//	//		return static_cast<TSuccessor*>(this)->isCollide();
//	//	};
//	//}
//	template<class 
//	template<class T>
//	bool isDetectCollision(const T& collision) {
//		return this.isCollide
//	}
};

template<class TSuccessorCollision>
class Collision: public BaseCollision {
  public:
	bool isCollide(const PointCollision& collision) {
		return static_cast<TSuccessorCollision*>(this)->isCollideImplementation(collision);
	}
	bool isCollide(const LineCollision& collision) {
		return static_cast<TSuccessorCollision*>(this)->isCollideImplementation(collision);
	}
	bool isCollide(const TriangleCollision& collision) {
		return static_cast<TSuccessorCollision*>(this)->isCollideImplementation(collision);
	}
	bool isCollide(const PolylineCollision& collision) {
		return static_cast<TSuccessorCollision*>(this)->isCollideImplementation(collision);
	}
	template<class TThis>
	bool isCollideInternal(const BaseCollision* collision) {
		auto pPoint = dynamic_cast<BaseCollision*>(collision);
		return static_cast<TThis*>(this)->isCollide(collision);
	}
	bool isCollideSelectThis(const BaseCollision& collision) {
		auto pPoint = dynamic_cast<PointCollision*>(this);
		if (pPoint) return pPoint->isCollideInternal<PointCollision>(collision);

		auto pPoint = dynamic_cast<PointCollision*>(this);
		if (pPoint) return pPoint->isCollideInternal<PointCollision>(collision);
		
	}
	void applyMatrix(const TranslationMatrix& matrix) {
		static_cast<TSuccessorCollision*>(this)->applyMatrixImpl(matrix);
	}
};

class PointCollision : public Collision<PointCollision> {
  public:
	PointEquation ptEq;
	PointCollision() = default;
	PointCollision(PointEquation& ptEq): ptEq{ptEq} {
	}

	PointCollision(float a, float b) : ptEq{ PointEquation{a, b} } {
	}

	PointCollision(Coords pt) : ptEq{pt} {
	}

	void applyMatrixImpl(const TranslationMatrix& matrix) {
		ptEq.init(matrix.apply(ptEq.getCoords()));
	}
	void init(Coords coords) { ptEq.init(coords.round()); }

	bool isCollideImplementation(const PointCollision& other);/* {
		return ptEq.x == other.ptEq.x && ptEq.y == other.ptEq.y;
	}*/

	bool isCollideImplementation(const LineCollision& other);
	bool isCollideImplementation(const TriangleCollision& other);
	bool isCollideImplementation(const PolylineCollision& other);

};

class LineCollision : public Collision<LineCollision> {
	bool isPointInLine(Coords pt, const LineEquation& lineEq) {
		Coords pt1 = lineEq.pt1, pt2 = lineEq.pt2;
		float a = lineEq.a, b = lineEq.b, c = lineEq.c;
		return isPointOnLine(pt, pt1, pt2, a, b, c);
	}
public:
	LineEquation lineEq;
	LineCollision() = default;
	LineCollision(LineEquation& lineEq) : lineEq{ lineEq } {	
	}
	LineCollision(Coords a, Coords b): lineEq{LineEquation{a, b}} {}

	void applyMatrixImpl(const TranslationMatrix& matrix) {
		lineEq.init(matrix.apply(lineEq.pt1), matrix.apply(lineEq.pt2));
	}
	void init(Coords a, Coords b) { lineEq.init(a.round(), b.round()); }

	bool isCollideImplementation(const PointCollision& other) {
		Coords pt{ other.ptEq.x, other.ptEq.y};
		return isPointInLine(pt, lineEq);
	}
	bool isCollideImplementation(const LineCollision& other) {
		Coords a1 = lineEq.pt1, b1 = lineEq.pt2;
		auto& oLineEq = other.lineEq;
		Coords a2 = oLineEq.pt1, b2 = oLineEq.pt2;
		if (a1 == a2 || a1 == b2 || b1 == a2 || b1 == b2) {
			return true;
		}
			
		if (isPointInLine(a1, other.lineEq) ||
			isPointInLine(b1, other.lineEq) ||
			isPointInLine(a2, lineEq) ||
			isPointInLine(b2, lineEq)
			) {
			return true;
		}
			
		float _a1 = lineEq.a, _b1 = lineEq.b, _c1 = lineEq.c, 
				_a2 = oLineEq.a, _b2 = oLineEq.b, _c2 = oLineEq.c;
		Coords cross;
		if (getCrossPoint(a1, b1, a2, b2, _a1, _b1, _c1, _a2, _b2, _c2, cross))
			return true;
		return false;
	}

	bool isCollideImplementation(const TriangleCollision& other);
	bool isCollideImplementation(const PolylineCollision& other);
};

class TriangleCollision: public Collision<TriangleCollision> {
	LineCollision ab, bc, ca;
  public:
	TriangleCollision(Coords a, Coords b, Coords c) {
		init(a, b, c);
	}

	void init(Coords a, Coords b, Coords c) {
		ab = {a.round(), b.round() };
		bc = {a.round(), c.round() };
		ca = {c.round(), a.round() };
	}
	void applyMatrixImpl(const TranslationMatrix& matrix) {
		ab.applyMatrixImpl(matrix);
		bc.applyMatrixImpl(matrix);
		ca.applyMatrixImpl(matrix);
	}

	bool isCollideImplementation(const PointCollision& other) {
		return ab.isCollideImplementation(other) 
			|| bc.isCollideImplementation(other) 
			|| ca.isCollideImplementation(other);
	}
	bool isCollideImplementation(const LineCollision& other) {
		return ab.isCollideImplementation(other) 
			|| bc.isCollideImplementation(other)
			|| ca.isCollideImplementation(other);
	}
	bool isCollideImplementation(const TriangleCollision& other) {
		return isCollideImplementation(other.ab) 
			|| isCollideImplementation(other.bc)
			|| isCollideImplementation(other.ca);
	}
	bool isCollideImplementation(const PolylineCollision& other);
};

class PolylineCollision : public Collision<PolylineCollision> {
	vector<LineCollision> lines;
  public:
	PolylineCollision() = default;
	PolylineCollision(const vector<Coords>& aCoords) {
		init(aCoords);
	}
	void init(const vector<Coords>& aCoords) {
		if (aCoords.size() < 2) {
			showError("Bad size coordinates array");
		}
		Coords lastPt = aCoords[0];
		lines.clear();
		for(size_t i = 1; i < aCoords.size(); ++i) {
			Coords nextPt = aCoords[i];
			lines.emplace_back(lastPt.round(), nextPt.round());
			lastPt = nextPt;
		}
	}

	void applyMatrixImpl(const TranslationMatrix& matrix) {
		for (auto& line : lines) {
			line.applyMatrixImpl(matrix);
		}
	}

	template<class T>
	bool isCollideForCollisions(const T& other) {
		return any_of(lines.begin(), lines.end(), [&other](LineCollision& line) {
			return line.isCollideImplementation(other);
			});
	}

	bool isCollideImplementation(const PointCollision& other) {
		return isCollideForCollisions(other);
	}
	bool isCollideImplementation(const LineCollision& other) {
		return isCollideForCollisions(other);
	}
	bool isCollideImplementation(const TriangleCollision& other) {
		return isCollideForCollisions(other);
	}
	bool isCollideImplementation(const PolylineCollision& other) {
		auto& otherLines = other.lines;
		return any_of(otherLines.begin(), otherLines.end(), [this](const LineCollision& otherLine) mutable {
			return isCollideImplementation(otherLine);
			});
	}
};

//bool PointCollision::isCollideImplementation(const LineCollision& other) {
//	return const_cast<LineCollision&>(other).isCollideImplementation(*this);
//}
//bool PointCollision::isCollideImplementation(const TriangleCollision& other) {
//	return const_cast<TriangleCollision&>(other).isCollideImplementation(*this);
//}
//bool PointCollision::isCollideImplementation(const PolylineCollision& other) {
//	return const_cast<PolylineCollision&>(other).isCollideImplementation(*this);
//}

//bool LineCollision::isCollideImplementation(const TriangleCollision& other) {
//	return const_cast<TriangleCollision&>(other).isCollideImplementation(*this);
//}
//bool LineCollision::isCollideImplementation(const PolylineCollision& other) {
//	return const_cast<PolylineCollision&>(other).isCollideImplementation(*this);
//}
//
//bool TriangleCollision::isCollideImplementation(const PolylineCollision& other) {
//	return const_cast<PolylineCollision&>(other).isCollideImplementation(*this);
//}

/////////////////////////////////////////
//class CollisionDetector {
//	//T collision;
//  public:
//    template<class T, class U>
//	bool isCollide(T cLeft, U cRight) {
//		return cLeft.isCollide(cRight);
//	}
//	
//};