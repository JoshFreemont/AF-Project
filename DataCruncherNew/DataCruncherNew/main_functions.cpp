//
//  excited_scan_algorithms.cpp
//  sdltest
//
//  Created by josh freemont on 17/11/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//

#include "main_functions.h"
#include <ctime>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iostream>


void pacemaker(array2D<int> &state_update, std::vector<int> &all_excited_coords, const int& RP, const int& GRIDSIZE, array2D<std::pair<int,int>> &excitedBy, int& exCells)
{
    for(int j=0; j<GRIDSIZE; ++j)
        if(state_update(0,j)==0)
        {
            state_update(0,j)=RP;
            excitedBy(0,j)=std::make_pair (0,j);
            all_excited_coords.push_back(0);//push back i coord in same coordinate definition as the state array.
            all_excited_coords.push_back(j);//push back j coord as above
			exCells++;
        }
    return;
}

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}

std::vector<int> getBirthDataVect(std::vector<rotorIDstruct>& rotorIdData)
{
    std::vector<int> dataVector (rotorIdData.size());
    int index = 0;
    for(auto it = rotorIdData.begin(); it != rotorIdData.end(); it++)
    {
        dataVector[index] = it->birthframe;
        index++;
    }
    return dataVector;
}

std::vector<int> getDeathDataVect(std::vector<rotorIDstruct>& rotorIdData, const int MAXFRAME)
{
    std::vector<int> dataVector (rotorIdData.size());
    int index = 0;
    for(auto it = rotorIdData.begin(); it != rotorIdData.end(); it++)
    {
        //if death is enforced by end of experiment then continue.
        if( it->lifetime + it->birthframe == MAXFRAME+1) continue;
        else dataVector[index] = it->birthframe + it->lifetime;
        index++;
    }
    
    //correct for deaths which are enforced by the end of experiment
    return dataVector;
}

std::vector<std::vector<int> > buildHist3D (std::vector<histogram> histogramData)
{
    //initialize counters
    int maxXValue = 0;
    int vectorSize = 0;
    
    //iterate through histogram vector to find max x value
    for (auto it = histogramData.begin(); it != histogramData.end(); ++it)
    {
        int xValue = (int)it->returnFreq().size();
        if(xValue > maxXValue) maxXValue = xValue;
        vectorSize++;
    }
    
    //by default fill hist3D with zero vectors.
    std::vector<std::vector<int> > hist3D(vectorSize, std::vector<int> (maxXValue, 0));
    
    //generate 3d vector
    int iIndex = 0;
    for (auto i = histogramData.begin(); i != histogramData.end(); ++i)
    {
        std::vector<int> tempVector = i->returnFreq();
        for(int j = 0; j < (int)tempVector.size(); j++)
        {
            hist3D[iIndex][j] = tempVector[j];
        }
        iIndex++;
    }
    return hist3D;
}
