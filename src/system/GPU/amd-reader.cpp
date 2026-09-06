#include "amd-reader.h"


#include <filesystem>
#include <iostream>
#include <ostream>
#include <fstream>


namespace fs = std::filesystem;


void AMDGpuReader::readMaxTemp() {
    for (const auto& entry : fs::directory_iterator("/sys/class/hwmon")) {
        if (entry.is_directory()) {
            std::string namePath = entry.path().string() + "/name";
            std::ifstream nameFile(namePath);
            if (!nameFile) {
                std::cerr<<"Couldn't open the name file for AMD based GPU!"<<std::endl;
                continue;
            }

            std::string type;
            nameFile >> type;
            if (type == "amdgpu") {
                for (const auto& tempEntry : fs::directory_iterator(entry.path().string())) {
                    if (tempEntry.path().string().find("temp") != std::string::npos
                    && tempEntry.path().string().find("_crit") != std::string::npos) {
                        int highestTemp = 0;
                        std::string tempPath = tempEntry.path().string();
                        std::ifstream  tempFile(tempPath);
                        if (!tempFile) {
                            std::cerr<<"Couldn't open the temperature file for AMD based GPU!"<<std::endl;
                            continue;
                        }

                        tempFile >> highestTemp;
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


void AMDGpuReader::readCurrTemp() {
    int highestTemperature = 0;
    for (const auto& entry : fs::directory_iterator("/sys/class/hwmon")) {
        if (entry.is_directory()) {
            std::string namePath = entry.path().string() + "/name";
            std::ifstream nameFile(namePath);
            if (!nameFile.is_open()) {
                continue;
            }
            std::string type;
            nameFile >> type;
            if (type == "amdgpu") {
                for (const auto& tempEntry : fs::directory_iterator(entry.path().string())) {
                    std::string tempPath = tempEntry.path().string();
                    if (tempPath.find("temp") != std::string::npos
                    && tempPath.find("_input") != std::string::npos) {
                        int temperature = 0;
                        std::ifstream tempFile(tempEntry.path().string());
                        if (!tempFile.is_open()) {
                            continue;
                        }
                        tempFile >> temperature;
                        temperature/=1000;
                        if (temperature > highestTemperature) {
                            highestTemperature = temperature;
                        }
                    }
                }
            }
        }
    }
    currTemp_ = highestTemperature;
}


void AMDGpuReader::readVRAM() {
    for (auto& entry : fs::directory_iterator("/sys/class/drm")){
        if (entry.is_directory() && entry.path().string().find("card") != std::string::npos) {
            std::string namePath = entry.path().string() + "/device/vendor";
            std::ifstream nameFile(namePath);
            if (!nameFile.is_open()) {
                continue;
            }
            std::string vendor;
            std::getline(nameFile, vendor);
            if (vendor.find("0x1002") != std::string::npos){
                long long temp;
                std::ifstream vramTotal(entry.path().string() + "/device/mem_info_vram_total");
                if (!vramTotal.is_open()){
                    vram_.total = -1;
                }
                else{
                    vramTotal >> temp;
                    temp/=1024*1024;
                    vram_.total = temp;
                }

                std::ifstream vramUsed(entry.path().string() + "/device/mem_info_vram_used");
                if (!vramUsed.is_open()){
                    vram_.used = -1 ;
                }
                else{
                    vramUsed >> temp;
                    temp/=1024*1024;
                    vram_.used = temp;
                }
                return;
            }
        }
    }
}

