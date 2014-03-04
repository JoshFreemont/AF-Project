#ifndef __FILENAMER__H__
#define __FILENAME__H__

#include <string>
#include <sstream>
#include <fstream>

class FileNamer
{
private:
    std::string m_FileHeader; //Header to be prepended
    std::ostringstream m_Convert; //Number to string conversion stream
public:
    FileNamer(const std::string& FileHeader="");

    void setFileHeader(const std::string& header);

    void RenameFile(std::ofstream& aStream, double nu_outside);
    //For Master files

    void RenameFile(std::ofstream& aStream, std::string middle, double nu, double nu_outside);
    //For files which are output per nu value
    //nu value is appended to the file name

    void RenameFile(std::ofstream& aStream, std::string middle, double nu, int iteration, double nu_outside);
    //For files which are output every iteration
    //Nu value and iteration number are appended to file name

};

#endif // __FILENAMER__H__
