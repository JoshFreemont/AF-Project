//
//  excited_scan_algorithms.h
//  sdltest
//
//  Created by josh freemont on 17/11/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//

#ifndef __sdltest__main_functions__
#define __sdltest__main_functions__
#include <iostream>
#include <cmath>
#include <vector>
#include "array2D.h"
#include <utility>
#include <string>
#include "rotorIDstruct.h"

//Inline Update Functions.
inline void update_arrays(int &state_update, std::vector<int> &all_excited_coords_cyclic, const int& RP, const int& i_coord, const int& j_coord)
{
    state_update=RP;
    all_excited_coords_cyclic.push_back(i_coord);
    all_excited_coords_cyclic.push_back(j_coord);
    return;
}

inline void updateRotorIdData(rotorIDstruct& rotor, const int& cycleLength, const int& averageX, const int& averageY)
{
    rotor.lifetime++;
    rotor.length.push_back(cycleLength);
    rotor.deathX = averageX;
    rotor.deathY = averageY;
    return;
}

inline void updateRotorCoords(std::vector<int> rotorCoordsNow, int& i, int& j, int& state, int& Id)
{
    rotorCoordsNow.push_back(i);
    rotorCoordsNow.push_back(j);
    rotorCoordsNow.push_back(state);
    rotorCoordsNow.push_back(Id);
}

inline void updateActiveRotorId(array2D<bool>& isRotorNow, array2D<int>& activeRotorId)
{
    for(int i=0, i_end = isRotorNow.m_Width; i < i_end; ++i)
    {
        for(int j=0, j_end = isRotorNow.m_Height; j < j_end; ++j)
        {
            if(!isRotorNow(i,j)) activeRotorId(i,j) = -1;
        }
    }
}


//Calculate Bucket Function
//Find bucket for given x,y under given discretization of the grid.
//arrange grid as follows, for bucketSize = GRIDSIZE/5
//1----2----3----4----5
//6----7----8----9----10
//11---12---13---14---15
//16---17---18---19---20
//21---22---23---24---25
inline int calcBucket (int& x, int& y, const int& bucketSize)
{
    int xBucket = int((double)x/(double)bucketSize + 1.0);
    int yBucket = int((double)y/(double)bucketSize + 1.0);
    int bucketNo = xBucket + 5*yBucket;
    return bucketNo;
}


//Pacemaker Function
void pacemaker(array2D<int> &state_update, std::vector<int> &all_excited_coords, const int &RP, const int& GRIDSIZE, array2D<std::pair<int,int>> &excitedBy);

//CurrentDatetime function
const std::string currentDateTime() ;


#endif /* defined(__sdltest__main_functions__) */
