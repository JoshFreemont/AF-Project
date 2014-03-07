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
#include <string>
#include "rotorIDstruct.h"

//Inline Update Functions.
inline void update_arrays(int &state_update, std::vector<int> &all_excited_coords_cyclic, int &excitation_frame_update,  const int& frame, const int& RP, const int& i_coord, const int& j_coord)
{
    state_update=RP;
    excitation_frame_update=frame;
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


//Pacemaker Function
void pacemaker(array2D<int> &state_update, std::vector<int> &all_excited_coords, array2D<int> &excitation_frame, const int &frame, const int &RP, const int& GRIDSIZE, array2D<std::pair<int,int>> &excitedBy);

//CurrentDatetime function
const std::string currentDateTime() ;


#endif /* defined(__sdltest__main_functions__) */
