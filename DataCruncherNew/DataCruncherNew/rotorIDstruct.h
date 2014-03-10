#ifndef __ROTORIDSTRUCT__H__
#define __ROTORIDSTRUCT__H__
#include <vector>
#include <unordered_map>
#include "array2D.h"

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

//fill temp cycle array with "excitedBy" coords.
inline void fillTempCycle(std::pair<int,int>* tempCycleArray, const int rotorLengthLimit, array2D<std::pair<int,int> > &excitedBy, const int& iInit, const int& jInit)
{
    tempCycleArray[0] = excitedBy(iInit, jInit);
    
    for (int k=1; k<rotorLengthLimit; ++k)
    {
        tempCycleArray[k]=excitedBy(tempCycleArray[k-1].first,tempCycleArray[k-1].second);
    }
   
    return;
}

//function(tempRotorIdFrequency, tempRotorId)
inline void calcMaxFreqId(std::unordered_map<int, int>& tempRotorIdFrequency, int& tempRotorId, int& maxFreq)
{
    maxFreq=0;//reset frequency
    for(std::unordered_map<int, int>::iterator it = tempRotorIdFrequency.begin(); it != tempRotorIdFrequency.end(); ++it)
    {
        if(it->second > maxFreq && it->first != 0)
        {
            maxFreq = it->second;
            tempRotorId = it->first;
        }
    }
    return;
}


#endif //__ROTORIDSTRUCT__H__
