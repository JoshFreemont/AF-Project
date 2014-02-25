//
//  state.cpp
//  sdltest
//
//  Created by josh freemont on 05/12/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//

#include "state.h"


state_display::state_display (int origin_x_coord_init, int origin_y_coord_init, int grid_size_init, bool** histoIsBinSelect, bool& histoIsAllSelect)
{
    IsZoomed = false;
    IsStateView = true;
    IsDiscView = true;
    click_x=0;
    click_y=0;
    origin_x_coord=origin_x_coord_init;
    origin_y_coord=origin_y_coord_init;
    grid_size=grid_size_init;
    zoom=4;
    zoombox_size=grid_size/zoom;
    IsDisc = histoIsBinSelect;
    IsAllDisc = &histoIsAllSelect;
}

inline void state_display::Zoom(){IsZoomed = true;}

inline void state_display::UnZoom(){IsZoomed = false;}

inline void state_display::ToggleStateView(){IsStateView = !IsStateView;}

inline void state_display::ToggleDiscView(){IsDiscView = !IsDiscView;}

bool state_display::getIsStateView(){return IsStateView;}

bool state_display::getIsDiscView(){return IsDiscView;}

bool state_display::getIsZoomed(){return IsZoomed;}

int state_display::get_x(){return click_x;}

int state_display::get_y(){return click_y;}

void state_display::handle_event(SDL_Event &event)
{
    switch(event.type)
    {
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
        {
            case SDLK_d:
                ToggleDiscView();
                return;
                
            case SDLK_s:
                ToggleStateView();
                return;
                
            case SDLK_ESCAPE:
                click_x=0;
                click_y=0;
                if(IsZoomed){UnZoom();}
                return;
        }
            return;
            
        case SDL_MOUSEBUTTONDOWN:
            SDL_GetMouseState(&click_x, &click_y);
            if(click_x<(origin_x_coord+grid_size) && click_x>origin_x_coord && click_y>(origin_y_coord-grid_size) && click_y<origin_y_coord)
            {
                Zoom();
                return;
            }
            click_x=0;
            click_y=0;
            return;
    }
    return;
}

inline void state_display::zoombox_x_limits (int& max_x, int& min_x)//set limits to x coords of the zoom box.
{
    if(click_x>zoombox_size/2-1 && click_x<grid_size-zoombox_size/2)
    {
        max_x= click_x+(zoombox_size/2)-1;
        min_x= click_x-zoombox_size/2;
        return;
    }
 
    else if(click_x<(zoombox_size/2)-1)
    {
        max_x= zoombox_size-1;
        min_x= 0;
        return;
    }
 
    else if(click_x>=grid_size-1-zoombox_size/2)
    {
        max_x= grid_size-1;
        min_x= grid_size-1-zoombox_size;
        return;
    }
 
}
 

 //function to determine y limits to zoomed area
inline void state_display::zoombox_y_limits (int& max_y, int& min_y)//set limits to the y coords of the zoom box.
 {
     if(click_y>=zoombox_size/2-1 && click_y<grid_size-1-zoombox_size/2)
     {
         max_y= click_y+(zoombox_size/2)-1;
         min_y= click_y-(zoombox_size/2);
         return;
     }
 
     else if(click_y<(zoombox_size/2)-1)
     {
         max_y= click_y+(zoombox_size/2)-1;
         min_y= click_y+(grid_size-zoombox_size)+zoombox_size/2;
         return;
     }
 
     else if(click_y>=grid_size-zoombox_size/2)
     {
         max_y=click_y-(grid_size-zoombox_size)-zoombox_size/2;
         min_y=click_y-zoombox_size/2;
         return;
     }
 }
 
 
 //function to carry out coordinate transform to zoomed coordinates
inline void state_display::transform_coordinates(int& x, int& y)
    {
        int max_x, min_x, max_y, min_y;
        zoombox_x_limits(max_x, min_x);
        zoombox_y_limits(max_y, min_y);
 
 //calculate new coordinates relative to box- ie. for y, define ymax= top edge, ymin=bottom edge and find all pixels between
 //then print them to the full screen.
 
        if(min_y>max_y)//case where zoombox overlaps with continuous N/S boundary
        {
            y=(grid_size/zoombox_size)*((y+(grid_size-min_y))%grid_size)-1;//reassigning coords based on zoom
            x=(grid_size/zoombox_size)*(x-min_x);
            return;
        }
        else//case where zoombox does not overlap with continous boundary
        {
            y=(grid_size/zoombox_size)*(y-min_y);//reassigning x y coords based on zoom.
            x=(grid_size/zoombox_size)*(x-min_x);
            return;
        }
 }
 
 
//functions to determine colour of "state" print out
inline double state_display::r(double ratio)
{
    return ratio*ratio;
}

inline double state_display::g(double ratio)
{
    return (1.0-ratio)*(1.0-ratio)*(1.0-ratio)*(1.0-ratio);
}

inline double state_display::b(double ratio)
{
    return (1.0-ratio);
}

inline double state_display::manual_rand(int seed, int maximum)
{
    return (double)((111397847*seed + 88736252)%maximum)/maximum;
}

//function to print out to screen.
void state_display::state_putpixel(SDL_Surface* screen, int x, int y, double ratio, int RP)
{
    if(IsStateView)
    {
        if(click_x==0 && click_y==0)//no zoom
        {
            Uint32* pixels = (Uint32*)screen->pixels;
            Uint32* pixel = pixels + y*screen->pitch/4 + x; // offset of pointer
 
            *pixel = SDL_MapRGB(screen->format, (int)255*r(ratio),(int)255*g(ratio), (int)255*b(ratio));
            return;
        }
 
        else//zoom
        {
            //transform to zoomed coordinates.
            int zoombox_size=(int)grid_size/zoom;
            transform_coordinates(x, y);
 
            if(x<grid_size && y<grid_size)//make sure that zoom falls within limits of screen
            {
                //rectangle position and fill
                SDL_Rect rect = {static_cast<Sint16>(x),static_cast<Sint16>(y),static_cast<Uint16>(grid_size/zoombox_size),static_cast<Uint16>(grid_size/zoombox_size)};
                SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, (int)255*r(ratio),(int)255*g(ratio), (int)255*b(ratio)));
            }
            return;
        }
    }
    else return;
}



void state_display::disc_putpixel(SDL_Surface* screen, int x, int y, int disc, const int &discId)//keep disc here for later logic
{
    if(IsDiscView && (*IsAllDisc || (*IsDisc)[disc]))
    {
        int r = int(255.0*manual_rand(discId, 992817652));
        int g = int(255.0*manual_rand(discId, 12817612));
        int b = int(255.0*manual_rand(discId, 33437652));
 
        if(click_x==0 && click_y==0)//no zoom
        {
            Uint32* pixels = (Uint32*)screen->pixels;
            Uint32* pixel = pixels + y*screen->pitch/4 + x; // offset of pointer 
            *pixel = SDL_MapRGB(screen->format, r, g, b);
            return;
        }
 
        else//zoom
        {
            //transform to zoomed coordinates.
            int zoombox_size=(int)grid_size/zoom;
            transform_coordinates(x, y);
 
            if(x<grid_size && y<grid_size)//make sure that zoom falls within limits of screen
            {
                //rectangle position and fill
                SDL_Rect rect = {static_cast<Sint16>(x),static_cast<Sint16>(y),static_cast<Uint16>(grid_size/zoombox_size),static_cast<Uint16>(grid_size/zoombox_size)};
                SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, r, g, b));
            }
            return;
        }
    }
    else return;
}






