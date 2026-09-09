#pragma once

#include <string>
#include <cstdint>
#include <utility>
#include <vector>

class ProcessReader{
private:
    [[nodiscard]] static constexpr std::string_view getProcessName(const char state){
        switch (state){
            case 'R': return "Running";
            case 'I': return "Idle";
            case 'S': return "Sleeping";
            case 'X': return "Dead";
            case 'T': return "Stopped";
            case 't': return "Tracing stop";
            case 'D': return "Disk sleep";
            case 'Z': return "Zombie";
            default: return "Unknown";
        }
    }
    struct Process{
        std::string name;
        uint32_t PID;
        uint32_t PPID;
        uint64_t ramUsed;
        char state;

        Process(std::string& name, const uint32_t PID, const uint32_t PPID, const uint64_t ramUsed, const char state)
        : name(std::move(name)), PID(PID), PPID(PPID), ramUsed(ramUsed), state(state){};
    };
    std::vector<Process> processes;

public:
    void readProcesses();
    void printProcesses() const;
};