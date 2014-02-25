#include "filenamer.h"
#include <iomanip>



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
