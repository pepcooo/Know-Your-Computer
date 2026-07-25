#include <iostream>
#include <ostream>

#include "proc-reader.h"
#include "gpu-reader.h"

int main()
{
    ProcReader proc;

    if (GpuReader* gpuReader = GpuFactory::createGpuReader()) {
        gpuReader->readMaxTemp();
        gpuReader->printModel();
    }
    else {
        std::cerr<<"Err: Couldn't find GPU!"<<std::endl;
    }
    proc.readModel();
    proc.printModel();

    return 0;
}

