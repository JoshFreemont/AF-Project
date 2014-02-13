//
//  graph.cpp
//  sdltest
//
//  Created by josh freemont on 18/11/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//

#include "graph.h"

graph::graph(int origin_x_coord_init, int origin_y_coord_init, int x_axis_length_init, int y_axis_length_init, int x_axis_max_value_init, int y_axis_max_value_init, char colour)
{
    origin_x_coord=origin_x_coord_init;
    origin_y_coord=origin_y_coord_init;
    x_axis_length=x_axis_length_init;
    y_axis_length=y_axis_length_init;
    
    //as yet- these are unused but will be employed later.
    x_scale_factor=(double)(x_axis_length/x_axis_max_value_init);
    y_scale_factor=(double)(y_axis_length/y_axis_max_value_init);
    
    x_label_interval=(int)x_axis_length/10;
    y_label_interval=(int)y_axis_length/10;
    label_height=1;
    
    switch(colour)
    {
        case 'r':
            plot_colour[0]=255;
            plot_colour[1]=50;
            plot_colour[2]=25;
            break;
            
        case 'g':
            plot_colour[0]=25;
            plot_colour[1]=255;
            plot_colour[2]=50;
            break;
        
        case 'b':
            plot_colour[0]=50;
            plot_colour[1]=25;
            plot_colour[2]=255;
            break;
        
        default:
            plot_colour[0]=255;
            plot_colour[1]=255;
            plot_colour[2]=255;
            break;
    }
    
    points.reserve(2*x_axis_max_value_init);
    points={0,0};
    scaled_points.reserve(2*x_axis_max_value_init);
    scaled_points={0,0};
}



void graph::add_point(int x, int y)
{
    points.push_back(x);
    points.push_back(y);
    scaled_points.push_back((int)x/10);
    scaled_points.push_back((int)y/25);
    return;
}



void graph::print_axes (SDL_Surface* screen)
{
    int x, y;
    for(x = origin_x_coord; x<(origin_x_coord+x_axis_length); ++x)//iterate along x axis and draw line accordingly
    {
        if((int)(x-origin_x_coord)%x_label_interval==0)//draw interval marker if at interval
        {
            for(y=origin_y_coord-label_height; y<=origin_y_coord+label_height; ++y)
            {
                Uint32* pixels = (Uint32*)screen->pixels;
                Uint32* pixel = pixels + y*screen->pitch/4 + x; // offset of pointer
                *pixel = SDL_MapRGB(screen->format, 200, 200, 200);//brighter white for interval marker
            }
        }
        else
        {
            Uint32* pixels = (Uint32*)screen->pixels;
            Uint32* pixel = pixels + origin_y_coord*screen->pitch/4 + x; // offset of pointer
            *pixel = SDL_MapRGB(screen->format, 100, 100, 100);
        }
    }
    
    for(y=origin_y_coord; y>(origin_y_coord-y_axis_length); --y)//iterate along y axis and draw line accordingly (remember y axis is inverted)
    {
        if((int)(y-origin_y_coord)%y_label_interval==0)//draw interval marker if at interval
        {
            for(x=origin_x_coord-label_height; x<=origin_x_coord+label_height; ++x)
            {
                Uint32* pixels = (Uint32*)screen->pixels;
                Uint32* pixel = pixels + y*screen->pitch/4 + x; // offset of pointer
                *pixel = SDL_MapRGB(screen->format, 200, 200, 200);//brighter white for interval marker
            }
        }
        else
        {
            Uint32* pixels = (Uint32*)screen->pixels;
            Uint32* pixel  = pixels + y*screen->pitch/4 + origin_x_coord; // offset of pointer
            *pixel = SDL_MapRGB(screen->format, 100, 100, 100);
        }
       
    }
    return;
}



void graph::print_points (SDL_Surface* screen)
{
    int x, y;
    for(std::vector<int>::iterator it=scaled_points.begin(); it!=scaled_points.end(); it+=2)
    {
        x= *it + origin_x_coord;
        y= origin_y_coord - *(it+1);
        Uint32* pixels = (Uint32*)screen->pixels;
        Uint32* pixel = pixels + y*screen->pitch/4 + x; // offset of pointer
        *pixel = SDL_MapRGB(screen->format, plot_colour[0], plot_colour[1], plot_colour[2]);//brighter white for points
    
    }
    return;
}

