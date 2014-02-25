//
//  state.cpp
//  sdltest
//
//  Created by josh freemont on 05/12/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//

#include "state.h"
#include <cmath>


state_display::state_display (int origin_x_coord_init, int origin_y_coord_init, int grid_size_init, int sWidthInit, int sHeightInit, const int sElementsInit, bool** histoIsBinSelect, bool& histoIsAllSelect)
{
    IsStateView = true;
    IsDiscView = false;
    IsBreakView = true;
    IsRotorView = false;
    click_x=0;
    click_y=0;
    origin_x_coord=origin_x_coord_init;
    origin_y_coord=origin_y_coord_init;
    grid_size=grid_size_init;
    sWidth = sWidthInit;
    sHeight = sHeightInit;
    sElements=sElementsInit;
    xScale=sWidth/sElements;
    yScale=sHeight/sElements;
    IsDisc = histoIsBinSelect;
    IsAllDisc = &histoIsAllSelect;
    
}


inline void state_display::ToggleStateView(){IsStateView = !IsStateView;}

inline void state_display::ToggleDiscView(){IsDiscView = !IsDiscView;}

inline void state_display::ToggleBreakView(){IsBreakView = !IsBreakView;}

inline void state_display::ToggleRotorView(){IsRotorView = !IsRotorView;}

bool state_display::getIsStateView(){return IsStateView;}

bool state_display::getIsDiscView(){return IsDiscView;}

bool state_display::getIsRotorView(){return IsRotorView;}

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
                
            case SDLK_b:
                ToggleBreakView();
                return;
                
            case SDLK_r:
                ToggleRotorView();
                return;
                
            case SDLK_ESCAPE:
                click_x=0;
                click_y=0;
                return;
        }
            return;
            
        case SDL_MOUSEBUTTONDOWN:
            SDL_GetMouseState(&click_x, &click_y);
            if(click_x<(origin_x_coord+sWidth) && click_x>origin_x_coord && click_y>(origin_y_coord-sHeight) && click_y<origin_y_coord)
            {
                //Zoom();
                return;
            }
            click_x=0;
            click_y=0;
            return;
        default:
            return;
    }
    return;
}


//functions to determine colour of "state" print out
inline double state_display::r(double ratio)
{
    return (int)255*ratio*ratio;
}

inline double state_display::g(double ratio)
{
    return (int)255*(1.0-ratio)*(1.0-ratio)*(1.0-ratio)*(1.0-ratio);
}

inline double state_display::b(double ratio)
{
    return (int)255*(1.0-ratio);
}

inline double state_display::manual_rand(int seed, int maximum)
{
    return (double)((111397847*seed + 88736252)%maximum)/maximum;
}

//function to print out to screen.
void state_display::state_putpixel(SDL_Surface* screen, int x, int y, double ratio)
{
    if(IsStateView)
    {
        SDL_Rect rect = {static_cast<Sint16>(xScale*x),static_cast<Sint16>(yScale*y),static_cast<Uint16>(xScale),static_cast<Uint16>(yScale)};
        SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, r(ratio), g(ratio) , b(ratio)));
        return;
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
        
        SDL_Rect rect = {static_cast<Sint16>(xScale*x),static_cast<Sint16>(yScale*y),static_cast<Uint16>(xScale),static_cast<Uint16>(yScale)};
        SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, r, g , b));
        return;
    }
}

void state_display::break_putpixel(SDL_Surface* screen, int x, int y, const int &breakId)//keep disc here for later logic
{
    if(IsBreakView)
    {
        int r = int(255.0*manual_rand(breakId, 992817652));
        int g = int(255.0*manual_rand(breakId, 12817612));
        int b = int(255.0*manual_rand(breakId, 33437652));
        
        SDL_Rect rect = {static_cast<Sint16>(xScale*x),static_cast<Sint16>(yScale*y),static_cast<Uint16>(xScale),static_cast<Uint16>(yScale)};
        SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, r, g , b));
        return;
        
    }
    else return;
}

void state_display::rotor_putpixel(SDL_Surface* screen, int x, int y, double ratio)
{
    if(IsRotorView)
    {
        SDL_Rect rect = {static_cast<Sint16>(xScale*x),static_cast<Sint16>(yScale*y),static_cast<Uint16>(xScale),static_cast<Uint16>(yScale)};
        SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, ratio*155 + 100, 155*ratio + 100, 0));
        return;
    }
}

void state_display::rotor_id_putpixel(SDL_Surface* screen, int x, int y,const int &rotorId , double ratio)
{
    int r = int(255.0*manual_rand(rotorId, 992817652));
    int g = int(255.0*manual_rand(rotorId, 12817612));
    int b = int(255.0*manual_rand(rotorId, 33437652));
    
    SDL_Rect rect = {static_cast<Sint16>(xScale*x),static_cast<Sint16>(yScale*y),static_cast<Uint16>(xScale),static_cast<Uint16>(yScale)};
    SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, r, g, b));
    return;
}


