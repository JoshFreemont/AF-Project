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

class Spatch
{
    int size;
    int x;
    int y;
    int xScale;
    int yScale;
    array2D<double>* inNAddress;
    array2D<double>* inSAddress;
    array2D<double>* inEAddress;
    array2D<double>* inWAddress;
    
public:
    Spatch(const int &size, const int &x, const int &y, const int displayWidth, const int displayHeight, const int gridsize, array2D<double> &inN, array2D<double> &inS, array2D<double> &inE, array2D<double> &inW, double inNu);
    void ablate();
    
};

class Cpatch
{
    int radius;
    int x;
    int y;
    int xScale;
    int yScale;
    array2D<double>* inNAddress;
    array2D<double>* inSAddress;
    
public:
    Cpatch(const int &size, const int &x, const int &y, const int displayWidth, const int displayHeight, const int gridsize, array2D<double> &inN, array2D<double> &inS, double inNu);
};


#endif /* defined(__sdltest__patch__) */
