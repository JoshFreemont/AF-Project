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
#include "array2D.h"
#include <utility>




inline void update_arrays(int &state_update, std::vector<int> &all_excited_coords_cyclic, int &excitation_frame_update,  const int& frame, const int& RP, const int& i_coord, const int& j_coord)
{
    state_update=RP;
    excitation_frame_update=frame;
    all_excited_coords_cyclic.push_back(i_coord);
    all_excited_coords_cyclic.push_back(j_coord);
    return;
}

void pacemaker(array2D<int> &state_update, std::vector<int> &all_excited_coords, array2D<int> &excitation_frame, const int &frame, const int &RP, const int& GRIDSIZE, array2D<std::pair<int,int>> &excitedBy);

void init_Kishan(double*** inN, double*** inE, double*** inS, double*** inW, const int GRIDSIZE, double defect_density, double vert_conn, double firing_prob);

#endif /* defined(__sdltest__main_functions__) */
