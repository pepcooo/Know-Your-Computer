#include "gpu-reader.h"
#include "intel-reader.h"
#include "amd-reader.h"
#include "nvidia-reader.h"

#include <iostream>
#include <cstdio>


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


void GpuReader::printVRAM() const {
    std::cout<<"Total VRAM: "<<vram_.total<<std::endl;
    std::cout<<"Free VRAM: "<<vram_.free<<std::endl;
    std::cout<<"Used VRAM: "<<vram_.used<<std::endl;
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