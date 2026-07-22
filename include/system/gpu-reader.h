#pragma once

#include "component-reader.h"

class GpuReader : public ComponentReader {
private:
    unsigned int maxTemp_;
public:
    GpuReader() : ComponentReader("Graphics Processing Unit (GPU)"), maxTemp_(0) {}
    ~GpuReader() override = default;

    void readModel() override;
    void printModel() const override;
    void readMaxTemp();
    void printMaxTemp() const;
};