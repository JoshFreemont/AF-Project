#ifndef __INITIALISE__H__
#define __INITIALISE__H__
#include <vector>
#include "array2D.h"
#include <fstream>

void outputDefects(array2D<double>& inE, std::ofstream& aStream);
void outputVerConn(array2D<double>& inS, std::ofstream& aStream);
void initStaticDefects (array2D<double>& inW, array2D<double>& inE, double& delta, double& epsilon);
void initStaticVerts (array2D<double>& inW, array2D<double>& inE, array2D<double>& inN, array2D<double>& inS, double& nu);

#endif //__INITIALISE__H__