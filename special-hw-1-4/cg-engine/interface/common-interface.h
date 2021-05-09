#pragma once

#include "../helpers/math.h"

namespace cgEngine {

struct IViewParent {
	virtual TransformMatrix& getMatrix() = 0;
	virtual Coords applyAllMatrixes(Coords coords) = 0;
};

}