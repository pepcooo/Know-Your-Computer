#include "gpu-reader.h"

#include "nvml.h"
#include <dlfcn.h>

#include <iostream>



typedef nvmlReturn_t (*nvmlInit_t)(void);
typedef nvmlReturn_t (*nvmlShutdown_t)(void);
typedef nvmlReturn_t (*nvmlDeviceGetCount_t)(unsigned int*);
typedef nvmlReturn_t (*nvmlDeviceGetHandleByIndex_t)(unsigned int, nvmlDevice_t*);
typedef nvmlReturn_t (*nvmlDeviceGetMaxTemperature_t)(nvmlDevice_t, nvmlTemperatureThresholds_enum, unsigned int*);
typedef nvmlReturn_t (*nvmlDeviceGetTemperature_t)(nvmlDevice_t, nvmlTemperatureSensors_t, unsigned int*);
typedef nvmlReturn_t (*nvmlDeviceGetMemoryInfo_t)(nvmlDevice_t, nvmlMemory_t*);




void NVIDIAGpuReader::readMaxTemp() {
    //dynamically linking the library
    void* nvmlLib = dlopen("libnvidia-ml.so.1", RTLD_NOW);
    if (!nvmlLib) {
        std::cerr<<"Couldn't initialize libnvidia-ml.so.1 library!"<<std::endl;
        return;
    }
    //Using nvml api to retrieve the information
    auto initFunc = (nvmlInit_t)dlsym(nvmlLib, ("nvmlInit_v2"));
    if (!initFunc) {
        initFunc = (nvmlInit_t)dlsym(nvmlLib, ("nvmlInit"));
    }

    auto shutdownFunc = (nvmlShutdown_t)dlsym(nvmlLib, "nvmlShutdown");
    auto getDeviceCount = (nvmlDeviceGetCount_t)dlsym(nvmlLib, "nvmlDeviceGetCount");
    auto getHandleFunc = (nvmlDeviceGetHandleByIndex_t)dlsym(nvmlLib, "nvmlDeviceGetHandleByIndex_v2");
    auto getTempFunc = (nvmlDeviceGetMaxTemperature_t)dlsym(nvmlLib, "nvmlDeviceGetTemperatureThreshold");

    if (!initFunc || !shutdownFunc || !getDeviceCount || !getHandleFunc || !getTempFunc) {
        std::cerr << "Couldn't find necessary functions in nvml library!" << std::endl;
        dlclose(nvmlLib);
        return;
    }

    if (initFunc() != NVML_SUCCESS) {
        std::cerr << "Couldn't initialize nvml library!" << std::endl;
        dlclose(nvmlLib);
        return;
    }

    unsigned int deviceCount;
    nvmlDevice_t gpu;
    unsigned int temp;

    if (getDeviceCount(&deviceCount) != NVML_SUCCESS || deviceCount == 0) {
        std::cerr<<"Unable to retrieve the device count!"<<std::endl;
        shutdownFunc();
        dlclose(nvmlLib);
        return;
    }

    if (getHandleFunc(0, &gpu) != NVML_SUCCESS) {
        std::cerr<<"Unable to retrieve the GPU handle!"<<std::endl;
        shutdownFunc();
        dlclose(nvmlLib);
        return;
    }

    if (getTempFunc(gpu, NVML_TEMPERATURE_THRESHOLD_SHUTDOWN, &temp) != NVML_SUCCESS) {
        std::cerr<<"Unable to retrieve max temperature threshold for GPU!"<<std::endl;
        shutdownFunc();
        dlclose(nvmlLib);
        return;
    }

    //Clang-Tidy requirement
    maxTemp_ = static_cast<int>(temp);
    shutdownFunc();
    dlclose(nvmlLib);
}


