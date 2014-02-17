//
//  patch.cpp
//  sdltest
//
//  Created by josh freemont on 11/02/2014.
//  Copyright (c) 2014 josh freemont. All rights reserved.
//

#include "patch.h"

Spatch::Spatch(const int &size_init, const int &x_init, const int &y_init, const int displayWidth, const int displayHeight, const int gridsize, array2D<double> &inN, array2D<double> &inS, double nuIn)
{
    x=x_init;
    y=y_init;
    size=size_init;
    xScale=(int)displayWidth/gridsize;
    yScale=(int)displayHeight/gridsize;
    
    for(int i=x_init-size/2; i<x_init+size/2; ++i)
    {
        for(int j=y_init; j<y_init+size; ++j)
        {
            inN(i,j)=nuIn;
            inS(i,j)=nuIn;
        }
    }
}


void Spatch::print (SDL_Surface* screen)
{
    for(int i=x-size/2; i<=x+size/2; ++i)
    {
        for(int j=y-size/2; j<=y+size/2; j+=size)
        {
            Uint32* pixels = (Uint32*)screen->pixels;
            Uint32* pixel = pixels + yScale*j*screen->pitch/4 + xScale*i; // offset of pointer
            *pixel = SDL_MapRGB(screen->format, 100, 100, 100);
        }
    }
    
    for(int i=x-size/2; i<=x+size/2; i+=size)
    {
        for(int j=y-size/2; j<=y+size/2; ++j)
        {
            Uint32* pixels = (Uint32*)screen->pixels;
            Uint32* pixel = pixels + yScale*j*screen->pitch/4 + xScale*i; // offset of pointer
            *pixel = SDL_MapRGB(screen->format, 100, 100, 100);
        }
    }
    return;
}


Cpatch::Cpatch(const int &radius_init, const int &x_init, const int &y_init, const int displayWidth, const int displayHeight, const int gridsize, array2D<double> &inN, array2D<double> &inS, double nuIn)
{
    x=x_init;
    y=y_init;
    radius=radius_init;
    xScale=(int)displayWidth/gridsize;
    yScale=(int)displayHeight/gridsize;
    
    for(int k=-radius; k<radius; ++k)
    {
        for(int l=-radius; l<radius; ++l)
        {
            int r2= k*k + l*l;
            if(r2<=(radius*radius))
            {
                inN(k+x,l+y)=nuIn;
                inS(k+x,l+y)=nuIn;
            }
        }
    }
}

void Cpatch::print (SDL_Surface* screen)
{
    for(int i=x-radius; i<=x+radius; ++i)
    {
        for(int j=y-radius; j<=y+radius; ++j)
        {
            int r2=(x-i)*(x-i) + (y-j)*(y-j);
            if(r2<(radius*radius) && r2>(radius*radius)-40)
            {
                Uint32* pixels = (Uint32*)screen->pixels;
                Uint32* pixel = pixels + yScale*j*screen->pitch/4 + xScale*i; // offset of pointer
                *pixel = SDL_MapRGB(screen->format, 100, 100, 100);
            }
        }
    }
    return;
}

