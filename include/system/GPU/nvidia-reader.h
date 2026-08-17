#pragma once

#include "gpu-reader.h"


class NVIDIAGpuReader : public GpuReader {
public:
    NVIDIAGpuReader(const std::string& modelName) : GpuReader(modelName) {}
    void readMaxTemp() override;
    void readCurrTemp() override;
    void readVRAM() override;
};
