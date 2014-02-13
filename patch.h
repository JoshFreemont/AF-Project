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
#include <SDL/SDL.h>

class Spatch
{
    int size;
    int x;
    int y;
    int xScale;
    int yScale;
    
public:
    Spatch(const int &size, const int &x, const int &y, const int displayWidth, const int displayHeight, const int gridsize);
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
    Cpatch(const int &radius, const int &x, const int &y, const int displayWidth, const int displayHeight, const int gridsize);
    void print (SDL_Surface* screen);
};


#endif /* defined(__sdltest__patch__) */
