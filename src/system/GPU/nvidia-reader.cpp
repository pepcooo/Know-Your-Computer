#include "nvidia-reader.h"


#include <dlfcn.h>

#include <iostream>


NVIDIAGpuReader::NVIDIAGpuReader(const std::string& modelName)
: GpuReader(modelName){
    nvmlLib = dlopen("libnvidia-ml.so.1", RTLD_NOW);
    if (!nvmlLib) {
        std::cerr<<"Couldn't load libnvidia-ml.so.1 library!"<<std::endl;
        return;
    }

    auto initFunc = (nvmlInit_t)dlsym(nvmlLib, ("nvmlInit_v2"));
    if (!initFunc) {
        initFunc = (nvmlInit_t)dlsym(nvmlLib, ("nvmlInit"));
    }

    auto getDeviceCount = (nvmlDeviceGetCount_t)dlsym(nvmlLib, "nvmlDeviceGetCount");
    auto getHandleFunc = (nvmlDeviceGetHandleByIndex_t)dlsym(nvmlLib, "nvmlDeviceGetHandleByIndex_v2");
    if (!getHandleFunc) {
        getHandleFunc = (nvmlDeviceGetHandleByIndex_t)dlsym(nvmlLib, "nvmlDeviceGetHandleByIndex");
    }


    nvmlGetCurrTempFunc = (nvmlDeviceGetTemperature_t)dlsym(nvmlLib, "nvmlDeviceGetTemperature");
    nvmlShutdown = (nvmlShutdown_t)dlsym(nvmlLib, "nvmlShutdown");
    nvmlGetMemFunc = (nvmlDeviceGetMemoryInfo_t)dlsym(nvmlLib, "nvmlDeviceGetMemoryInfo");
    nvmlGetMaxTempFunc = (nvmlDeviceGetMaxTemperature_t)dlsym(nvmlLib, "nvmlDeviceGetTemperatureThreshold");

    if (!initFunc || !getDeviceCount || !getHandleFunc || !nvmlGetCurrTempFunc
        || !nvmlGetMemFunc || !nvmlGetMaxTempFunc || !nvmlShutdown ) {
        dlclose(nvmlLib);
        nvmlLib = nullptr;
        return;
    }

    if (initFunc() != NVML_SUCCESS) {
        std::cerr << "Couldn't initialize nvml library!" << std::endl;
        dlclose(nvmlLib);
        nvmlLib = nullptr;
        return;
    }

    unsigned int deviceCount;
    tempVram = {0, 0, 0};

    if (getDeviceCount(&deviceCount) != NVML_SUCCESS || deviceCount == 0) {
        std::cerr<<"Unable to retrieve the device count!"<<std::endl;
        nvmlShutdown();
        dlclose(nvmlLib);
        nvmlLib = nullptr;
        return;
    }

    if (getHandleFunc(0, &gpu) != NVML_SUCCESS) {
        std::cerr<<"Unable to retrieve the GPU handle!"<<std::endl;
        nvmlShutdown();
        dlclose(nvmlLib);
        nvmlLib = nullptr;
        return;
    }

}

NVIDIAGpuReader::~NVIDIAGpuReader(){
    if (nvmlLib && nvmlGetCurrTempFunc && nvmlGetMemFunc){
        nvmlShutdown();
        dlclose(nvmlLib);
    }
}

void NVIDIAGpuReader::readMaxTemp() {
    if (nvmlLib){
        unsigned int temp = 0;
        if (nvmlGetMaxTempFunc(gpu, NVML_TEMPERATURE_THRESHOLD_SHUTDOWN, &temp) != NVML_SUCCESS) {
            std::cerr<<"Unable to retrieve max temperature threshold for GPU!"<<std::endl;
            return;
        }

        maxTemp_ = static_cast<int>(temp);
    }
}


void NVIDIAGpuReader::readCurrTemp() {
    if (nvmlLib){
        unsigned int temp = 0;
        if (nvmlGetCurrTempFunc(gpu, NVML_TEMPERATURE_GPU, &temp) != NVML_SUCCESS) {
            std::cerr<<"Unable to retrieve current temperature!"<<std::endl;
            return;
        }

        currTemp_ = static_cast<int>(temp);
    }
}


void NVIDIAGpuReader::readVRAM() {
    if (nvmlLib){
        nvmlMemory_t vram;
        if (nvmlGetMemFunc(gpu, &vram) != NVML_SUCCESS) {
            std::cerr<<"Unable to retrieve VRAM!"<<std::endl;
            return;
        }

        vram_.total = vram.total/(1024*1024);
        vram_.free = vram.free/(1024*1024);
        vram_.used = vram.used/(1024*1024);
    }
}
