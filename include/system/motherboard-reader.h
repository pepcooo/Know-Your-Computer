#pragma once

#include "component-reader.h"

class MotherboardReader : public ComponentReader {
public:
    MotherboardReader() : ComponentReader("Feature") {};
    ~MotherboardReader() override = default;

    void readModel() override;
    void printModel() const override;

private:
    std::string RAM_type;
    std::string socket;
    std::string vendor;
};
