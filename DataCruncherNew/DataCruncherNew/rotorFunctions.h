//
//  rotorFunctions.h
//  DataCruncherNew
//
//  Created by josh freemont on 10/03/2014.
//  Copyright (c) 2014 com.example. All rights reserved.
//

#ifndef __DataCruncherNew__rotorFunctions__
#define __DataCruncherNew__rotorFunctions__

#include <iostream>
#include <cmath>

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
        if(it->second > maxFreq && it->first != -1)
        {
            maxFreq = it->second;
            tempRotorId = it->first;
        }
    }
    return;
}


//function(rotorLengthLimit, RP, tempCycleArray, isCycle, cycleLength)
inline bool returnIsCycleRepeat(const int& rotorLengthLimit, const int& RP, std::pair<int, int>* tempCycleArray, int& cycleLength, int& cycleStart)
{
    cycleLength=0;
    bool isCycle = false;
    for(int tortoise=rotorLengthLimit-1; tortoise>=RP; --tortoise)
    {
        for(int hare=tortoise-RP; hare>=0; hare--)
        {
            if((tempCycleArray[tortoise]==tempCycleArray[hare]) &&
               (tempCycleArray[hare+1]!=tempCycleArray[hare]))
            {
                cycleStart=hare;
                cycleLength=tortoise-hare;
                tortoise=-1;
                isCycle=true;
                break;
            }
        }
    }
    return isCycle;
}


//function(cycleStart, cycleLength, tempCycleArray, avX, avY, rotorHeightCutoff, GRIDSIZE, activeRotorId)
inline void calcAvPos(const int& cycleStart, const int& cycleLength, std::pair<int, int>* tempCycleArray, std::unordered_map<int, int>& tempRotorIdFrequency, double& avX, double& avY, const int& rotorHeightCutoff, const int& GRIDSIZE, array2D<int>& activeRotorId)
{
    for (int m=cycleStart, k=0; k<cycleLength; ++k)
    {
        int i = tempCycleArray[m+k].first, j = tempCycleArray[m+k].second;
        avX += i;
        
        //check if j is far from running average- if so assume j is across boundary.
        if(k==0) avY += j;//first iteration, add j.
        else if((double)j - (avY/(double)k) > (double)rotorHeightCutoff) avY += j-GRIDSIZE;
        else if((double)j - (avY/(double)k) < (double)-rotorHeightCutoff) avY += j+GRIDSIZE;
        else avY += j;
        
        //fill tempRotorIdFrequency map.
        tempRotorIdFrequency[activeRotorId(i,j)]++;
    }
    
    avX /= (double)(cycleLength);
    avY /= (double)(cycleLength);
	avX = floor(avX+0.5);
    avY = (double)(static_cast<int>(floor(avY+GRIDSIZE+0.5))%GRIDSIZE);//enforce correct bounds.
    
    return;
}


#endif /* defined(__DataCruncherNew__rotorFunctions__) */
