#pragma once

#include <string>
#include <cstdint>
#include <utility>
#include <vector>

class ProcessReader{
private:
    struct Process{
        std::string name;
        uint32_t PID;
        uint64_t ramUsed;

        Process(std::string& name, const uint32_t PID, const uint64_t ramUsed)
        : name(std::move(name)), PID(PID), ramUsed(ramUsed){};
    };
    std::vector<Process> processes;

public:
    void readProcesses();
    void printProcesses() const;
};