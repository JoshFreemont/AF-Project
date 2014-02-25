//
//  excited_scan_algorithms.cpp
//  sdltest
//
//  Created by josh freemont on 17/11/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//

#include "main_functions.h"
#include <cmath>

void update_arrays(int &state_update, std::vector<int> &all_excited_coords_cyclic, int &excitation_frame_update, const int &frame, const int &RP, const int &i_coord, const int &j_coord)
{
    state_update=RP;
    excitation_frame_update=frame;
    all_excited_coords_cyclic.push_back(i_coord);
    all_excited_coords_cyclic.push_back(j_coord);
    return;
}

int returnDisc(const int &excitation_frame_cell, const int &excitation_frame_neighbour, const int &discont_low_bound, const int &discont_upper_bound)
{
    int diff = std::abs( excitation_frame_cell - excitation_frame_neighbour );

    if(diff < discont_upper_bound && diff > discont_low_bound)return diff;
    else return 0;
}


void pacemaker(int* state_update_first_col, std::vector<int> &all_excited_coords,
               std::vector<int> &excitation_frame, const int &frame, const int &RP,
               const int &GRIDSIZE, std::pair<int,int>*excitedBy)
{
    for(int j=0; j<GRIDSIZE; ++j)
        if(state_update_first_col[j]==0)
        {
            state_update_first_col[j]=RP;
            excitedBy[j]=std::make_pair (0,j);

            all_excited_coords.push_back(0);//push back i coord in same coordinate definition as the state array.
            all_excited_coords.push_back(j);//push back j coord as above
            excitation_frame[j]=frame;
        }
    return;
}


/*void pacemaker_central(SDL_Surface *screen, int &state_update_central, std::vector<int> &all_excited_coords, int &excitation_frame, const int &frame, const int& RP, const int& x_click, const int& y_click, const int& grid_size, const int& zoom, bool IsStateView)
{
        if(state_update_central==0)
        {
            state_update_central=RP;
            all_excited_coords.push_back(grid_size/2);//push back i coord in same coordinate definition as the state array.
            all_excited_coords.push_back(grid_size/2);//push back j coord as above
            excitation_frame=frame;

            if(IsStateView)state_putpixel(screen, grid_size/2, grid_size/2, 1.0, x_click, y_click, grid_size, RP, zoom);
        }
    return;
}*/

void update_discId(int& maxDiscId, int& discIdCurrent, int& discIdExcited)
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

double avg(std::vector<double> values, int length)
{
    double sum = 0;
    for(int i=0; i<length; i++)
    {
        sum += values[i];
    }
    return sum/(double)length;
}

double std_dev(std::vector<double> values, int length, double mean)
{
    double sum = 0;
    for(int i=0; i<length; i++)
    {
        sum += (values[i]-mean)*(values[i]-mean);
    }
    return sqrt(sum/(double)(length-1));
}

