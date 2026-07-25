#include "gpu-reader.h"

#include <fstream>
#include <iostream>
#include <cstdio>


void GpuReader::printModel() const{
    std::cout<<"GPU: "<<modelName_<<std::endl;
}

void GpuReader::printMaxTemp() const {
    std::cout<<"Max temperature: "<<maxTemp_<<std::endl;
}

void IntelGpuReader::readMaxTemp() {
    return;
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