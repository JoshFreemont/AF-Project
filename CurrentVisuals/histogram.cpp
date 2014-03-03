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

//breaks for bin_no!=length.

histogram::histogram(int origXInit, int origYInit, int xLengthInit, int yLengthInit, int maxInputInit, int noBinsInit, int maxFreqInit)
{
    origX=origXInit;
    origY=origYInit;
    xLength=xLengthInit;
    yLength=yLengthInit;
    yScale=(double)yLength/maxFreqInit;
    xScale=(double)xLength/maxInputInit;
    labelSize=1;
    noBins=noBinsInit;
    binInterval=(maxInputInit/noBins);//this is the "numerical" bin interval not the "printed" bin interval.
    xLabel = 25*xScale;
    yLabel=(double)0.1*maxFreqInit*yScale;
    frequency = new int[noBins];
    maxFreq=maxFreqInit;
    maxInput=maxInputInit;
    xLength=binInterval*noBins;
    
    
    //histogram event logic variables
    click_x=0;
    click_y=0;
    IsBinSelectArray = new bool [noBins];
    IsAllSelect = true;
    IsCumulative = false;
    
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
    yLabel *= (yScale/yScaleOld);
    
    if(yLabel<20.0)
    {
        yLabel *= 5.0 ;
    }
}


void histogram::add_point(int data_value)
{
    int bin_select = (int)(data_value/binInterval);
    frequency[bin_select] += 1;
    
    //if frequency is more than 75% or the total axis height then rescale.
    if(frequency[bin_select]>int(((3*maxFreq)/4) + 0.5))
    {
        scale_frequency(frequency[bin_select]);
    }
    return;
}


void histogram::print_axes (SDL_Surface* screen)
{
    int x, y;
    for(x = origX; x<=(origX+(int)noBins*binInterval*xScale); ++x)//iterate along x axis and draw line accordingly
    {
        if((x-origX)%(int((double)binInterval*xScale)) == 0)//draw interval marker if at interval
        {
            if((x-origX)%xLabel == 0)
            {
                for(y=origY-labelSize; y<=origY+3*labelSize; ++y)
                {
                    Uint32* pixels = (Uint32*)screen->pixels;
                    Uint32* pixel = pixels + y*screen->pitch/4 + x; // offset of pointer
                    *pixel = SDL_MapRGB(screen->format, 200, 200, 200);//brighter white for interval marker
                }
            }
            else
            {
                for(y=origY; y<=origY+labelSize; ++y)
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
        if((y-origY)%(int(yLabel+1.0)) == 0)//draw interval marker if at interval
        {
            for(x=origX; x<=(origX + (int)noBins*binInterval*xScale); ++x)
            {
                Uint32* pixels = (Uint32*)screen->pixels;
                Uint32* pixel = pixels + y*screen->pitch/4 + x; // offset of pointer
                *pixel = SDL_MapRGB(screen->format, 44, 44, 44);//brighter white for interval marker
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
        x = origX + (int)i*binInterval*xScale;

        //rectangle position on screen
        SDL_Rect rect = {static_cast<Sint16>(x),static_cast<Sint16>(y),static_cast<Uint16>((int)binInterval*xScale),static_cast<Uint16>(origY-y)};
       
        //colour white if selected, grey if not.
        if(IsAllSelect)SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 200, 200, 200));
        else if(IsBinSelectArray[i])SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 255, 255, 255));
        else SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 100, 100, 100));
    }
    return;
}


void histogram::reset_frequency()
{
    if(!IsCumulative)
    {
        for(int i = 0; i<noBins; ++i)
        {
            frequency[i]=0;
        }
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
    return;
}


void inline histogram::revert_mouse_unclicked()
{
    click_x=0;
    click_y=0;
    IsAllSelect = true;
    reset_IsBinSelect_array();
    
}

void histogram::reset_maxFreq()
{
    maxFreq=0;
    for(int i=0; i<noBins; ++i)
    {
       if(frequency[i]>maxFreq)maxFreq=frequency[i];
    }
    return;
}



void histogram::handle_event(SDL_Event& event)
{
    switch(event.type)
    {
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    reset_IsBinSelect_array();
                    revert_mouse_unclicked();
                    /*reset_maxFreq();
                    scale_frequency(maxFreq);
                    return;
                     */
                /*case SDLK_a:
                    IsCumulative=!IsCumulative;
                    return;
                */
                    
                default: return;
            }
            
        case SDL_MOUSEBUTTONDOWN:
            SDL_GetMouseState(&click_x, &click_y);
            
            if(click_x>origX && click_x<origX+xLength && click_y<origY && click_y>origY-yLength)
            {
                int binSelect = int(((click_x-origX)/((double)binInterval*xScale)+0.5));//round bin.
                IsBinSelectArray[binSelect]=!IsBinSelectArray[binSelect];
                for(int i=0; i<noBins; ++i)
                    {
                        IsAllSelect+=IsBinSelectArray[i];
                    }
                IsAllSelect=!IsAllSelect;
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
