#include "gpu-reader.h"
#include "nvml.h"

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


void GpuReader::printCurrTemp() const {
    std::cout<<"Current GPU temperature: "<<currTemp_<<"°C"<<std::endl;
}


void IntelGpuReader::readMaxTemp() {
    //Checking every directory in hwmon directory to find Intel made GPUs.
    for (const auto& entry : fs::directory_iterator("/sys/class/hwmon")) {
        if (entry.is_directory()) {
            std::string hwmonNamePath = entry.path().string() + "/name";
            std::ifstream hwmonNameFile(hwmonNamePath);

            if (!hwmonNameFile.is_open()) {
                continue;
            }
            std::string name;
            hwmonNameFile >> name;
            if (name == "i915" || name == "xe") {
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

    //Fallback 1 - if nothing found, check for name file equal to "coretemp"
    if (maxTemp_ <= 0){
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


    //Fallback 2 - check every directory in search for thermal_zone* directory with type of x86_pkg_temp
    // if nothing found in hwmon.
    if (maxTemp_ <= 0){
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
    }

    //Final fallback - if no info found, set maxTemp_ to default max Intel GPU temperature
    if (maxTemp_ <= 0) {
        maxTemp_ = 100;
    }
}


void IntelGpuReader::readCurrTemp() {
    for (const auto& entry : fs::directory_iterator("/sys/class/hwmon")) {
        if (entry.is_directory()) {
            std::string hwmonNamePath = entry.path().string() + "/name";
            std::ifstream hwmonNameFile(hwmonNamePath);
            if (!hwmonNameFile.is_open()) {
                continue;
            }

            std::string name;
            hwmonNameFile >> name;

            if (name == "i915" || name == "xe") {
                int highestTemperature = 0;
                for (const auto& tempEntry : fs::directory_iterator(entry.path())) {
                    if (tempEntry.path().string().find("temp") != std::string::npos
                    && tempEntry.path().string().find("_input") != std::string::npos) {
                        std::ifstream tempFile(tempEntry.path().string());
                        if (!tempFile.is_open()) {
                            continue;
                        }
                        int temperature = 0;
                        tempFile >> temperature;
                        temperature/=1000;
                        //Searching for highest temperature sensor on the GPU
                        if (temperature > highestTemperature) {
                            highestTemperature = temperature;
                        }
                    }
                }
                currTemp_ = highestTemperature;
                return;
            }
        }
    }

    //Fallback 1 - if there wasn't a hwmon folder with name "i915" nor "xe" search for "coretemp"
    //because the GPU is probably integrated with CPU.
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
                int highestTemperature = 0;
                for (const auto& labelEntry : fs::directory_iterator(entry.path())) {
                    if (labelEntry.path().string().find("temp") != std::string::npos
                    && labelEntry.path().string().find("_label") != std::string::npos) {

                        std::ifstream labelFile(labelEntry.path().string());
                        if (!labelFile.is_open()) {
                            continue;
                        }

                        std::string label;
                        std::getline(labelFile, label);
                        if (label.find("Package id") != std::string::npos) {
                            std::string tempPath = labelEntry.path().string();
                            size_t index = tempPath.find("_label");
                            tempPath.replace(index, 6, "_input");

                            std::ifstream tempFile(tempPath);
                            if (!tempFile.is_open()) {
                                continue;
                            }
                            int temperature = 0;
                            tempFile >> temperature;
                            temperature/=1000;
                            if (temperature > highestTemperature) {
                                highestTemperature = temperature;
                            }
                        }
                    }
                }
                currTemp_ = highestTemperature;
                return;
            }
        }

    if (currTemp_ <= 0) {
        currTemp_ = -273;
    }}
}


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

}


void NVIDIAGpuReader::readMaxTemp() {
    //Using nvml api to retrieve the information
    nvmlReturn_t initType = nvmlInit();
    if (initType != NVML_SUCCESS) {
        std::cerr<<"Unable to initialize nvml library!"<<std::endl;
        return;
    }

    unsigned int deviceCount;
    nvmlDevice_t gpu;
    unsigned int temp;

    if (nvmlDeviceGetCount(&deviceCount) != NVML_SUCCESS || deviceCount == 0) {
        std::cerr<<"Unable to retrieve the device count!"<<std::endl;
        nvmlShutdown();
        return;
    }

    if (nvmlDeviceGetHandleByIndex(0, &gpu) != NVML_SUCCESS) {
        std::cerr<<"Unable to retrieve the GPU handle!"<<std::endl;
        nvmlShutdown();
        return;
    }

    if (nvmlDeviceGetTemperatureThreshold(gpu, NVML_TEMPERATURE_THRESHOLD_SHUTDOWN, &temp) != NVML_SUCCESS) {
        std::cerr<<"Unable to retrieve max temperature threshold for GPU!"<<std::endl;
        nvmlShutdown();
        return;
    }

    //Clang-Tidy requirement
    maxTemp_ = static_cast<int>(temp);
    nvmlShutdown();
}


void NVIDIAGpuReader::readCurrTemp() {

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