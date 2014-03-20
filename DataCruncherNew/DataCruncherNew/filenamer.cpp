#include "filenamer.h"
#include <iomanip>
#include <iostream>

using namespace std;

FileNamer::FileNamer(const string& FileHeader): m_FileHeader(FileHeader)
    {}

    void FileNamer::setFileHeader(const string& header)
    {
        m_FileHeader = header;
    }

    void FileNamer::RenameFile(ofstream& aStream, double nu_outside)
    //For Master files
    {
        string fileName;
        m_Convert.clear();
        m_Convert.str("");
        //Clear number to string conversion stream
        aStream.close();
        m_Convert << setprecision(4) << "out" << nu_outside;
        fileName =
        m_FileHeader + m_Convert.str() + "_" + "master.txt";; //Name file automatically
        aStream.open(fileName.c_str());
    }

    void FileNamer::RenameFile(ofstream& aStream, string middle, double nu_inside, double nu_outside)
    //For files which are output per nu value
    //nu value is appended to the file name
    {
        string fileName;
        m_Convert.clear();
        m_Convert.str("");
        aStream.close();
        m_Convert << setprecision(4) << "out" << nu_outside << "_" << nu_inside;
        fileName =
        m_FileHeader + "_" + middle + "_" + m_Convert.str() + ".txt";
        //Name file automatically

        aStream.open(fileName.c_str());
    }

    void FileNamer::RenameFile(ofstream& aStream, string middle,
                               double nu, int iteration, double nu_outside)
    //For files which are output every iteration
    //Nu value and iteration number are appended to file name
    {
        string fileName;
        m_Convert.clear();
        m_Convert.str("");
        aStream.close();

        m_Convert << setprecision(4) << "out" << nu_outside << "_" << nu << "_" << iteration;
        fileName =
        m_FileHeader + "_" + middle + "_" + m_Convert.str() + ".txt";

        aStream.open(fileName.c_str());
    }


    void FileNamer::IDFile(std::ofstream& aStream, double nu, double iteration, double rotorIDthreshold)
    {
        string fileName;
        m_Convert.clear();
        m_Convert.str("");
        aStream.close();

        m_Convert << setprecision(4) << "ID_nu_" << nu << "_IDThresh" << rotorIDthreshold << "_" << iteration;
        fileName =
        m_FileHeader + "_" + m_Convert.str() + ".txt";

        aStream.open(fileName.c_str());

    }

    void FileNamer::RotorExCountFile(std::ofstream& aStream, double nu, double iteration, double rotorIDthreshold)
    {
        string fileName;
        m_Convert.clear();
        m_Convert.str("");
        aStream.close();

        m_Convert << setprecision(4) << "rtrcellcount_nu_" << nu << "_IDThresh" << rotorIDthreshold << "_" << iteration;
        fileName =
        m_FileHeader + "_" + m_Convert.str() + ".txt";

        aStream.open(fileName.c_str());
    }

    void FileNamer::RotorCountFile(std::ofstream& aStream, double nu, double iteration, double rotorIDthreshold)
    {
        string fileName;
        m_Convert.clear();
        m_Convert.str("");
        aStream.close();

        m_Convert << setprecision(4) << "rotorcount_nu_" << nu << "_IDThresh" << rotorIDthreshold << "_" << iteration;
        fileName =
        m_FileHeader + "_" + m_Convert.str() + ".txt";

        aStream.open(fileName.c_str());
	}

    void FileNamer::EdgeList(std::ofstream& aStream, double nu, double iteration, double rotorIDThreshold, std::string type)
	{
		string fileName;
		m_Convert.clear();
		m_Convert.str("");
		aStream.close();

		m_Convert << setprecision(4) << "edge_nu_" << nu << "_IDThresh" << rotorIDThreshold << "_" << iteration;

		fileName =m_FileHeader + "_" + m_Convert.str() + "_" + type + ".txt";

		aStream.open(fileName.c_str());
    }

	void FileNamer::LocGrid(std::ofstream& aStream, double nu, double iteration, double rotorIDThreshold, std::string type)
		{
		string fileName;
		m_Convert.clear();
		m_Convert.str("");
		aStream.close();

		m_Convert << setprecision(4) << "loc_nu_" << nu << "_Thr" << rotorIDThreshold << "_" << iteration;

		fileName =m_FileHeader + "_" + m_Convert.str() + "_" + type + ".txt";

		aStream.open(fileName.c_str());
    }

    void FileNamer::HistoFile(std::ofstream& aStream, double nu, double iteration, double rotorIDThreshold, std::string type)
    {
        string fileName;
        m_Convert.clear();
        m_Convert.str("");
        aStream.close();

        m_Convert << setprecision(4) << nu << "_IDThresh" << rotorIDThreshold << "_" << iteration;

        fileName =m_FileHeader + "_" + m_Convert.str() + "_" + type + "histo" + ".txt";

        aStream.open(fileName.c_str());
    }

	void FileNamer::ExCountFile(std::ofstream& aStream, double nu, double iteration, double rotorIDThreshold)
	{
		string fileName;
        m_Convert.clear();
        m_Convert.str("");
        aStream.close();

        m_Convert << setprecision(4) << nu << "_Thr" << rotorIDThreshold << "_" << iteration;

        fileName =m_FileHeader + "_exCells" + m_Convert.str() + ".txt";

        aStream.open(fileName.c_str());
    }

	void FileNamer::ExStatsFile(std::ofstream& aStream, double nu, double rotorIDThreshold)
	{
		string fileName;
        m_Convert.clear();
        m_Convert.str("");
        aStream.close();

        m_Convert << setprecision(4) << nu << "_Thr" << rotorIDThreshold;

        fileName =m_FileHeader + "_exStats" + m_Convert.str() + ".txt";

        aStream.open(fileName.c_str());
	}

	void FileNamer::ExMasterFile(std::ofstream& aStream, double rotorIDThreshold)
	{
		string fileName;
        m_Convert.clear();
        m_Convert.str("");
        aStream.close();

        m_Convert << "_Thr" << rotorIDThreshold;

        fileName =m_FileHeader + "_exMaster" + m_Convert.str() + ".txt";

        aStream.open(fileName.c_str());
	}

void FileNamer::XYFile(std::ofstream& aStream, double nu, double iteration, double rotorIDThreshold, std::string type)
{
    string fileName;
    m_Convert.clear();
    m_Convert.str("");
    aStream.close();
    
    m_Convert << setprecision(4) << "XY_nu" << nu << "_IDThresh" << rotorIDThreshold << "_" << iteration;
    
    fileName =m_FileHeader + "_" + m_Convert.str() + "_" + type + ".txt";
    
    aStream.open(fileName.c_str());
}

void FileNamer::CleanBirthFile(std::ofstream& aStream, double& nu, double& rotorIDThreshold)
    {
        string fileName;
        m_Convert.clear();
        m_Convert.str("");
        aStream.close();
    
        m_Convert << setprecision(4) << nu << "_IDThr" << rotorIDThreshold;
    
        fileName =m_FileHeader + "_" + m_Convert.str() + "_" + "firstbirth" + ".txt";
    
        aStream.open(fileName.c_str());
    }
	
	void FileNamer::CleanBirthMasterFile(std::ofstream& aStream, double& rotorIDThreshold)
    {
        string fileName;
        m_Convert.clear();
        m_Convert.str("");
        aStream.close();

		m_Convert << setprecision(4) << "_IDThr" << rotorIDThreshold;
    
        fileName =m_FileHeader + "_" + m_Convert.str() + "_firstbirthmaster" + ".txt";
    
        aStream.open(fileName.c_str());
    }