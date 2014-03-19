//
//  histogram.h
//  sdltest
//
//  Created by josh freemont on 28/11/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//

#ifndef __sdltest__histogram__
#define __sdltest__histogram__

#include <iostream>
#include <vector>

class histogram
{
    //histogram variables
    int binInterval;
    std::vector<int> frequency;
    
public:
    histogram(int binIntervalInit);
    ~histogram();
    void addPoint(int data_value);
    void addPoint(double data_value);
    void resetFrequency();
    void addPoints(std::vector<int> dataVector);
    void addPoints(std::vector<double> dataVector);
    int expValue();
    void printHist(std::ofstream& aStream);
    
};




#endif /* defined(__sdltest__histogram__) */
