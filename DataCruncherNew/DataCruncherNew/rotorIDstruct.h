#ifndef __ROTORIDSTRUCT__H__
#define __ROTORIDSTRUCT__H__
#include <vector>

struct rotorIDstruct
{
	int lifetime;
	int birthframe;
	int birthX, birthY;
	int deathX, deathY;
	std::vector<int> length;
	rotorIDstruct(int frame,int currentlength, int x, int y):
	lifetime(1), birthframe(frame), birthX(x), birthY(y),
	deathX(x), deathY(y)
	{
		length.reserve(50);
		length.push_back(currentlength);
	}
};

#endif //__ROTORIDSTRUCT__H__