void NVIDIAGpuReader::readCurrTemp() {
    void* nvmlLib = dlopen("libnvidia-ml.so.1", RTLD_NOW);
    if (!nvmlLib) {
        return;
    }

    auto initFunc = (nvmlInit_t)dlsym(nvmlLib, ("nvmlInit_v2"));
    if (!initFunc) {
        initFunc = (nvmlInit_t)dlsym(nvmlLib, ("nvmlInit"));
    }

    auto shutdownFunc = (nvmlShutdown_t)dlsym(nvmlLib, "nvmlShutdown");
    auto getDeviceCount = (nvmlDeviceGetCount_t)dlsym(nvmlLib, "nvmlDeviceGetCount");
    auto getHandleFunc = (nvmlDeviceGetHandleByIndex_t)dlsym(nvmlLib, "nvmlDeviceGetHandleByIndex_v2");
    if (!getHandleFunc) {
        getHandleFunc = (nvmlDeviceGetHandleByIndex_t)dlsym(nvmlLib, "nvmlDeviceGetHandleByIndex");
    }
    auto getTempFunc = (nvmlDeviceGetTemperature_t)dlsym(nvmlLib, "nvmlDeviceGetTemperature");

    if (!initFunc || !shutdownFunc || !getDeviceCount || !getHandleFunc || !getTempFunc) {
        dlclose(nvmlLib);
        return;
    }

    if (initFunc() != NVML_SUCCESS) {
        dlclose(nvmlLib);
        return;
    }

    unsigned int deviceCount;
    nvmlDevice_t gpu;
    unsigned int temp;

    if (getDeviceCount(&deviceCount) != NVML_SUCCESS || deviceCount == 0) {
        shutdownFunc();
        dlclose(nvmlLib);
        return;
    }

    if (getHandleFunc(0, &gpu) != NVML_SUCCESS) {
        shutdownFunc();
        dlclose(nvmlLib);
        return;
    }

    if (getTempFunc(gpu, NVML_TEMPERATURE_GPU, &temp) != NVML_SUCCESS) {
        shutdownFunc();
        dlclose(nvmlLib);
        return;
    }

    currTemp_ = static_cast<int>(temp);
    shutdownFunc();
    dlclose(nvmlLib);
}


void NVIDIAGpuReader::readVRAM() {
    void* nvmlLib = dlopen("libnvidia-ml.so.1", RTLD_NOW);
    if (!nvmlLib) {
        return;
    }

    auto initFunc = (nvmlInit_t)dlsym(nvmlLib, ("nvmlInit_v2"));
    if (!initFunc) {
        initFunc = (nvmlInit_t)dlsym(nvmlLib, ("nvmlInit"));
    }

    auto shutdownFunc = (nvmlShutdown_t)dlsym(nvmlLib, "nvmlShutdown");
    auto getDeviceCount = (nvmlDeviceGetCount_t)dlsym(nvmlLib, "nvmlDeviceGetCount");
    auto getHandleFunc = (nvmlDeviceGetHandleByIndex_t)dlsym(nvmlLib, "nvmlDeviceGetHandleByIndex_v2");
    if (!getHandleFunc) {
        getHandleFunc = (nvmlDeviceGetHandleByIndex_t)dlsym(nvmlLib, "nvmlDeviceGetHandleByIndex");
    }
    auto getVramFunc = (nvmlDeviceGetMemoryInfo_t)dlsym(nvmlLib, "nvmlDeviceGetMemoryInfo");

    if (!initFunc || !shutdownFunc || !getDeviceCount || !getHandleFunc || !getVramFunc) {
        std::cerr << "Couldn't find necessary functions in nvml library (VRAM reading)!" << std::endl;
        dlclose(nvmlLib);
        return;
    }

    if (initFunc() != NVML_SUCCESS) {
        dlclose(nvmlLib);
        return;
    }

    unsigned int deviceCount;
    nvmlDevice_t gpu;
    nvmlMemory_t vram = {0, 0, 0};

    if (getDeviceCount(&deviceCount) != NVML_SUCCESS || deviceCount == 0) {
        shutdownFunc();
        dlclose(nvmlLib);
        return;
    }

    if (getHandleFunc(0, &gpu) != NVML_SUCCESS) {
        shutdownFunc();
        dlclose(nvmlLib);
        return;
    }

    if (getVramFunc(gpu, &vram) != NVML_SUCCESS) {
        shutdownFunc();
        dlclose(nvmlLib);
        return;
    }

    vram_.total = vram.total;
    vram_.free = vram.free;
    vram_.used = vram.used;

    shutdownFunc();
    dlclose(nvmlLib);
}
