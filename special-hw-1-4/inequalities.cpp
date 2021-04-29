#include "inequalities.h"

//bool operator==(const Coords& coords1, const Coords& coords2)
//{
//	return false;
//}

bool operator==(const Coords& coords1, const Coords& coords2)
{
	return static_cast<int16_t>(coords1.x) == static_cast<int16_t>(coords2.x)
			&& static_cast<int16_t>(coords1.y) == static_cast<int16_t>(coords2.y);
}
