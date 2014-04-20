#ifndef __ROTORIDSTRUCT__H__
#define __ROTORIDSTRUCT__H__
#include <vector>
#include "array2D.h"

struct rotorIDstruct
{
	int lifetime;
	int birthframe;
	int birthX, birthY;
	int deathX, deathY;
	std::vector<int> length;
    int totalVerts;
    int totalDefects;
	rotorIDstruct(int frame,int currentlength, int x, int y, int defectInit, int vertInit):
	lifetime(1), birthframe(frame), birthX(x), birthY(y),
	deathX(x), deathY(y), totalVerts(defectInit), totalDefects(vertInit)
	{
		length.reserve(50);
		length.push_back(currentlength);
	}
};

#endif //__ROTORIDSTRUCT__H__
