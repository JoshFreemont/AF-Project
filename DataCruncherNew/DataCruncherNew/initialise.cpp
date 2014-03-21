#include "initialise.h"
#include "mtrand.h"
#include <ctime>

void readOptionsFile(std::ifstream& opFile, optionsStruct& startOptions)
{
	std::string line;
	while(getline(opFile, line))
    {
        if(line[0] == '#') continue;
		else break;
	}
	getline(opFile, line);
	std::stringstream data;
	std::string dataPart;
	startOptions.m_FileHeader = line;
	bool isTrue;
    
	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_DETECTROTORS = isTrue;
    
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_COUNTEXCELLS = isTrue;
	
	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_DISPLAYFULLEXCELLS = isTrue;
	
	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_BIRTHPROBDIST = isTrue;
	
	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_BIRTHEXPECTATION = isTrue;
	
    
	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_STATICMODEL = isTrue;
	
    
	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_JOINTMODEL = isTrue;
	
    
	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_OUTPUTDEFECTLOC = isTrue;
	
    
	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> std::boolalpha >> isTrue;
	startOptions.m_DETECTCLEANBIRTH = isTrue;
	
    
	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> dataPart;
	startOptions.m_nuSTART = static_cast<double>(atof(dataPart.c_str()));
    
	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> dataPart;
	startOptions.m_nuMAX = static_cast<double>(atof(dataPart.c_str()));
    
	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> dataPart;
	startOptions.m_nuSTEP = static_cast<double>(atof(dataPart.c_str()));
    
	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> dataPart;
	startOptions.m_delta = static_cast<double>(atof(dataPart.c_str()));
    
	
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> dataPart;
	startOptions.m_epsilon = static_cast<double>(atof(dataPart.c_str()));
    
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> dataPart;
	startOptions.m_repeatMAX = atoi(dataPart.c_str());
	opFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	getline(opFile, line);
	opFile >> dataPart;
	startOptions.m_MAXFRAME = atoi(dataPart.c_str());
}


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




