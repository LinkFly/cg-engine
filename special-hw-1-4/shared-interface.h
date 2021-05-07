#pragma once

#include "TransformMatrix.h"

struct IViewParent {
	virtual TranslationMatrix& getMatrix() = 0;
	virtual Coords applyAllMatrixes(Coords coords) = 0;
};