#include "gpu-reader.h"

#include <fstream>
#include <iostream>
#include <cstdio>
#include <filesystem>


//Add namespace because it's tedious to write std::filesystem all the time
namespace fs = std::filesystem;


void GpuReader::printModel() const{
    std::cout<<"GPU: "<<modelName_<<std::endl;
}


//Print maximum permissive temperature (in Celsius) before the system shuts it down or starts enhanced cooling
void GpuReader::printMaxTemp() const {
    std::cout<<"Max GPU temperature: "<<maxTemp_<<"°C"<<std::endl;
}

void IntelGpuReader::readMaxTemp() {
    //Check every directory in serach for thermal_zone* directories
    for (const auto& entry : fs::directory_iterator("/sys/class/thermal")) {
        if (entry.is_directory()) {
            if (entry.path().string().find("thermal_zone") != std::string::npos) {
                std::string typePath = entry.path().string() + "/type";
                std::ifstream typeFile(typePath);
                if (!typeFile.is_open()) {
                    continue;
                }
                std::string type;
                typeFile >> type;
                //If the type matches Intel GPUs, check the thermal_zone directory for trip_point files
                if (type == "x86_pkg_temp") {
                    for (const auto& maxTempEntry : fs::directory_iterator(entry.path())) {
                        if (maxTempEntry.path().string().find("trip_point_") != std::string::npos
                        && maxTempEntry.path().string().find("temp") != std::string::npos) {
                            std::ifstream maxTempFile(maxTempEntry.path().string());
                            if (!maxTempFile.is_open()) {
                                continue;
                            }
                            int highestTemp = 0;
                            maxTempFile >> highestTemp;
                            //Divide by 1000 - Linux holds information in millidegrees
                            highestTemp/=1000;
                            if (highestTemp > maxTemp_) {
                                maxTemp_ = highestTemp;
                            }
                        }
                    }
                }
            }
        }
    }


    if (maxTemp_ <= 0) {
        //Fallback to check if there is actual data about max temp in hwmon (/sys/class/hwmon)
        for (const auto& entry : fs::directory_iterator("/sys/class/hwmon")) {
            if (entry.is_directory()) {
                std::string hwmonNamePath = entry.path().string() + "/name";
                std::ifstream hwmonNameFile(hwmonNamePath);

                if (!hwmonNameFile.is_open()) {
                    continue;
                }
                std::string name;
                hwmonNameFile >> name;
                if (name == "coretemp") {
                    for (const auto& maxTempEntry : fs::directory_iterator(entry.path())) {
                        if (maxTempEntry.path().string().find("temp") != std::string::npos
                        && maxTempEntry.path().string().find("_crit") != std::string::npos
                        && maxTempEntry.path().string().find("_alarm") == std::string::npos) {
                            std::ifstream maxTempFile(maxTempEntry.path().string());
                            if (!maxTempFile.is_open()) {
                                continue;
                            }
                            int highestTemp = 0;
                            maxTempFile >> highestTemp;
                            highestTemp/=1000;
                            if (highestTemp > maxTemp_) {
                                maxTemp_ = highestTemp;
                            }
                        }
                    }
                }
            }
        }
    }

    //Final fallback - if no info found, set maxTemp_ to default max Intel GPU temperature
    if (maxTemp_ == 0) {
        maxTemp_ = 100;
    }
}


void AMDGpuReader::readMaxTemp() {
    return;
}

void NVIDIAGpuReader::readMaxTemp() {
    return;
}


//The only purpose of GpuFactory - a static method that returns appropriate GPU class.
GpuReader* GpuFactory::createGpuReader() {
    FILE* pipe = popen("lspci | grep -i \"vga\"", "r");
    if (!pipe) {
        std::cerr<<"Couldn't create a pipe to lspci! "<<std::endl;
        return nullptr;
    }

    char buffer[256];
    std::string temp;
    while (fgets(buffer, sizeof(buffer), pipe)) {
        temp += buffer;
    }
    pclose(pipe);

    size_t index = temp.find(": ");
    std::string modelName;

    if (index != std::string::npos) {
        modelName = temp.substr(index + 2);
        if (modelName.back() == '\n') {
            modelName.pop_back();
        }
    }

    if (modelName.find("Intel") != std::string::npos) {
        return new IntelGpuReader(modelName);
    }
    if (modelName.find("NVIDIA") != std::string::npos) {
        return new NVIDIAGpuReader(modelName);
    }
    if (modelName.find("AMD") != std::string::npos) {
        return new AMDGpuReader(modelName);
    }
    return nullptr;
}