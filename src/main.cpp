#include "proc-reader.h"
#include "gpu-reader.h"

int main()
{
    ProcReader proc;
    GpuReader* gpuReader = GpuFactory::createGpuReader();

    gpuReader->readModel();
    gpuReader->readMaxTemp();
    gpuReader->printModel();

    proc.readModel();
    proc.printModel();

    return 0;
}

