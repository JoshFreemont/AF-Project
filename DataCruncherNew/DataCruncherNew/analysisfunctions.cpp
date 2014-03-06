#include "analysisfunctions.h"

double mean(std::vector<int>& someVector)
{
	double sum = std::accumulate(std::begin(someVector), std::end(someVector), 0.0);
	return sum/(someVector.size());
}
double mean(std::vector<double>& someVector) //overloaded just in case
{
	double sum = std::accumulate(std::begin(someVector), std::end(someVector), 0.0);
	return sum/(someVector.size());
}

double stddev(std::vector<int>& someVector, double& mean)
{
	double accum = 0.0;
	std::for_each (std::begin(someVector), std::end(someVector), [&](const double d)
		{
			accum += (d - mean) * (d - mean);
		});
	return sqrt(accum/someVector.size());
}

double stddev(std::vector<double>& someVector, double& mean)
{
	double accum = 0.0;
	std::for_each (std::begin(someVector), std::end(someVector), [&](const double d)
		{
			accum += (d - mean) * (d - mean);
		});
	return sqrt(accum/someVector.size());
}