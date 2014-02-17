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
#include <utility>




inline void update_arrays(int &state_update, std::vector<int> &all_excited_coords_cyclic, int &excitation_frame_update,  const int& frame, const int& RP, const int& i_coord, const int& j_coord)
{
    state_update=RP;
    excitation_frame_update=frame;
    all_excited_coords_cyclic.push_back(i_coord);
    all_excited_coords_cyclic.push_back(j_coord);
    return;
}

inline int returnDisc(const int &excitation_frame_cell, const int &excitation_frame_neighbour, const int &discont_low_bound, const int &discont_high_bound)
{
    int diff = abs(excitation_frame_cell - excitation_frame_neighbour);
    if(diff < discont_high_bound && diff > discont_low_bound)return diff;
    else return 0;
}

//forwards means state1->state2.
inline int IsVBackwards(const int& state1, const int& state2)
{
    if(state1-state2<0)return true;
    else return false;
}

//state 1= state on one side of main cell, state 2= other side of main cell, state3= behind state1, state4= behind state2, state5=beside state 3, state6=beside state5.
//presume first excitation is "forward", calculate the direction of excitation between 1 and 2, with 1->2 defined as forward, calculate direction of excitation between 3 and 4, with 3->4 defined forwards. If both are Forwards then return true.

inline bool returnIsBreak(const int &state1, const int &state2, const int &state3, const int& state4)
{
    if(IsVBackwards(state1, state2) && IsVBackwards(state3, state4))
    {
        return true;
    }
    return false;
}


inline void update_discId(int& maxDiscId, int& discIdCurrent, int& discIdExcited)
{
    if(discIdCurrent == 0)
    {
        maxDiscId+=1;
        discIdExcited=maxDiscId;
        return;
    }
    
    else
    {
        discIdExcited=discIdCurrent;
        return;
    }
}


inline void update_breakId(int& breakIdNow, int& breakIdEx)
{
    breakIdEx=breakIdNow;
    return;
}

inline void generate_breakId(int& maxBreakId, int& breakIdEx)
{
    ++maxBreakId;
    breakIdEx=maxBreakId;
    return;
}


void pacemaker(SDL_Surface *screen, array2D<int> &state_update, std::vector<int> &all_excited_coords, array2D<int> &excitation_frame, const int &frame, const int &RP, const int& GRIDSIZE, state_display &display, array2D<std::pair<int,int>> &excitedBy);

void init_Kishan(double*** inN, double*** inE, double*** inS, double*** inW, const int GRIDSIZE, double defect_density, double vert_conn, double firing_prob);

#endif /* defined(__sdltest__main_functions__) */
