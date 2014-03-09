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

//function to output rotor count vs. frame
void FOutFrameVsVar(std::ofstream& aStream, const int& Var, const int& FRAME);


#endif /* defined(__DataCruncherNew__DataOutput__) */
