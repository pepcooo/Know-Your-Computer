#include "gpu-reader.h"

#include <fstream>
#include <cstring>
#include <iostream>
#include <cstdio>


/*
void GpuReader::readModel() {
    FILE* pipe = popen("lspci | grep -i \"vga\"", "r");
    if (!pipe) {
        std::cerr<<"Couldn't create a pipe to \"lspci -vga\"!"<<std::endl;
        return;
    }

    char buffer[256];
    std::string tempModelName;
    while(fgets(buffer, sizeof(buffer), pipe)) {
        tempModelName += buffer;
    }
    size_t  index = tempModelName.find(": ");
    if (index != std::string::npos) {
        modelName_ = tempModelName.substr(index + 2);
    }

    pclose(pipe);
}
*/

void GpuReader::printModel() const{
    std::cout<<"GPU: "<<modelName_<<std::endl;
}

void IntelReader::readMaxTemp() {
    return;
}

void AMDReader::readMaxTemp() {
    return;
}

void NVIDIAReader::readMaxTemp() {
    return;
}

void GpuReader::printMaxTemp() const {
    std::cout<<"Max temperature: "<<maxTemp_<<std::endl;
}

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
        return new IntelReader(modelName);
    }
    if (modelName.find("NVIDIA") != std::string::npos) {
        return new NVIDIAReader(modelName);
    }
    if (modelName.find("AMD") != std::string::npos) {
        return new AMDReader(modelName);
    }
    return nullptr;
}