#include "Shapes.h"

//bool shapes::Shape::isCollide(IShape* other)
//{
//
//	return false;
//}

namespace cgEngine {
namespace shapes {

	bool Point::isCollide(IShape* other) {
		auto pPoint = dynamic_cast<Point*>(other);
		if (pPoint) return ptCollision.isCollide(pPoint->ptCollision);
		auto pLine = dynamic_cast<Line*>(other);
		if (pLine) return ptCollision.isCollide(pLine->lineCollision);
		auto pRect = dynamic_cast<Rectangle*>(other);
		if (pRect) return ptCollision.isCollide(pRect->polylineCollision);
		return false;
	}

	bool Line::isCollide(IShape* other) {
		auto pPoint = dynamic_cast<Point*>(other);
		if (pPoint) return lineCollision.isCollide(pPoint->ptCollision);
		auto pLine = dynamic_cast<Line*>(other);
		if (pLine) return lineCollision.isCollide(pLine->lineCollision);
		auto pRect = dynamic_cast<Rectangle*>(other);
		if (pRect) return lineCollision.isCollide(pRect->polylineCollision);
		return false;
	}

	bool Rectangle::isCollide(IShape* other) {
		auto pPoint = dynamic_cast<Point*>(other);
		if (pPoint) return polylineCollision.isCollide(pPoint->ptCollision);
		auto pLine = dynamic_cast<Line*>(other);
		if (pLine) return polylineCollision.isCollide(pLine->lineCollision);
		auto pRect = dynamic_cast<Rectangle*>(other);
		if (pRect) return polylineCollision.isCollide(pRect->polylineCollision);
		return false;
	}
}

}