//
//  DataOutput.h
//  DataCruncherNew
//
//  Created by josh freemont on 06/03/2014.
//  Copyright (c) 2014 com.example. All rights reserved.
//

#ifndef __DataCruncherNew__DataOutput__
#define __DataCruncherNew__DataOutput__

#include <iostream>
#include <fstream>
#include <string>
#include "rotorIDstruct.h"
#include "analysisfunctions.h"
#include "array2D.h"
#include "main_functions.h"

//function to output rotor Id data
void FOutRotorIdData(std::ofstream& aStream, std::vector<struct rotorIDstruct> &rotorIDdata);

//set columns for RotorIdStream.
void FOutRotorIDColumns(std::ofstream& aStream);

//function to output rotor excited count data
void FOutRotorExCountData(std::ofstream& aStream, array2D<int>& rotorCellFrequency);

//function to "cout" the current status of an experiment.
void COutCurrentStatus(const int& TotalIterations, const int& CurrentIteration);

//function to output y vs. x
void FOutXvsY(std::ofstream& aStream, const int& x, const int& y);

//overload FOutXvsY for vector input
void FOutXvsY(std::ofstream& aStream, std::vector<int>& data);

//function to write out columns for excited cell count file
void FOutExCellsColumns(std::ofstream& aStream);

//function to write out columns for excited cell count stats file
void FOutExStatsColumns(std::ofstream& aStream);

//function to write out columns for excited cell count master file
void FOutExMasterColumns(std::ofstream& aStream);

//function to write out data for excited cell count file
void FOutExCellsData(std::ofstream& aStream,  const std::vector<int>& exCellCount);

//function to write out data for excited cell count stats file
void FOutExStatsData(std::ofstream& aStream, const std::vector<int>& exCellStats, const int& repeat, const int& MAXFRAME, const double& HorFiringProb, const double& VerFiringProb);

//function to write out data for excited cell count master file
void FOutExMasterData(std::ofstream& aStream, std::vector<int>& exCellStats, const int& MAXFRAME, const double& nu);


#endif /* defined(__DataCruncherNew__DataOutput__) */
