#include "gpu-reader.h"
#include "zes_api.h"
#include <dlfcn.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>


namespace fs = std::filesystem;


typedef ze_result_t (*zesInit_t)(zes_init_flags_t);
typedef ze_result_t (*zesDriverGet_t)(uint32_t*, zes_driver_handle_t*);
typedef ze_result_t (*zesDeviceGet_t)(zes_driver_handle_t, uint32_t*, zes_device_handle_t*);
typedef ze_result_t (*zesDeviceEnumMemoryModules_t)(zes_device_handle_t, uint32_t*, zes_mem_handle_t*);
typedef ze_result_t (*zesMemoryGetState_t)(zes_mem_handle_t, zes_mem_state_t*);


void IntelGpuReader::readMaxTemp()
{
    //Checking every directory in hwmon directory to find Intel made GPUs.
    for (const auto& entry : fs::directory_iterator("/sys/class/hwmon")) {
        if (entry.is_directory()) {
            std::string hwmonNamePath = entry.path().string() + "/name";
            std::ifstream hwmonNameFile(hwmonNamePath);

            if (!hwmonNameFile.is_open()) {
                std::cerr<<"Couldn't open the name file for Intel based GPU!"<<std::endl;
                continue;
            }
            std::string name;
            hwmonNameFile >> name;
            if (name == "i915" || name == "xe") {
                for (const auto& maxTempEntry : fs::directory_iterator(entry.path())) {
                    if (maxTempEntry.path().string().find("temp") != std::string::npos
                    && maxTempEntry.path().string().find("_crit") != std::string::npos
                    && maxTempEntry.path().string().find("_alarm") == std::string::npos) {
                        std::ifstream maxTempFile(maxTempEntry.path().string());
                        if (!maxTempFile.is_open()) {
                            std::cerr<<"Couldn't open the temp*_crit file for Intel based GPU!"<<std::endl;
                            continue;
                        }
                        int highestTemp = 0;
                        maxTempFile >> highestTemp;
                        highestTemp/=1000;
                        if (highestTemp > maxTemp_) {
                            maxTemp_ = highestTemp;
                        }
                    }
                }
            }
        }
    }

    //Fallback 1 - if nothing found, check for name file equal to "coretemp"
    if (maxTemp_ <= 0){
        for (const auto& entry : fs::directory_iterator("/sys/class/hwmon")) {
            if (entry.is_directory()) {
                std::string hwmonNamePath = entry.path().string() + "/name";
                std::ifstream hwmonNameFile(hwmonNamePath);

                if (!hwmonNameFile.is_open()) {
                    std::cerr<<"Couldn't open the name file for potentially integrated Intel based GPU!"<<std::endl;
                    continue;
                }
                std::string name;
                hwmonNameFile >> name;
                if (name == "coretemp") {
                    for (const auto& maxTempEntry : fs::directory_iterator(entry.path())) {
                        if (maxTempEntry.path().string().find("temp") != std::string::npos
                        && maxTempEntry.path().string().find("_crit") != std::string::npos
                        && maxTempEntry.path().string().find("_alarm") == std::string::npos) {
                            std::ifstream maxTempFile(maxTempEntry.path().string());
                            if (!maxTempFile.is_open()) {
                                std::cerr<<"Couldn't open the temp*_crit file for potentially integrated Intel based GPU!"<<std::endl;
                                continue;
                            }
                            int highestTemp = 0;
                            maxTempFile >> highestTemp;
                            highestTemp/=1000;
                            if (highestTemp > maxTemp_) {
                                maxTemp_ = highestTemp;
                            }
                        }
                    }
                }
            }
        }
    }


    //Fallback 2 - check every directory in search for thermal_zone* directory with type of x86_pkg_temp
    // if nothing found in hwmon.
    if (maxTemp_ <= 0){
        for (const auto& entry : fs::directory_iterator("/sys/class/thermal")) {
            if (entry.is_directory()) {
                if (entry.path().string().find("thermal_zone") != std::string::npos) {
                    std::string typePath = entry.path().string() + "/type";
                    std::ifstream typeFile(typePath);
                    if (!typeFile.is_open()) {
                        std::cerr<<"Couldn't open the type file in thermal_zone* directory!"<<std::endl;
                        continue;
                    }
                    std::string type;
                    typeFile >> type;
                    //If the type matches Intel GPUs, check the thermal_zone directory for trip_point files
                    if (type == "x86_pkg_temp") {
                        for (const auto& maxTempEntry : fs::directory_iterator(entry.path())) {
                            if (maxTempEntry.path().string().find("trip_point_") != std::string::npos
                            && maxTempEntry.path().string().find("temp") != std::string::npos) {
                                std::ifstream maxTempFile(maxTempEntry.path().string());
                                if (!maxTempFile.is_open()) {
                                    std::cerr<<"Couldn't open the trip_point_*_temp file in thermal_zone directory!"<<std::endl;
                                    continue;
                                }
                                int highestTemp = 0;
                                maxTempFile >> highestTemp;
                                //Divide by 1000 - Linux holds information in millidegrees
                                highestTemp/=1000;
                                if (highestTemp > maxTemp_) {
                                    maxTemp_ = highestTemp;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //Final fallback - if no info found, set maxTemp_ to default max Intel GPU temperature
    if (maxTemp_ <= 0) {
        maxTemp_ = 100;
    }
}


void IntelGpuReader::readCurrTemp() {
    for (const auto& entry : fs::directory_iterator("/sys/class/hwmon")) {
        if (entry.is_directory()) {
            std::string hwmonNamePath = entry.path().string() + "/name";
            std::ifstream hwmonNameFile(hwmonNamePath);
            if (!hwmonNameFile.is_open()) {
                continue;
            }

            std::string name;
            hwmonNameFile >> name;

            if (name == "i915" || name == "xe") {
                int highestTemperature = 0;
                for (const auto& tempEntry : fs::directory_iterator(entry.path())) {
                    if (tempEntry.path().string().find("temp") != std::string::npos
                    && tempEntry.path().string().find("_input") != std::string::npos) {
                        std::ifstream tempFile(tempEntry.path().string());
                        if (!tempFile.is_open()) {
                            continue;
                        }
                        int temperature = 0;
                        tempFile >> temperature;
                        temperature/=1000;
                        //Searching for highest temperature sensor on the GPU
                        if (temperature > highestTemperature) {
                            highestTemperature = temperature;
                        }
                    }
                }
                currTemp_ = highestTemperature;
                return;
            }
        }
    }

    //Fallback 1 - if there wasn't a hwmon folder with name "i915" nor "xe" search for "coretemp"
    //because the GPU is probably integrated with CPU.
    for (const auto& entry : fs::directory_iterator("/sys/class/hwmon")) {
        if (entry.is_directory()) {
            std::string hwmonNamePath = entry.path().string() + "/name";
            std::ifstream hwmonNameFile(hwmonNamePath);
            if (!hwmonNameFile.is_open()) {
                continue;
            }
            std::string name;
            hwmonNameFile >> name;

            if (name == "coretemp") {
                int highestTemperature = 0;
                for (const auto& labelEntry : fs::directory_iterator(entry.path())) {
                    if (labelEntry.path().string().find("temp") != std::string::npos
                    && labelEntry.path().string().find("_label") != std::string::npos) {

                        std::ifstream labelFile(labelEntry.path().string());
                        if (!labelFile.is_open()) {
                            continue;
                        }

                        std::string label;
                        std::getline(labelFile, label);
                        if (label.find("Package id") != std::string::npos) {
                            std::string tempPath = labelEntry.path().string();
                            size_t index = tempPath.find("_label");
                            tempPath.replace(index, 6, "_input");

                            std::ifstream tempFile(tempPath);
                            if (!tempFile.is_open()) {
                                continue;
                            }
                            int temperature = 0;
                            tempFile >> temperature;
                            temperature/=1000;
                            if (temperature > highestTemperature) {
                                highestTemperature = temperature;
                            }
                        }
                    }
                }
                currTemp_ = highestTemperature;
                return;
            }
        }

    if (currTemp_ <= 0) {
        currTemp_ = -273;
    }}
}


void IntelGpuReader::readVRAM() {
    void* zeLib = dlopen("libze_loader.so.1", RTLD_NOW);
    if (!zeLib){
        return;
    }

    auto initFunc = (zesInit_t)dlsym(zeLib, "zesInit");
    auto getDriverFunc = (zesDriverGet_t)dlsym(zeLib, "zesDriverGet");
    auto getDeviceHandle = (zesDeviceGet_t)dlsym(zeLib, "zesDeviceGet");
    auto getModuleHandle = (zesDeviceEnumMemoryModules_t)dlsym(zeLib, "zesDeviceEnumMemoryModules");
    auto getMemoryStateFunc = (zesMemoryGetState_t)dlsym(zeLib, "zesMemoryGetState");

    if (!initFunc || !getDriverFunc || !getDeviceHandle || !getModuleHandle || !getMemoryStateFunc){
        std::cerr << "Couldn't find necessary functions in level-zero library!" << std::endl;
        dlclose(zeLib);
        return;
    }

    if (initFunc(0) != ZE_RESULT_SUCCESS){
        std::cerr<<"Couldn't initialize level-zero library!"<<std::endl;
        dlclose(zeLib);
        return;
    }

    uint32_t driverCount = 0;
    if (getDriverFunc(&driverCount, nullptr) != ZE_RESULT_SUCCESS){
        std::cerr<<"Couldn't get driver count!"<<std::endl;
        dlclose(zeLib);
        return;
    }

    std::vector<zes_driver_handle_t> drivers(driverCount);
    if (driverCount == 0 || getDriverFunc(&driverCount, drivers.data()) != ZE_RESULT_SUCCESS){
        std::cerr<<"Couldn't write the drivers data!"<<std::endl;
        dlclose(zeLib);
        return;
    }

    uint32_t deviceCount = 0;
    if (getDeviceHandle(drivers.at(0), &deviceCount, nullptr) != ZE_RESULT_SUCCESS){
        std::cerr<<"Couldn't get gpu device!"<<std::endl;
        dlclose(zeLib);
        return;
    }

    std::vector<zes_device_handle_t> devices(deviceCount);
    if (deviceCount == 0 || getDeviceHandle(drivers.at(0), &deviceCount, devices.data()) != ZE_RESULT_SUCCESS){
        std::cerr<<"Couldn't get gpu devices!"<<std::endl;
        dlclose(zeLib);
        return;
    }
    zes_device_handle_t gpu = devices.at(0);

    uint32_t memCount = 0;
    if (getModuleHandle(gpu, &memCount, nullptr) != ZE_RESULT_SUCCESS){
        std::cerr<<"Couldn't get memory handle!"<<std::endl;
        dlclose(zeLib);
        return;
    }

    std::vector<zes_mem_handle_t> memory(memCount);
    if (memCount == 0 || getModuleHandle(gpu, &memCount, memory.data()) != ZE_RESULT_SUCCESS){
        std::cerr<<"Couldn't get memory handle!"<<std::endl;
        dlclose(zeLib);
        return;
    }
    zes_mem_handle_t memHandle = memory.at(0);

    zes_mem_state_t memState = {};
    memState.stype = ZES_STRUCTURE_TYPE_MEM_STATE;
    if (getMemoryStateFunc(memHandle, &memState) != ZE_RESULT_SUCCESS){
        std::cerr<<"Couldn't get memory state!"<<std::endl;
        dlclose(zeLib);
        return;
    }

    vram_.total = memState.size;
    vram_.free = memState.free;

    dlclose(zeLib);
}
