#include "../../include/system/motherboard-reader.h"
#include <iostream>
#include <fstream>

void MotherboardReader::readModel() {
    std::ifstream fileVendor_("/sys/class/dmi/id/board_vendor");
    if (fileVendor_.is_open()) {
        std::getline(fileVendor_, vendor_);
        fileVendor_.close();
    }
    else {
        vendor_ = "Unknown";
    }

    std::ifstream fileModel_("/sys/class/dmi/id/board_name");
    if (fileModel_.is_open()) {
        std::getline(fileModel_, modelName_);
        fileModel_.close();
    }
    else {
        modelName_ = "Unknown";
    }

    ramType_ = execCommand("/usr/sbin/dmidecode -t memory 2>/dev/null | grep 'Type:' | grep -v 'Error' | head -n 1 | cut -d ':' -f 2");

    if (!ramType_.empty() && ramType_[0] == ' ') {
        ramType_.erase(0, 1);
    }

}

void MotherboardReader::printModel() const {
    std::cout << "Vendor: " << vendor_ << std::endl;
    std::cout << "Model: " << modelName_ << std::endl;
    std::cout << "RAM type: " << ramType_ << std::endl;

}