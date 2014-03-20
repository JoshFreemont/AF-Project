//
//  excited_scan_algorithms.cpp
//  sdltest
//
//  Created by josh freemont on 17/11/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//

#include "main_functions.h"
#include <ctime>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iostream>


void pacemaker(array2D<int> &state_update, std::vector<int> &all_excited_coords, const int& RP, const int& GRIDSIZE, array2D<std::pair<int,int>> &excitedBy, int& exCells)
{
    for(int j=0; j<GRIDSIZE; ++j)
        if(state_update(0,j)==0)
        {
            state_update(0,j)=RP;
            excitedBy(0,j)=std::make_pair (0,j);
            all_excited_coords.push_back(0);//push back i coord in same coordinate definition as the state array.
            all_excited_coords.push_back(j);//push back j coord as above
			exCells++;
        }
    return;
}

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}

std::vector<int> getBirthDataVect(std::vector<rotorIDstruct>& rotorIdData)
{
    std::vector<int> dataVector (rotorIdData.size());
    int index = 0;
    for(auto it = rotorIdData.begin(); it != rotorIdData.end(); it++)
    {
        dataVector[index] = it->birthframe;
        index++;
    }
    return dataVector;
}

std::vector<int> getDeathDataVect(std::vector<rotorIDstruct>& rotorIdData, const int MAXFRAME)
{
    std::vector<int> dataVector (rotorIdData.size());
    int index = 0;
    for(auto it = rotorIdData.begin(); it != rotorIdData.end(); it++)
    {
        //if death is enforced by end of experiment then continue.
        if( it->lifetime + it->birthframe == MAXFRAME+1) continue;
        else dataVector[index] = it->birthframe + it->lifetime;
        index++;
    }
    
    //correct for deaths which are enforced by the end of experiment
    return dataVector;
}

void readOptionsFile(std::ifstream& opFile, optionsStruct& startOptions)
{
	std::string line;
	while(getline(opFile, line)) 
    {
        if(line[0] == '#') continue;
		else break;
	}
	getline(opFile, line);
	std::stringstream data;
	std::string dataPart;
	startOptions.m_FileHeader = line;
	bool isTrue;

	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_DETECTROTORS = isTrue;

	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_COUNTEXCELLS = isTrue;
	
	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_DISPLAYFULLEXCELLS = isTrue;
	
	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_BIRTHPROBDIST = isTrue;
	
	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_BIRTHEXPECTATION = isTrue;
	

	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_STATICMODEL = isTrue;
	

	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_JOINTMODEL = isTrue;
	

	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_OUTPUTDEFECTLOC = isTrue;
	

	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_DETECTCLEANBIRTH = isTrue;
	

	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> dataPart;
	startOptions.m_nuSTART = static_cast<double>(atof(dataPart.c_str()));

	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> dataPart;
	startOptions.m_nuMAX = static_cast<double>(atof(dataPart.c_str()));

	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> dataPart;
	startOptions.m_nuSTEP = static_cast<double>(atof(dataPart.c_str()));

	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> dataPart;
	startOptions.m_delta = static_cast<double>(atof(dataPart.c_str()));

	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> dataPart;
	startOptions.m_epsilon = static_cast<double>(atof(dataPart.c_str()));

	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> dataPart;
	startOptions.m_repeatMAX = atoi(dataPart.c_str());
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> dataPart;
	startOptions.m_MAXFRAME = atoi(dataPart.c_str());
}