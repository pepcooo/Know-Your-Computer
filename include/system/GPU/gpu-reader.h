#pragma once

#include "../component-reader.h"

class GpuReader : public ComponentReader {
protected:
    unsigned int maxTemp_;
public:
    //Constructor assigns modelName_ given by GpuFactory
    GpuReader(const std::string& modelName)
    : ComponentReader("Graphics Processing Unit (GPU)"), maxTemp_(0) {
        modelName_ = modelName;
    }
    ~GpuReader() override = default;

    //ALREADY READ BY FACTORY
    void readModel() override {};
    void printModel() const override;

    //Purely virtual = Intel, NVIDIA and AMD GPUs read max temperature differently.
    virtual void readMaxTemp() = 0;
    void printMaxTemp() const;
};


class IntelGpuReader : public GpuReader {
public:
    IntelGpuReader(const std::string& modelName) : GpuReader(modelName) {}
    void readMaxTemp() override;
};

class NVIDIAGpuReader : public GpuReader {
public:
    NVIDIAGpuReader(const std::string& modelName) : GpuReader(modelName) {}
    void readMaxTemp() override;
};

class AMDGpuReader : public GpuReader {
public:
    AMDGpuReader(const std::string& modelName) : GpuReader(modelName) {}
    void readMaxTemp() override;
};


//A GPU factory that only checks the name of the model and looks for one of the three keywords and returns
//appropriate class.
class GpuFactory {
public:
    static GpuReader* createGpuReader();
};