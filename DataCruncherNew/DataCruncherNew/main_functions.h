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
#include <fstream>
#include <string>

//INLINE FUNCTIONS

//update functions
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

//state de-excitation process
inline void deExciteState(std::vector<std::vector<int> >& exCoords, const int& cyclicOld, const int& cyclicBackRP, const int& MEMLIMIT, array2D<int>& state_update)
{
    for (int row = cyclicOld, rowEnd = cyclicBackRP; row != rowEnd; row=(row-1+MEMLIMIT)%MEMLIMIT)
    {
        for(std::vector<int>::iterator col = exCoords[row].begin(), col_end = exCoords[row].end(); col != col_end; col+=2)
        {
            --state_update(*col,*(col+1));
        }
    }
    return;
}

//Buckets
//assign bucket xy coords
inline void assignBucketCoords(std::vector<std::pair<int, int> >& coordList, int noBuckets, int bucketSize)
{
    for(int i=0; i < noBuckets; ++i)
    {
        int noBuckets1D = (int)sqrt(noBuckets);
        coordList[i] = std::make_pair((i%noBuckets1D)*bucketSize + (int)bucketSize/2, (int)(i/noBuckets1D)*bucketSize + (int)bucketSize/2);
    }
    return;
}


//Find bucket for given x,y under given discretization of the grid.
//arrange grid as follows, for bucketSize = GRIDSIZE/5
//0----1----2----3----4
//5----6----7----8----9
//...
//20---21---22---23---24
inline int calcBucket (int& x, int& y, const int& bucketSize, const int& noBuckets)
{
    int bucketNo1D = (int)sqrt(noBuckets);
    int xBucket = int((double)x/(double)bucketSize);
    int yBucket = int((double)y/(double)bucketSize);
    int bucketNo = xBucket + bucketNo1D*yBucket;
    return bucketNo;
}


//NON-INLINE FUNCTIONS
//Pacemaker Function
void pacemaker(array2D<int> &state_update, std::vector<int> &all_excited_coords, const int &RP, const int& GRIDSIZE, array2D<std::pair<int,int>> &excitedBy, int& exCells);

//CurrentDatetime function
const std::string currentDateTime() ;

//rotorIdDataStruct
//Birth data
std::vector<int> getBirthDataVect(std::vector<rotorIDstruct>& rotorIdData);

//Death data
std::vector<int> getDeathDataVect(std::vector<rotorIDstruct>& rotorIdData, const int MAXFRAME);

struct optionsStruct
{
	std::string m_FileHeader;
	bool m_DETECTROTORS;
	bool m_COUNTEXCELLS;
	bool m_DISPLAYFULLEXCELLS;
	double m_nuSTART;
	double m_nuMAX;
	double m_nuSTEP;
	int m_repeatMAX;
	int m_MAXFRAME;
};

void readOptionsFile(std::ifstream& opFile, optionsStruct& startOptions);

#endif /* defined(__sdltest__main_functions__) */
