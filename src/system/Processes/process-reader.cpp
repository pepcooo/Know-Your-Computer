#include "process-reader.h"

#include <iostream>
#include <fstream>
#include <cstdint>
#include <filesystem>

namespace fs = std::filesystem;


void ProcessReader::printProcesses() const{
    for (const auto& process : processes){
        std::cout<<process.name<<": "<<process.PID<<", "<<process.PPID<<", "<<getProcessName(process.state)<<", "
        <<process.ramUsed<<" MB"<<std::endl;
    }
}


void ProcessReader::readProcesses(){
    for (const auto& process : fs::directory_iterator("/proc")){
        if (process.is_directory() && process.path().filename().string().find_first_not_of("0123456789") == std::string::npos){
            std::ifstream ifs(process.path().string() + "/status");
            if (!ifs.is_open()){
                std::cerr<<"Couldn't open process file with PID "<<process.path().filename().string()<<std::endl;
                continue;
            }
            std::string line;
            std::string name = "Unknown";
            uint32_t PID = 0;
            uint32_t PPID = 0;
            uint64_t ramUsed = 0;
            char state = 'U';
            while (std::getline(ifs, line)){
                if (line.find("Name:") != std::string::npos){
                    size_t index = line.find(':');
                    name = line.substr(index + 2);
                }
                else if (line.find("Pid:") == 0){
                    size_t index = line.find(':');
                    std::string pidStr = line.substr(index + 2);
                    PID = std::stoi(pidStr);
                }
                else if (line.find("PPid:") == 0){
                    size_t index = line.find(':');
                    std::string ppidStr = line.substr(index + 2);
                    PPID = std::stoi(ppidStr);
                }
                else if (line.find("VmRSS:") != std::string::npos){
                    size_t index = line.find(':');
                    std::string ramStr = line.substr(index + 2);
                    ramUsed = std::stoi(ramStr)/1024;
                }
                else if (line.find("State:") != std::string::npos){
                    size_t index = line.find(':');
                    std::string stateStr = line.substr(index + 2);
                    state = stateStr[0];
                }
            }
            processes.push_back(Process(name, PID, PPID, ramUsed, state));
        }
    }
}