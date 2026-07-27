#pragma once
#include <vector>

#include "component-reader.h"

class RamReader : public ComponentReader {
public:
    RamReader() : ComponentReader("RAM") {};
    ~RamReader() override = default;

    void readModel() override;
    void printModel() const override;

private:
    std::vector<std::string> speeds_;
    std::vector<std::string> vendors_;
    std::vector<std::string> sizes_;
    std::vector<std::string> models_;
    std::string ramTotal_;
    std::string ramUsed_;
    std::string ramCache_;
    std::string ramAvailable_;


};