#ifndef __ANALYSISFUNCTIONS__H__
#define __ANALYSISFUNCTIONS__H__
#include <vector>

double mean(std::vector<int>& someVector);
double mean(std::vector<double>& someVector); //overloaded just in case

double stddev(std::vector<int>& someVector, double mean);
double stddev(std::vector<double>& someVector, double mean);

#endif //__ANALYSISFUNCTIONS__H__
