#pragma once

#include "gpu-reader.h"


class AMDGpuReader : public GpuReader {
public:
    AMDGpuReader(const std::string& modelName) : GpuReader(modelName) {}
    void readMaxTemp() override;
    void readCurrTemp() override;
    void readVRAM() override;
};