//
//  DataOutput.cpp
//  DataCruncherNew
//
//  Created by josh freemont on 06/03/2014.
//  Copyright (c) 2014 com.example. All rights reserved.
//

#include "DataOutput.h"
#include "analysisfunctions.h"
#include <cmath>
#include <iomanip>
#include <utility>
#include <vector>


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
		aStream << it->deathX << "\t" << it->deathY << "\t"; //rotor death locations
        aStream << it->totalDefects << "\t" << it->totalVerts << "\n";

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
    aStream << "DeathX" << "\t" << "DeathY" << "\t";
    aStream << "totalDefects" << "\t" << "totalVerts" << "\n";

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
    aStream.flush();
    aStream.clear();
    return;
}

void COutCurrentStatus(const int& TotalIterations, const int& iterationcount)
{
    std::cout << iterationcount << " out of " << TotalIterations << " complete";
    std::cout << " at time " << currentDateTime() << ".\n";
}

void FOutXvsY(std::ofstream& aStream, const int& x, const int& y)
{
    aStream<<x<<"\t"<<y<<"\n";
    aStream.flush();
    aStream.clear();
}

void FOutXvsY(std::ofstream& aStream, std::vector<int>& data)
{
    int x = 0;
    for(auto it = data.begin(); it != data.end(); ++it)
    {
        aStream << x << "\t" << *it << "\n";
        x++;
    }
    
    aStream.flush();
    aStream.clear();
}


void FOutExCellsColumns(std::ofstream& aStream)
{
	aStream << "Frame" << "\t";
    aStream << "Excited Cells" << "\n";
    aStream.flush();
    aStream.clear();
    return;
}

void FOutExStatsColumns(std::ofstream& aStream)
{
	aStream << "Iteration" << "\t";
    aStream << "Frames in AF" << "\t";
	aStream << "Time in AF" << "\t";
	aStream << "Horizontal Firing Prob" << "\t";
	aStream << "Vertical Firing Prob" << "\n";
    aStream.flush();
    aStream.clear();
    return;
}

void FOutExMasterColumns(std::ofstream& aStream)
{
	aStream << "Nu" << "\t";
	aStream << "Mean Frames in AF" << "\t";
    aStream << "Mean Time in AF" << "\t";
    aStream << "Cases in AF" << "\t";
    aStream << "STDDEV of Time in AF" << "\t";
    aStream << "STDERR of Time in AF" << "\n";
    aStream.flush();
    aStream.clear();
    return;
}

//function to write out data for excited cell count file
void FOutExCellsData(std::ofstream& aStream,  const std::vector<int>& exCellCount)
{
	int frame = 0;
	for (auto it=exCellCount.begin(); it!=exCellCount.end(); it++)
	{
	aStream << frame << "\t" << *it << "\n";
	frame++;
	}
	aStream.flush();
	aStream.clear();
	return;
}

//function to write out data for excited cell count stats file
void FOutExStatsData(std::ofstream& aStream, const std::vector<int>& exCellStats, const int& repeat, const int& MAXFRAME, const double& HorFiringProb, const double& VerFiringProb)
{
	aStream << repeat << "\t" << exCellStats[repeat] << "\t";
	double dblMAXFRAME = static_cast<double>(MAXFRAME);
	double timeInAF = exCellStats[repeat]/dblMAXFRAME;
	aStream << timeInAF << "\t";
	aStream << HorFiringProb << "\t";
	aStream << VerFiringProb << "\n";
	aStream.flush();
	aStream.clear();
	return;
}

//function to write out data for excited cell count master file
void FOutExMasterData(std::ofstream& aStream, std::vector<int>& exCellStats, const int& MAXFRAME, const double& nu)
{
	int AFCases = 0;
	std::vector<double> fractionInAF;
	for (auto it=exCellStats.begin(); it!=exCellStats.end(); it++)
	{
		if(*it > 0)
		AFCases++;

		fractionInAF.push_back(*it/(double)MAXFRAME);
	}

	aStream << nu << "\t";
	aStream << mean(exCellStats) << "\t";

	double m = mean(fractionInAF);
	aStream << m << "\t";
	aStream << AFCases << "\t";

	double sdev = standarddev(fractionInAF, m);
	aStream << sdev << "\t";

	aStream << sdev/sqrt((double)(fractionInAF.size())) << "\n";
	aStream.flush();
	aStream.clear();
	return;
}

//function to write out column headers for clean birth  stats file
void FOutCleanBirthColumns(std::ofstream& aStream)
{
	aStream << "xDist\tyDist\tDistance\tAngle" << std::endl;
	aStream.flush();
	aStream.clear();
	return;
}

//function to write out data for clean birth stats file
void FOutCleanBirthData(std::ofstream& aStream, const std::vector<std::pair<int,int> >& firstCleanBirthVec)
{
	for (auto it=firstCleanBirthVec.begin();it!=firstCleanBirthVec.end();
				it++)
				{	
					double xDist = it->first;
					double yDist = it->second;
					aStream << xDist << "\t";
					aStream << yDist << "\t";
					aStream << sqrt(xDist*xDist+yDist*yDist) << "\t";
					aStream << atan2(yDist,xDist) << std::endl;
				}
	aStream.flush();
	aStream.clear();
	return;
}

//function to write out columns for clean birth master file
void FOutCleanBirthMasterColumns(std::ofstream& aStream)
{
	aStream << "nu\trepeats\txDistMean\txDistSdev\tyDistMean\tyDistSdev\tDistanceMean\tDistanceSdev\tAngleMean\tAngleSdev" << std::endl;
	aStream.flush();
	aStream.clear();
	return;
}

//function to write out data for clean birth master file
void FOutCleanBirthMasterData(std::ofstream& aStream, const std::vector<std::pair<int,int> >& firstCleanBirthVec, double& nu)
{
	std::vector<int> xDistances;
	std::vector<int> yDistances;
	std::vector<double> angles;
	std::vector<double> distances;
	for (auto it=firstCleanBirthVec.begin();it!=firstCleanBirthVec.end(); it++)
	{
		double xDist = it->first;
		double yDist = it->second;
		xDistances.push_back(xDist);
		yDistances.push_back(it->second);
		angles.push_back(atan2(yDist,xDist));
		distances.push_back(sqrt(xDist*xDist+yDist*yDist));
	}
	double Mean = mean(xDistances);
	double sdev = standarddev(xDistances,Mean);
	aStream << nu << "\t";
	aStream << xDistances.size();
	aStream << Mean << "\t" << sdev << "\t";
	Mean = mean(yDistances);
	sdev = standarddev(yDistances,Mean);
	aStream << Mean << "\t" << sdev << "\t";
	Mean = mean(angles);
	sdev = standarddev(angles,Mean);
	aStream << Mean << "\t" << sdev << "\t";
	Mean = mean(distances);
	sdev = standarddev(distances,Mean);
	aStream << Mean << "\t" << sdev << std::endl;
}


//function to write out 3d histogram data
void FOut2DVector(std::ofstream& aStream, std::vector<std::vector<int> > vector2D)
{
    for(auto it = vector2D.begin(); it != vector2D.end(); ++it)
    {
        for(auto it1 = it->begin(); it1 != it->end(); ++it1) {aStream << *it1 << "\t";}
        aStream<<"\n";
    }
    
    aStream.flush();
    aStream.clear();
}
