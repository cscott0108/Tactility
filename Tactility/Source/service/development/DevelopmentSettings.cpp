#ifdef ESP_PLATFORM
#include <Tactility/file/PropertiesFile.h>
#include <Tactility/Log.h>
#include <Tactility/service/development/DevelopmentSettings.h>
#include <map>
#include <string>

namespace tt::service::development {

constexpr auto* TAG = "DevSettings";
constexpr auto* SETTINGS_FILE = "/data/settings/development.properties";
constexpr auto* SETTINGS_KEY_ENABLE_ON_BOOT = "enableOnBoot";
constexpr auto* SETTINGS_KEY_IDLE_CHECK_INTERVAL = "idleCheckIntervalSeconds";
constexpr uint32_t DEFAULT_IDLE_CHECK_INTERVAL_SECONDS = 10;

struct DevelopmentSettings {
    bool enableOnBoot;
    uint32_t idleCheckIntervalSeconds;
};

static bool load(DevelopmentSettings& settings) {
    std::map<std::string, std::string> map;
    if (!file::loadPropertiesFile(SETTINGS_FILE, map)) {
        return false;
    }

    if (!map.contains(SETTINGS_KEY_ENABLE_ON_BOOT)) {
        return false;
    }

    auto enable_on_boot_string = map[SETTINGS_KEY_ENABLE_ON_BOOT];
    settings.enableOnBoot = (enable_on_boot_string == "true");

    // Load idle check interval with default fallback
    if (map.contains(SETTINGS_KEY_IDLE_CHECK_INTERVAL)) {
        auto& value_str = map[SETTINGS_KEY_IDLE_CHECK_INTERVAL];
        // Simple validation: check if non-empty and starts with digit
        if (!value_str.empty() && std::isdigit(value_str[0])) {
            settings.idleCheckIntervalSeconds = std::stoul(value_str);
        } else {
            settings.idleCheckIntervalSeconds = DEFAULT_IDLE_CHECK_INTERVAL_SECONDS;
        }
    } else {
        settings.idleCheckIntervalSeconds = DEFAULT_IDLE_CHECK_INTERVAL_SECONDS;
    }

    return true;
}

static bool save(const DevelopmentSettings& settings) {
    std::map<std::string, std::string> map;
    map[SETTINGS_KEY_ENABLE_ON_BOOT] = settings.enableOnBoot ? "true" : "false";
    map[SETTINGS_KEY_IDLE_CHECK_INTERVAL] = std::to_string(settings.idleCheckIntervalSeconds);
    return file::savePropertiesFile(SETTINGS_FILE, map);
}

void setEnableOnBoot(bool enable) {
    DevelopmentSettings properties;
    if (!load(properties)) {
        properties.enableOnBoot = enable;
        properties.idleCheckIntervalSeconds = DEFAULT_IDLE_CHECK_INTERVAL_SECONDS;
    } else {
        properties.enableOnBoot = enable;
    }
    if (!save(properties)) {
        TT_LOG_E(TAG, "Failed to save %s", SETTINGS_FILE);
    }
}

bool shouldEnableOnBoot() {
    DevelopmentSettings settings;
    if (!load(settings)) {
        return false;
    }
    return settings.enableOnBoot;
}

void setIdleCheckIntervalSeconds(uint32_t seconds) {
    DevelopmentSettings settings;
    if (!load(settings)) {
        settings.enableOnBoot = false;
        settings.idleCheckIntervalSeconds = seconds;
    } else {
        settings.idleCheckIntervalSeconds = seconds;
    }
    if (!save(settings)) {
        TT_LOG_E(TAG, "Failed to save %s", SETTINGS_FILE);
    }
}

uint32_t getIdleCheckIntervalSeconds() {
    DevelopmentSettings settings;
    if (!load(settings)) {
        return DEFAULT_IDLE_CHECK_INTERVAL_SECONDS;
    }
    return settings.idleCheckIntervalSeconds;
}
}

#endif // ESP_PLATFORM
