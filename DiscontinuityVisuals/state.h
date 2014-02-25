//
//  state.h
//  sdltest
//
//  Created by josh freemont on 05/12/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//

#ifndef __sdltest__state__
#define __sdltest__state__

#include <iostream>
#include <SDL/SDL.h>

class state_display
{
    int grid_size;
    int origin_x_coord;
    int origin_y_coord;
    int zoom;
    int zoombox_size;
    
    //state display logic parameters
    bool IsZoomed;
    bool IsStateView;
    bool IsDiscView;
    int click_x;
    int click_y;
    bool** IsDisc;
    bool* IsAllDisc;
    
    
public:
    state_display (int origin_x_coord, int origin_y_coord, int grid_size, bool** histoIsBinSelect, bool& histoIsAllSelect);
    inline double r(double ratio);
    inline double b(double ratio);
    inline double g(double ratio);
    inline void zoombox_x_limits(int&, int&);
    inline void zoombox_y_limits(int&, int&);
    inline void transform_coordinates(int& x, int& y);
    void state_putpixel(SDL_Surface* , int i , int j , double RP_ratio, int RP);
    void disc_putpixel(SDL_Surface* screen, int x, int y, int disc, const int &discId);
    inline double manual_rand(int seed, int maximum);
    
    //state display logic methods
    inline void Zoom ();
    inline void UnZoom();
    inline void ToggleStateView();
    inline void ToggleDiscView();
    bool getIsStateView();
    bool getIsDiscView();
    bool getIsZoomed();
    void handle_event(SDL_Event&);
    int get_x();
    int get_y();
   
};

#endif /* defined(__sdltest__state__) */
