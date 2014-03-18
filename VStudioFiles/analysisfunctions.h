#ifndef __ANALYSISFUNCTIONS__H__
#define __ANALYSISFUNCTIONS__H__
#include <vector>
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>

double mean(std::vector<int>& someVector);
double mean(std::vector<double>& someVector); //overloaded just in case
double standarddev(std::vector<int>& someVector, double& m);
double standarddev(std::vector<double>& someVector, double& m);

#endif //__ANALYSISFUNCTIONS__H__
