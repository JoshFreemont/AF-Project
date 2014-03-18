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

double standarddev(std::vector<int>& someVector, double& m)
{
	double accum = 0.0;
	std::for_each (std::begin(someVector), std::end(someVector), [&](const double d)
		{
			accum += (d - m) * (d - m);
		});
	return sqrt(accum/someVector.size());
}

double standarddev(std::vector<double>& someVector, double& m)
{
	double accum = 0.0;
	std::for_each (std::begin(someVector), std::end(someVector), [&](const double d)
		{
			accum += (d - m) * (d - m);
		});
	return sqrt(accum/someVector.size());
}