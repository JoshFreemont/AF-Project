#include "initialise.h"
#include "mtrand.h"
#include <ctime>

//Outputs defect locations to file
void outputDefects(array2D<double>& inE, std::ofstream& aStream)
{
	for (int j=0; j< inE.m_Width; j++)
	{
		for (int i=0; i<inE.m_Height; i++)
		{
			aStream << (inE(i,j)!=1.0? 1 : 0) << "\t";
		}
		aStream << std::endl;
	}
}

//Outputs (South) Vertical Connections to file 
void outputVerConn(array2D<double>& inS, std::ofstream& aStream)
{
	for (int j=0; j< inS.m_Height; j++)
	{
		for (int i=0; i<inS.m_Width; i++)
		{
			aStream << (inS(i,j)>0? 1 : 0) << "\t";
		}
		aStream << std::endl;
	}
}

void initStaticDefects (array2D<double>& inW, array2D<double>& inE, const double& delta, const double& epsilon)
{
	MTRand drand(time(NULL));

	for(int i = 0; i<inW.m_Width; ++i)
	{
		for(int j = 0; j<inW.m_Height; ++j)
		{
			if(drand() <= delta)
			{
				inW(i,j) = 1.0 - epsilon;
				inE(i,j) = 1.0 - epsilon;
			}

			else
			{
				inW(i,j) = 1.0;
				inE(i,j) = 1.0;
			}
		}
	}
}


void initStaticVerts (array2D<double>& inW, array2D<double>& inE, array2D<double>& inN, array2D<double>& inS, const double& nu, const int& GRIDSIZE)
{
	MTRand drand(time(NULL));
	double defectFactor;
	for(int i = 0; i<inW.m_Width; ++i)
	{
		for(int j = 0; j<inW.m_Height; ++j)
		{
			defectFactor = inW(i,j);
			if(drand() <= nu)
			{
				inS(i,j) = defectFactor;
				inN(i,(j+1)%GRIDSIZE) = defectFactor;
			}

			else
			{
				inS(i,j) = 0.0;
				inN(i,(j+1)%GRIDSIZE) = 0.0;
			}
		}
	}
}

