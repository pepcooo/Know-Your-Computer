#pragma once

#include "gpu-reader.h"


class IntelGpuReader : public GpuReader {
public:
    IntelGpuReader(const std::string& modelName) : GpuReader(modelName) {}
    void readMaxTemp() override;
    void readCurrTemp() override;
    void readVRAM() override;
};