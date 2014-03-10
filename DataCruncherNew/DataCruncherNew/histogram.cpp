 //
//  histogram.cpp
//  sdltest
//
//  Created by josh freemont on 28/11/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//
//ADAPT FOR VARIABLE BIN LENGTH- specify bin length, make x scale factor, change getbin.

#include "histogram.h"
#include <fstream>


histogram::histogram(int binIntervalInit)
{
    binInterval = binIntervalInit;
    frequency.resize(1,0);
}

histogram::~histogram()
{
    frequency.clear();
}

void histogram::addPoint(int data_value)
{
    int bin_select = (int)(data_value/binInterval);
    if(bin_select > frequency.size())
    {
        //i is equivalent to index of vector-> must be less than bin_select.
        for(int i = (int)(frequency.size()+0.5); i<=bin_select; ++i)
        {
            frequency.push_back(0);
        }
    }
    
    if(!frequency[bin_select])frequency[bin_select] = 1;
    else frequency[bin_select]++;
}

void histogram::addPoint(double data_value)
{
    //find selected bin and resize frequency vector if out of current bounds.
    int bin_select = (int)(data_value/binInterval);
    if(bin_select > frequency.size())
    {
        //i is equivalent to index of vector-> must be less than bin_select.
        for(int i = (int)frequency.size(); i<=bin_select; ++i)
        {
            frequency.push_back(0);
        }
    }
    
    if(!frequency[bin_select])frequency[bin_select] = 1;
    else frequency[bin_select]++;
}


void histogram::addPoints(std::vector<int> dataVector)
{
    for(auto it = dataVector.begin(); it != dataVector.end(); ++it)
    {
        addPoint(*it);
    }
}
void histogram::addPoints(std::vector<double> dataVector)
{
    for(auto it = dataVector.begin(); it != dataVector.end(); ++it)
    {
        addPoint(*it);
    }
}

void histogram::resetFrequency()
{
    frequency.clear();
}

void histogram::printHist(std::ofstream& aStream)
{
    //set column headings
    aStream << "binNo" << "\t" << "frequency"<< "\t" << "minBinValue" << std::endl;
    
    //output data
    int binNo = 0;
    for(auto freq = frequency.begin(); freq != frequency.end(); freq++)
    {
        aStream << binNo << "\t" << *freq << "\t" << binInterval*binNo << std::endl;
        binNo++;
    }
    
    aStream << "\n";
}