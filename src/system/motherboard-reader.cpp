#include "../../include/system/motherboard-reader.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <array>
#include <cstdio>


std::string MotherboardReader::execCommand(const char* cmd) {
    std::array<char, 128> buffer{};
    std::string type;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

    if (!pipe) {
        return "Unknown";
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        type += buffer.data();
    }

    if (!type.empty() && type.back() == '\n') {
        type.pop_back();
    }
    return type;
}

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