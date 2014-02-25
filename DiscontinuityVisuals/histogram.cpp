//
//  histogram.cpp
//  sdltest
//
//  Created by josh freemont on 28/11/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//
//ADAPT FOR VARIABLE BIN LENGTH- specify bin length, make x scale factor, change getbin.

#include "histogram.h"
#include <cmath>

histogram::histogram(int origXInit, int origYInit, int xLengthInit, int yLengthInit, int maxInputInit, int maxFreqInit)
{
    origX=origXInit;
    origY=origYInit;
    xLength=xLengthInit;
    yLength=yLengthInit;
    yScale=(double)yLength/maxFreqInit;
    labelHeight=1;
    noBins=maxInputInit;
    binInterval=(xLength/noBins);
    inputInterval = 25;
    freqInterval=(double)0.1*maxFreqInit*yScale;
    frequency = new int[noBins];
    maxFreq=maxFreqInit;
    maxInput=maxInputInit;
    
    //histogram event logic variables
    click_x=0;
    click_y=0;
    IsBinSelectArray = new bool [noBins];
    IsAllSelect = true;
    
    //initializing arrays
    for(int i=0; i<noBins; ++i)
    {
        frequency[i] = 0;
        IsBinSelectArray[i] = false;
    }
}


void inline histogram::scale_frequency(int maxValue)
{
    maxFreq = (int)(4.5/3.0*(double)maxValue + 0.5);
    double yScaleOld = yScale;
    yScale = (double)yLength/maxFreq;
    
    //frequency axis label interval scaling
    freqInterval *= (yScale/yScaleOld);
    
    if(freqInterval<20.0)
    {
        freqInterval *= 5.0 ;
    }
}


void histogram::add_point(int data_value)
{
    frequency[data_value] += 1;
    
    //if frequency is more than 75% or the total axis height then rescale.
    if(frequency[data_value]>int(((3*maxFreq)/4) + 0.5))
    {
        scale_frequency(frequency[data_value]);
    }
    return;
}


void histogram::print_axes (SDL_Surface* screen)
{
    int x, y;
    for(x = origX; x<=(origX+noBins*binInterval); ++x)//iterate along x axis and draw line accordingly
    {
        if((x-origX)%(binInterval) == 0)//draw interval marker if at interval
        {
            if((x-origX)%inputInterval == 0)
            {
                for(y=origY-labelHeight; y<=origY+3*labelHeight; ++y)
                {
                    Uint32* pixels = (Uint32*)screen->pixels;
                    Uint32* pixel = pixels + y*screen->pitch/4 + x; // offset of pointer
                    *pixel = SDL_MapRGB(screen->format, 200, 200, 200);//brighter white for interval marker
                }
            }
            else
            {
                for(y=origY; y<=origY+labelHeight; ++y)
                {
                Uint32* pixels = (Uint32*)screen->pixels;
                Uint32* pixel = pixels + y*screen->pitch/4 + x; // offset of pointer
                *pixel = SDL_MapRGB(screen->format, 200, 200, 200);//brighter white for interval marker
                }
            }
        }
        else
        {
            Uint32* pixels = (Uint32*)screen->pixels;
            Uint32* pixel = pixels + origY*screen->pitch/4 + x; // offset of pointer
            *pixel = SDL_MapRGB(screen->format, 100, 100, 100);
        }
    }
    
    for(y=origY; y>=(origY-yLength); --y)//iterate along y axis and draw line accordingly (remember y axis is inverted)
    {
        if((y-origY)%(int(freqInterval+0.5)) == 0)//draw interval marker if at interval
        {
            for(x=origX; x<=(origX + noBins*binInterval); ++x)
            {
                Uint32* pixels = (Uint32*)screen->pixels;
                Uint32* pixel = pixels + y*screen->pitch/4 + x; // offset of pointer
                *pixel = SDL_MapRGB(screen->format, 50, 50, 50);//brighter white for interval marker
            }
        }
        else
        {
            Uint32* pixels = (Uint32*)screen->pixels;
            Uint32* pixel  = pixels + y*screen->pitch/4 + origX; // offset of pointer
            *pixel = SDL_MapRGB(screen->format, 100, 100, 100);
        }
        
    }
    return;
}


void histogram::print_histogram(SDL_Surface* screen)
{
    int x,y;
    for(int i=0; i<noBins; ++i)
    {
        y = origY - (int)(frequency[i]*yScale + 0.5);
        x = origX + i*binInterval;

        //rectangle position on screen
        SDL_Rect rect = {static_cast<Sint16>(x),static_cast<Sint16>(y),static_cast<Uint16>(binInterval),static_cast<Uint16>(origY-y)};
       
        //colour red if selected, white if not.
        if(IsAllSelect)SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 200, 200, 200));
        else if(IsBinSelectArray[i])SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 255, 255, 255));
        else SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 100, 100, 100));
    }
    return;
}


void histogram::reset_frequency()
{
    for(int i = 0; i<noBins; ++i)
    {
        frequency[i]=0;
    }
}


int histogram::getbin (int bin_entry)
{
    //bin interval- max bin entry/noBins
    int bin = (int)bin_entry*maxInput/noBins;
    return bin;
}


//histogram event logic method
void inline histogram::reset_IsBinSelect_array()
{
    for(int i = 0; i<noBins; ++i)
    {
        IsBinSelectArray[i]=false;
    }
}


void histogram::revert_mouse_unclicked()
{
    click_x=0;
    click_y=0;
    IsAllSelect = true;
    reset_IsBinSelect_array();
    
}


void histogram::handle_event(SDL_Event& event)
{
    switch(event.type)
    {
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE)
            {
                reset_IsBinSelect_array();
                revert_mouse_unclicked();
                return;
            }
            return;
            
        case SDL_MOUSEBUTTONDOWN:
            SDL_GetMouseState(&click_x, &click_y);
            
            if(click_x>origX && click_x<origX+xLength && click_y<origY && click_y>origY-yLength)
            {
                int binSelect = int(((click_x-origX)/binInterval)+0.5);//round bin.
                IsBinSelectArray[binSelect]=true;
                if(IsAllSelect)
                {
                    IsAllSelect = !IsAllSelect;
                }
                return;
            }
            else
            {
                click_x=0;
                click_y=0;
            }
            return;
    }
}


bool** histogram::getIsBinSelectArray(){return &IsBinSelectArray;}

bool& histogram::getIsAllSelect(){return IsAllSelect;}
