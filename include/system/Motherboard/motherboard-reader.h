#pragma once

#include "../component-reader.h"

class MotherboardReader : public ComponentReader {
public:
    MotherboardReader() : ComponentReader("Motherboard") {};
    ~MotherboardReader() override = default;

    void readModel() override;
    void printModel() const override;

private:
    std::string ramType_;
    std::string socket_;
    std::string vendor_;
};
