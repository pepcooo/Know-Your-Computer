#pragma once

#include "../component-reader.h"

class GpuReader : public ComponentReader {
protected:
    unsigned int maxTemp_;
public:
    GpuReader(const std::string& modelName)
    : ComponentReader("Graphics Processing Unit (GPU)"), maxTemp_(0) {
        modelName_ = modelName;
    }
    ~GpuReader() override = default;

    void readModel() override {};
    void printModel() const override;

    virtual void readMaxTemp() = 0;
    void printMaxTemp() const;
};

class IntelReader : public GpuReader {
public:
    IntelReader(const std::string& modelName) : GpuReader(modelName) {}
    void readMaxTemp() override;
};

class NVIDIAReader : public GpuReader {
public:
    NVIDIAReader(const std::string& modelName) : GpuReader(modelName) {}
    void readMaxTemp() override;
};

class AMDReader : public GpuReader {
public:
    AMDReader(const std::string& modelName) : GpuReader(modelName) {}
    void readMaxTemp() override;
};

class GpuFactory {
public:
    static GpuReader* createGpuReader();
};