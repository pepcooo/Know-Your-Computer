#pragma once

#include "gpu-reader.h"
#include "zes_api.h"

typedef ze_result_t (*zesInit_t)(zes_init_flags_t);
typedef ze_result_t (*zesDriverGet_t)(uint32_t*, zes_driver_handle_t*);
typedef ze_result_t (*zesDeviceGet_t)(zes_driver_handle_t, uint32_t*, zes_device_handle_t*);
typedef ze_result_t (*zesDeviceEnumMemoryModules_t)(zes_device_handle_t, uint32_t*, zes_mem_handle_t*);
typedef ze_result_t (*zesMemoryGetState_t)(zes_mem_handle_t, zes_mem_state_t*);


class IntelGpuReader : public GpuReader {
private:
    void* zeLib;
    zes_mem_handle_t memHandle;
    zes_mem_state_t memState;
    zesMemoryGetState_t getMemoryStateFunc;
public:
    IntelGpuReader(const std::string& modelName);
    ~IntelGpuReader();
    void readMaxTemp() override;
    void readCurrTemp() override;
    void readVRAM() override;
};