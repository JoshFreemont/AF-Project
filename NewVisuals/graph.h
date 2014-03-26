//
//  graph.h
//  sdltest
//
//  Created by josh freemont on 18/11/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//

#ifndef __sdltest__graph__
#define __sdltest__graph__

#include <SDL/SDL.h>
#include <iostream>
#include <vector>


class graph
{
    int origin_x_coord;
    int origin_y_coord;
    int x_axis_length;
    int y_axis_length;
    int x_scale_factor;
    int y_scale_factor;
    int x_label_interval;
    int y_label_interval;
    int label_height;
    int plot_colour [3];//select from r/g/b. [0]=r value, [1]= g_value, [2]=b_value;
    std::vector<int> points;//gives coordinates of points in form {i1,j1,i2,j2,i3,j3....iN,jN)
    std::vector<int> scaled_points;//gives scaled coordinateds of points in same form as above.
    
    //graph logic
    //INSERT LOGIC HERE//
    
    
public:
    graph(int origin_x_coord_init, int origin_y_coord_init, int x_axis_length_init, int y_axis_length_init, int x_axis_max_value_init, int y_axis_max_value_init, const char plot_colour);
    void add_point(int x, int y);
    void print_axes (SDL_Surface* screen);
    void print_points (SDL_Surface* screen);
};

#endif /* defined(__sdltest__graph__) */
