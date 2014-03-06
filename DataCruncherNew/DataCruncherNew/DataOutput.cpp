//
//  DataOutput.cpp
//  DataCruncherNew
//
//  Created by josh freemont on 06/03/2014.
//  Copyright (c) 2014 com.example. All rights reserved.
//

#include "DataOutput.h"


void FOutRotorIdData(std::ofstream& aStream, std::vector<struct rotorIDstruct> &rotorIDdata)
{
    int rotorCounter = 0;
    for (auto it=rotorIDdata.begin();it!=rotorIDdata.end();it++)
    {
		aStream << rotorCounter << "\t";
		aStream << it->birthframe << "\t";
        aStream << it->lifetime << "\t";
		
		double m = mean(it->length); //std dev of lengths
		double accum = 0.0;
		std::for_each (std::begin(it->length), std::end(it->length), [&](const double d)
                       {
                           accum += (d - m) * (d - m);
                       });
        
		double stdev = sqrt(accum / (it->length.size())); //std dev of lengths
		aStream << m << "\t";
		aStream << stdev << "\t";
		aStream << *min_element(it->length.begin(),it->length.end()) << "\t"; //Min length
		aStream << *max_element(it->length.begin(),it->length.end()) << "\t"; //Max length
		aStream << it->birthX << "\t" << it->birthY << "\t"; //rotor birth locations
		aStream << it->deathX << "\t" << it->deathY << "\n"; //rotor death locations
		
		rotorCounter++;
    }
    return;
}

void FOutRotorIDColumns(std::ofstream& aStream)
{
    aStream << "Rotor ID" << "\t";
    aStream << "Birth Frame" << "\t";
    aStream << "Lifetime" << "\t";
    aStream << "Mean length" << "\t";
    aStream << "Length.stddev" << "\t";
    aStream << "Min Length" << "\t";
    aStream << "Max Length" << "\t";
    aStream << "BirthX" << "\t" << "BirthY" << "\t";
    aStream << "DeathX" << "\t" << "DeathY" << "\n";

}

void FOutRotorExCountData(std::ofstream& aStream, array2D<int>& rotorCellFrequency)
{
    for(int l=0; l<rotorCellFrequency.m_Height; ++l)
    {
        for(int k=0; k<rotorCellFrequency.m_Width; ++k)
        {
            aStream << rotorCellFrequency(k,l) << "\t";
        }
        aStream << "\n";
    }
    return;
}

void COutCurrentStatus(const int& TotalIterations, const int& iterationcount)
{
    std::cout << iterationcount << " out of " << TotalIterations << " complete";
    std::cout << " at time " << currentDateTime() << ".\n";
}

void FOutFrameVsVar(std::ofstream& aStream, const int& FRAME, const int& Var)
{
    aStream<<FRAME<<"\t"<<Var<<"\n";
}