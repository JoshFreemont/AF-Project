//
//  excited_scan_algorithms.cpp
//  sdltest
//
//  Created by josh freemont on 17/11/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//

#include "main_functions.h"


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

/*void init_Kishan(double*** inN, double*** inE, double*** inS, double*** inW, const int GRIDSIZE, double defect_density, double vert_conn, double firing_prob)
 {
 srand((unsigned int)time(NULL));
 for(int i=1; i<(GRIDSIZE+1); i++)
 {
 for(int j=0; j<GRIDSIZE; j++)
 {
 if (((double)rand()/(RAND_MAX)) < defect_density)
 {
 *(inW)[i][j] = firing_prob;
 *(inE)[i][j] = firing_prob;
 }
 else
 {
 *(inW)[i][j] = 1.0;
 *(inE)[i][j] = 1.0;
 }
 }
 }
 
 for(int i=1; i<(GRIDSIZE+1); i++)
 {
 for(int j=0; j<GRIDSIZE; j++)
 {
 if (((double)rand()/(RAND_MAX)) < vert_conn)
 {
 inS[i][j] = inW[i][j];
 inN[i][(j+1)%GRIDSIZE] = inW[i][(j+1)%GRIDSIZE];
 }
 else
 {
 *(inS)[i][j] = 0.0;
 *(inN)[i][(j+1)%GRIDSIZE] = 0.0;
 }
 }
 }
 }
 */
