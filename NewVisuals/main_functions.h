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
#include <vector>
#include <SDL/SDL.h>
#include "state.h"
#include "array2D.h"
//#include "data_structs.h"
#include <utility>


inline void update_arrays(int &state_update, std::vector<int> &all_excited_coords_cyclic, int &excitation_frame_update, const int& frame, const int& RP, const int& i_coord, const int& j_coord)
{
    state_update=RP;
    excitation_frame_update=frame;
    all_excited_coords_cyclic.push_back(i_coord);
    all_excited_coords_cyclic.push_back(j_coord);
    return;
}

void pacemaker(SDL_Surface *screen, array2D<int> &state_update, std::vector<int> &all_excited_coords, array2D<int> &excitation_frame, const int &frame, const int &RP, const int& GRIDSIZE, state_display &display, array2D<std::pair<int,int>> &excitedBy);

void initStaticDefects (array2D<double>& inW, array2D<double>& inE, const double& delta, const double& espilon);

void initStaticVerts (array2D<double>& inW, array2D<double>& inE, array2D<double>& inN, array2D<double>& inS, const double& nu, const int& GRIDSIZE);

#endif /* defined(__sdltest__main_functions__) */
