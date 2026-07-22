#pragma once

#include "../component-reader.h"

class ProcReader : public ComponentReader
{
public:
    ProcReader() : ComponentReader("Processor") {}
    ~ProcReader() override = default;

    void readModel() override;
    void printModel() const override;
};