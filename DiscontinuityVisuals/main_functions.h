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

//all_excited coords[previous_vector_frame], vector_frame, RP, zoom, logic, grid_size, x, y.

void update_arrays(int &state_update, std::vector<int> &all_excited_coords_cyclic, int &excitation_frame_update,  const int& frame, const int& RP, const int& i_coord, const int& j_coord);

int returnDisc(const int &excitation_frame_cell, const int &excitation_frame_neighbour, const int &discont_low_bound, const int &discont_high_bound);

void pacemaker(SDL_Surface *screen, int* state_update_first_col, std::vector<int> &all_excited_coords, std::vector<int> &excitation_frame, const int &frame, const int& RP, const int& GRIDSIZE, state_display &display);

void pacemaker_central(SDL_Surface *screen, int &state_update_central, std::vector<int> &all_excited_coords, int &excitation_frame, const int &frame, const int& RP, const int& x_click, const int& y_click, const int& grid_size, const int& zoom, bool IsStateView);

void update_discId(int& maxDiscId, int& discIdCurrent, int&discIdExcited);

void initialise_Kishan(std::vector< std::vector<double> > &, std::vector< std::vector<double> > &, std::vector< std::vector<double> > &, std::vector< std::vector<double> > &,int,double,double,double);

#endif /* defined(__sdltest__main_functions__) */
