#include "../../include/system/feature-reader.h"
#include <iostream>
#include <fstream>
#include <memory>   // do "otworzenia" terminala wirtualnego
#include <array>
#include <cstdio>   // do popen
using namespace std;

string FeatureReader::execCommand(const char* cmd) {
    array<char, 128> buffer{};
    string type;

    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);      //pilnowanie zamkniecia

    if (!pipe) {
        return "Unknown";
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        type += buffer.data();
    }

    if (!type.empty() && type.back() == '\n') { //usuwanie entera na koncu
        type.pop_back();
    }
    return type;
}

void FeatureReader::readModel() {
    ifstream fileVendor("/sys/class/dmi/id/board_vendor");
    if (fileVendor.is_open()) {
        getline(fileVendor, vendor);
        fileVendor.close();
    }
    else {
        vendor = "Unknown";
    }

    ifstream fileModel("/sys/class/dmi/id/board_name");
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

void FeatureReader::printModel() const {
    cout << "Vendor: " << vendor << endl;
    cout << "Model: " << modelName_ << endl;
    cout << "RAM type: " << RAM_type << endl;

}