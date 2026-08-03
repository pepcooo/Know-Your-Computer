#include <iostream>
#include <ostream>

#include "proc-reader.h"
#include "gpu-reader.h"
#include "motherboard-reader.h"

int main()
{
    ProcReader proc;
    MotherboardReader motherboard;

    if (GpuReader* gpuReader = GpuFactory::createGpuReader()) {
        gpuReader->readMaxTemp();
        gpuReader->printModel();
        gpuReader->printMaxTemp();
    }
    else {
        std::cerr<<"Err: Couldn't find GPU!"<<std::endl;
    }
    proc.readModel();
    proc.printModel();

    motherboard.readModel();
    motherboard.printModel();

    return 0;
}

