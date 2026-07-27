#include "../../include/system/motherboard-reader.h"
#include <iostream>
#include <fstream>

void MotherboardReader::readModel() {
    std::ifstream fileVendor("/sys/class/dmi/id/board_vendor");
    if (fileVendor.is_open()) {
        getline(fileVendor, vendor);
        fileVendor.close();
    }
    else {
        vendor = "Unknown";
    }

    std::ifstream fileModel("/sys/class/dmi/id/board_name");
    if (fileModel.is_open()) {
        getline(fileModel, modelName_);
        fileModel.close();
    }
    else {
        modelName_ = "Unknown";
    }

    RAM_type = execCommand("/usr/sbin/dmidecode -t memory 2>/dev/null | grep 'Type:' | grep -v 'Error' | head -n 1 | cut -d ':' -f 2");

    if (!RAM_type.empty() && RAM_type[0] == ' ') {
        RAM_type.erase(0, 1);
    }

}

void MotherboardReader::printModel() const {
    std::cout << "Vendor: " << vendor << std::endl;
    std::cout << "Model: " << modelName_ << std::endl;
    std::cout << "RAM type: " << RAM_type << std::endl;

}