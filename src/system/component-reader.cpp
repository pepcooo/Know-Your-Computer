#include "../../include/system/component-reader.h"
#include <iostream>
#include <memory>
#include <array>
#include <cstdio>

std::string ComponentReader::execCommand(const char* cmd) {
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
