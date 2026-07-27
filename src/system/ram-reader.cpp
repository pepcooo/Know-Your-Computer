#include "ram-reader.h"
#include <iostream>
#include <array>
#include <sstream>

void RamReader::readModel() {
    const std::string ramSizes = execCommand("free -h | awk 'NR==2 {print $2, $3, $6, $7}'");
    std::stringstream ss(ramSizes);
    ss >> ramTotal_ >> ramUsed_ >> ramCache_ >> ramAvailable_;

    std::string allVendors = execCommand("LC_ALL=C /usr/sbin/dmidecode -t memory | grep 'Manufacturer:' | awk -F': ' '{print $2}'");
    std::stringstream ssVendors(allVendors);
    std::string vendorsLine;
    vendors_.clear();

    while (std::getline(ssVendors, vendorsLine)) {
        if (!vendorsLine.empty()) {
            vendors_.push_back(vendorsLine);
        }
    }

    std::string allSpeeds = execCommand("LC_ALL=C /usr/sbin/dmidecode -t memory 2>/dev/null | grep 'Speed:' | grep -v 'Configured' | awk -F': ' '{print $2}'");
    std::stringstream ssSpeeds(allSpeeds);
    std::string speedsLine;
    speeds_.clear();

    while (std::getline(ssSpeeds, speedsLine)) {
        if (!speedsLine.empty()) {
            speeds_.push_back(speedsLine);
        }
    }

    std::string allSizes= execCommand("LC_ALL=C /usr/sbin/dmidecode -t memory 2>/dev/null | grep '^[[:space:]]*Size:' | cut -d ':' -f 2");
    std::stringstream ssSizes(allSizes);
    std::string sizeLine;
    sizes_.clear();

    while (std::getline(ssSizes, sizeLine)) {
        if (!sizeLine.empty()) {
            sizes_.push_back(sizeLine);
        }
    }

    std::string allModels = execCommand("LC_ALL=C /usr/sbin/dmidecode -t memory 2>/dev/null | grep 'Part Number:' | cut -d ':' -f 2");
    std::stringstream ssModels(allModels);
    std::string modelLine;
    models_.clear();

    while (std::getline(ssModels, modelLine)) {
        if (!modelLine.empty()) {
            models_.push_back(modelLine);
        }
    }
}

void RamReader::printModel() const {
    std::cout << "Total Memory: " << ramTotal_ << std::endl;
    std::cout << "Used Memory: " << ramUsed_ << std::endl;
    std::cout << "Cache Memory: " << ramCache_ << std::endl;
    std::cout << "Available Memory: " << ramAvailable_ << std::endl;
    std::cout << "Number of RAM sticks: " << vendors_.size() << std::endl;

    for (int i = 0; i < vendors_.size(); i++) {
        std::cout << "Slot " << i+1 << ": "<< vendors_[i] << std::endl;
        if (i < sizes_.size() && sizes_[i] != " None" && sizes_[i] != "No Module Installed") {
            std::cout << "  Size:   " << sizes_[i] << std::endl;
        }
        if (i < models_.size() && models_[i] != " Not Specified" && models_[i] != "Unknown") {
            std::cout << "  Model:  " << models_[i] << std::endl;
        }
        if (i < speeds_.size() && speeds_[i] != "Unknown") {
            std::cout << "  Speed:  " << speeds_[i] << std::endl;
        }
    }
}
