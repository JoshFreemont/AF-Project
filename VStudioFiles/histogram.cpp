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
#include <cmath>


histogram::histogram(int binIntervalInit)
{
    binInterval = binIntervalInit;
    frequency.resize(1,0);
}

histogram::~histogram()
{
    frequency.clear();
}


//add individual datum to histo in int form
void histogram::addPoint(int data_value)
{
    //add point to "lower" bin. ie. if bin interval = 2 and data is 1 then add to 0th bin.
    int bin_select = static_cast<int>(floor((double)data_value/(double)binInterval));
    if(bin_select >= frequency.size())//resize vector if vector[bin] doesn't exist.
    {
        for(int i = (int)((double)frequency.size() + 0.5); i <= bin_select; ++i)
        {
            frequency.push_back(0);
        }
    }
    
    if(!frequency[bin_select])frequency[bin_select] = 1;
    else frequency[bin_select]++;
}


//add individual datum to histo in double form
void histogram::addPoint(double data_value)
{
    //find selected bin and resize frequency vector if out of current bounds.
    //selected bin is labelled [0]->[maxBin -1]
    int bin_select = (int)floor(data_value/(double)binInterval+0.5);
    if(bin_select > frequency.size())
    {
        //i is equivalent to index of vector-> must be less than bin_select.
        for(int i = (int)frequency.size(); i<=(bin_select+1); ++i)
        {
            frequency.push_back(0);
        }
    }
    
    if(!frequency[bin_select])frequency[bin_select] = 1;
    else frequency[bin_select]++;
}


//add data to histo in vector<int> form
void histogram::addPoints(std::vector<int> dataVector)
{
    for(auto it = dataVector.begin(); it != dataVector.end(); ++it)
    {
        addPoint(*it);
    }
}

//add data to histo in vector<double> form
void histogram::addPoints(std::vector<double> dataVector)
{
    if(!dataVector.size()) return;
    for(auto it = dataVector.begin(); it != dataVector.end(); ++it)
    {
        addPoint(*it);
    }
}

//Calculate expectation value for the data input - ie. E(x).
int histogram::expValue()
{
    int sum = 0;
    int bin = 0;
    int totalFrequency = 0;
    for(auto it = frequency.begin(); it != frequency.end(); ++it)
    {
        sum += (int)((*it)*(bin*binInterval + binInterval/2));
        totalFrequency += (*it);
        bin++;
    }
    
    if(totalFrequency)sum /= totalFrequency;
    return sum;
}

//Calculate Std deviation of data input -  ie. sqrt(Var(x))



//reset frequency
void histogram::resetFrequency()
{
    frequency.clear();
}


//write histo to file.
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