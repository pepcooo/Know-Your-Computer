#include "proc-reader.h"
#include "gpu-reader.h"

int main()
{
    ProcReader proc;
    GpuReader gpuReader;

    proc.readModel();
    proc.printModel();
    gpuReader.readModel();
    gpuReader.printModel();

    return 0;
}

