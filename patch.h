//
//  patch.h
//  sdltest
//
//  Created by josh freemont on 11/02/2014.
//  Copyright (c) 2014 josh freemont. All rights reserved.
//

#ifndef __sdltest__patch__
#define __sdltest__patch__

#include <iostream>
#include "array2D.h"
#include <SDL/SDL.h>

class Spatch
{
    int size;
    int x;
    int y;
    int xScale;
    int yScale;
    
public:
    Spatch(const int &size, const int &x, const int &y, const int displayWidth, const int displayHeight, const int gridsize, array2D<double> &inN, array2D<double> &inS, double inNu);
    void print (SDL_Surface* screen);
    
};

class Cpatch
{
    int radius;
    int x;
    int y;
    int xScale;
    int yScale;
    
public:
    Cpatch(const int &size, const int &x, const int &y, const int displayWidth, const int displayHeight, const int gridsize, array2D<double> &inN, array2D<double> &inS, double inNu);
    void print (SDL_Surface* screen);
};


#endif /* defined(__sdltest__patch__) */
