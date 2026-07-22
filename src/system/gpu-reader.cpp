#include "gpu-reader.h"

#include <fstream>
#include <cstring>
#include <iostream>
#include <cstdio>

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

void GpuReader::readMaxTemp() {

}

void GpuReader::printModel() const{
    std::cout<<"GPU: "<<modelName_<<std::endl;
}

void GpuReader::printMaxTemp() const {
    std::cout<<maxTemp_<<std::endl;
}
