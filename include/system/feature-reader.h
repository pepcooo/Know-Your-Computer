#pragma once

#include "component-reader.h"

class FeatureReader : public ComponentReader {
public:
    FeatureReader() : ComponentReader("Feature") {};
    ~FeatureReader() override = default;

    void readModel() override;
    void printModel() const override;

private:
    std::string RAM_type;
    std::string RAM_size;
    std::string socket;
    std::string vendor;

    static std::string execCommand(const char* cmd);
};
