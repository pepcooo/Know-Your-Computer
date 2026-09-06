#pragma once

#include "../component-reader.h"

class GpuReader : public ComponentReader {
protected:
    int maxTemp_;
    int currTemp_;

    typedef struct VRAM_t{
        unsigned long long total;
        unsigned long long free;
        unsigned long long used;
    };
    VRAM_t vram_;

public:
    //Constructor assigns modelName_ given by GpuFactory
    GpuReader(const std::string& modelName)
    : ComponentReader("Graphics Processing Unit (GPU)"), maxTemp_(0), currTemp_(0) {
        modelName_ = modelName;
        vram_.total = vram_.used = vram_.free = 0;
    }
    ~GpuReader() override = default;

    //ALREADY READ BY FACTORY
    void readModel() override {};
    void printModel() const override;

    //Purely virtual = Intel, NVIDIA and AMD GPUs read max temperature differently.
    virtual void readMaxTemp() = 0;
    void printMaxTemp() const;

    virtual void readCurrTemp() = 0;
    void printCurrTemp() const;

    virtual void readVRAM() = 0;
    void printVRAM() const;
};


//A GPU factory that only checks the name of the model and looks for one of the three keywords and returns
//appropriate class.
class GpuFactory {
public:
    static GpuReader* createGpuReader();
};