//
//  line.h
//  AF
//
//  Created by josh freemont on 27/02/2014.
//  Copyright (c) 2014 com.example. All rights reserved.
//

#ifndef __AF__line__
#define __AF__line__

#include <SDL/SDL.h>
#include <iostream>
#include <cmath>
#include <vector>

class line
{
    int xStart, xEnd, yStart, yEnd;

public:
    line();
    void addLine(int xStart, int yStart, int xEnd, int yEnd);
    void drawLine(SDL_Surface* screen);
};


class lineGraph
{
    int xOrigin, yOrigin;
    std::vector<line* > lineCoords;//data stored [0]=xStart, [1]=yStart, [2]=xEnd, [3]=yEnd;

public:
    lineGraph(int xOriginInit, int yOriginInit);
    void addLine(int xStart, int yStart, int xEnd, int yEnd);
    void drawLineGraph(SDL_Surface* screen);
    
};

#endif /* defined(__AF__line__) */