#include "proc-reader.h"

#include <fstream>
#include <string>
#include <iostream>


//Reading a CPU model by opening /proc/cpuinfo and looking for model name : line.
void ProcReader::readModel()
{
    std::ifstream ifs("/proc/cpuinfo");
    if (!ifs.is_open())
    {
        std::cerr << "Cannot open /proc/cpuinfo!" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(ifs, line))
    {
        if (line.find("model name") != std::string::npos)
        {
            size_t index = line.find(':');
            modelName_ = line.substr(index + 2);
            break;
        }
    }
    if (modelName_ == "Unknown")
    {
        std::cerr << "Unknown CPU model name" <<std::endl;
    }
    ifs.close();
}

void ProcReader::printModel() const
{
    std::cout<<"CPU: "<<modelName_<<std::endl;
}

