#ifndef __INITIALISE__H__
#define __INITIALISE__H__
#include <vector>
#include "array2D.h"
#include "optionsStruct.h"
#include <string>
#include <sstream>
#include <fstream>

void readOptionsFile(std::ifstream& opFile, optionsStruct& startOptions);


void outputDefects(array2D<double>& inE, std::ofstream& aStream);

void outputVerConn(array2D<double>& inS, std::ofstream& aStream);

void initStaticDefects (array2D<double>& inW, array2D<double>& inE, const double& delta, const double& espilon);

void initStaticVerts (array2D<double>& inW, array2D<double>& inE, array2D<double>& inN, array2D<double>& inS, const double& nu, const int& GRIDSIZE);



#endif //__INITIALISE__H__