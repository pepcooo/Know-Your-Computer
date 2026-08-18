#pragma once

#include "gpu-reader.h"
#include "nvml.h"


typedef nvmlReturn_t (*nvmlInit_t)(void);
typedef nvmlReturn_t (*nvmlShutdown_t)(void);
typedef nvmlReturn_t (*nvmlDeviceGetCount_t)(unsigned int*);
typedef nvmlReturn_t (*nvmlDeviceGetHandleByIndex_t)(unsigned int, nvmlDevice_t*);
typedef nvmlReturn_t (*nvmlDeviceGetMaxTemperature_t)(nvmlDevice_t, nvmlTemperatureThresholds_enum, unsigned int*);
typedef nvmlReturn_t (*nvmlDeviceGetTemperature_t)(nvmlDevice_t, nvmlTemperatureSensors_t, unsigned int*);
typedef nvmlReturn_t (*nvmlDeviceGetMemoryInfo_t)(nvmlDevice_t, nvmlMemory_t*);



class NVIDIAGpuReader : public GpuReader {
private:
    void* nvmlLib;
    nvmlDevice_t gpu;
    nvmlMemory_t tempVram;
    nvmlDeviceGetTemperature_t nvmlGetCurrTempFunc;
    nvmlDeviceGetMemoryInfo_t nvmlGetMemFunc;
    nvmlDeviceGetMaxTemperature_t nvmlGetMaxTempFunc;
    nvmlShutdown_t nvmlShutdown;
public:
    NVIDIAGpuReader(const std::string& modelName);
    ~NVIDIAGpuReader();
    void readMaxTemp() override;
    void readCurrTemp() override;
    void readVRAM() override;
};
