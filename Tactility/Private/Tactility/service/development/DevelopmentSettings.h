#pragma once
#ifdef ESP_PLATFORM

#include <cstdint>

namespace tt::service::development {

void setEnableOnBoot(bool enable);

bool shouldEnableOnBoot();

void setIdleCheckIntervalSeconds(uint32_t seconds);

uint32_t getIdleCheckIntervalSeconds();

}

#endif // ESP_PLATFORM
