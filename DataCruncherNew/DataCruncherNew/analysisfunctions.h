#ifndef __ANALYSISFUNCTIONS__H__
#define __ANALYSISFUNCTIONS__H__
#include <vector>
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>

double mean(std::vector<int>& someVector);
double mean(std::vector<double>& someVector); //overloaded just in case
double stddev(std::vector<int>& someVector, double mean);
double stddev(std::vector<double>& someVector, double mean);

#endif //__ANALYSISFUNCTIONS__H__
