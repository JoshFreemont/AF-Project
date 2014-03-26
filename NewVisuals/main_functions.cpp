//
//  excited_scan_algorithms.cpp
//  sdltest
//
//  Created by josh freemont on 17/11/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//

#include "main_functions.h"
#include "mtrand.h"
#include <ctime>


void pacemaker(SDL_Surface *screen, array2D<int> &state_update, std::vector<int> &all_excited_coords, array2D<int> &excitation_frame, const int &frame, const int& RP, const int& GRIDSIZE, state_display &display, array2D<std::pair<int,int>> &excitedBy)
{
    for(int j=0; j<GRIDSIZE; ++j)
        if(state_update(0,j)==0)
        {
            state_update(0,j)=RP;
            excitedBy(0,j)=std::make_pair (0,j);
            all_excited_coords.push_back(0);//push back i coord in same coordinate definition as the state array.
            all_excited_coords.push_back(j);//push back j coord as above
            excitation_frame(0,j)=frame;
            display.state_putpixel(screen, 0, j, 1.0);
        }
    return;
}

void initStaticDefects (array2D<double>& inW, array2D<double>& inE, const double& delta, const double& epsilon)
{
	MTRand drand(time(NULL));

	for(int i = 0; i<inW.m_Width; ++i)
	{
		for(int j = 0; j<inW.m_Height; ++j)
		{
			if(drand() <= delta)
			{
				inW(i,j) = 1.0 - epsilon;
				inE(i,j) = 1.0 - epsilon;
			}

			else
			{
				inW(i,j) = 1.0;
				inE(i,j) = 1.0;
			}
		}
	}
}


void initStaticVerts (array2D<double>& inW, array2D<double>& inE, array2D<double>& inN, array2D<double>& inS, const double& nu, const int& GRIDSIZE)
{
	MTRand drand(time(NULL));
	double defectFactor;
	for(int i = 0; i<inW.m_Width; ++i)
	{
		for(int j = 0; j<inW.m_Height; ++j)
		{
			defectFactor = inW(i,j);
			if(drand() <= nu)
			{
				inS(i,j) = defectFactor;
				inN(i,(j+1)%GRIDSIZE) = defectFactor;
			}

			else
			{
				inS(i,j) = 0.0;
				inN(i,(j+1)%GRIDSIZE) = 0.0;
			}
		}
	}
}
