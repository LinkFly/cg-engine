#include "Collision.h"

bool PointCollision::isCollideImplementation(const PointCollision& other) {
	return ptEq.x == other.ptEq.x && ptEq.y == other.ptEq.y;
}

bool PointCollision::isCollideImplementation(const LineCollision& other) {
	return const_cast<LineCollision&>(other).isCollideImplementation(*this);
}
bool PointCollision::isCollideImplementation(const TriangleCollision& other) {
	return const_cast<TriangleCollision&>(other).isCollideImplementation(*this);
}
bool PointCollision::isCollideImplementation(const PolylineCollision& other) {
	return const_cast<PolylineCollision&>(other).isCollideImplementation(*this);
}

bool LineCollision::isCollideImplementation(const TriangleCollision& other) {
	return const_cast<TriangleCollision&>(other).isCollideImplementation(*this);
}
bool LineCollision::isCollideImplementation(const PolylineCollision& other) {
	return const_cast<PolylineCollision&>(other).isCollideImplementation(*this);
}

bool TriangleCollision::isCollideImplementation(const PolylineCollision& other) {
	return const_cast<PolylineCollision&>(other).isCollideImplementation(*this);
}
