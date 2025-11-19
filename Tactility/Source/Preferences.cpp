#include <Tactility/Preferences.h>
#ifdef ESP_PLATFORM
#include <nvs_flash.h>
#include <nvs.h>
#include <cstring>
#include <esp_log.h>

namespace tt {

// Helper for error logging
static const char* TAG = "Preferences";

// Open NVS handle
static nvs_handle_t open_nvs(const char* ns) {
    nvs_handle_t handle = 0;
    esp_err_t err = nvs_open(ns, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace '%s': %s", ns, esp_err_to_name(err));
        return 0;
    }
    return handle;
}

bool Preferences::hasBool(const std::string& key) const {
    nvs_handle_t handle = open_nvs(namespace_);
    if (!handle) return false;
    uint8_t value;
    esp_err_t err = nvs_get_u8(handle, key.c_str(), &value);
    nvs_close(handle);
    return err == ESP_OK;
}

bool Preferences::hasInt32(const std::string& key) const {
    nvs_handle_t handle = open_nvs(namespace_);
    if (!handle) return false;
    int32_t value;
    esp_err_t err = nvs_get_i32(handle, key.c_str(), &value);
    nvs_close(handle);
    return err == ESP_OK;
}

bool Preferences::hasInt64(const std::string& key) const {
    nvs_handle_t handle = open_nvs(namespace_);
    if (!handle) return false;
    int64_t value;
    esp_err_t err = nvs_get_i64(handle, key.c_str(), &value);
    nvs_close(handle);
    return err == ESP_OK;
}

bool Preferences::hasString(const std::string& key) const {
    nvs_handle_t handle = open_nvs(namespace_);
    if (!handle) return false;
    size_t len = 0;
    esp_err_t err = nvs_get_str(handle, key.c_str(), nullptr, &len);
    nvs_close(handle);
    return err == ESP_OK && len > 1;
}

bool Preferences::optBool(const std::string& key, bool& out) const {
    nvs_handle_t handle = open_nvs(namespace_);
    if (!handle) return false;
    uint8_t value;
    esp_err_t err = nvs_get_u8(handle, key.c_str(), &value);
    nvs_close(handle);
    if (err == ESP_OK) {
        out = value != 0;
        return true;
    }
    return false;
}

bool Preferences::optInt32(const std::string& key, int32_t& out) const {
    nvs_handle_t handle = open_nvs(namespace_);
    if (!handle) return false;
    int32_t value;
    esp_err_t err = nvs_get_i32(handle, key.c_str(), &value);
    nvs_close(handle);
    if (err == ESP_OK) {
        out = value;
        return true;
    }
    return false;
}

bool Preferences::optInt64(const std::string& key, int64_t& out) const {
    nvs_handle_t handle = open_nvs(namespace_);
    if (!handle) return false;
    int64_t value;
    esp_err_t err = nvs_get_i64(handle, key.c_str(), &value);
    nvs_close(handle);
    if (err == ESP_OK) {
        out = value;
        ESP_LOGD(TAG, "optInt64 key=%s value=%lld", key.c_str(), (long long)value);
        return true;
    }
    return false;
}

bool Preferences::optString(const std::string& key, std::string& out) const {
    nvs_handle_t handle = open_nvs(namespace_);
    if (!handle) return false;
    size_t len = 0;
    esp_err_t err = nvs_get_str(handle, key.c_str(), nullptr, &len);
    if (err != ESP_OK || len < 1) {
        nvs_close(handle);
        return false;
    }
    char* buf = new char[len];
    err = nvs_get_str(handle, key.c_str(), buf, &len);
    if (err == ESP_OK) {
        out.assign(buf);
        delete[] buf;
        nvs_close(handle);
        return true;
    }
    delete[] buf;
    nvs_close(handle);
    return false;
}

void Preferences::putBool(const std::string& key, bool value) {
    nvs_handle_t handle = open_nvs(namespace_);
    if (!handle) return;
    nvs_set_u8(handle, key.c_str(), value ? 1 : 0);
    nvs_commit(handle);
    nvs_close(handle);
}

void Preferences::putInt32(const std::string& key, int32_t value) {
    nvs_handle_t handle = open_nvs(namespace_);
    if (!handle) return;
    nvs_set_i32(handle, key.c_str(), value);
    nvs_commit(handle);
    nvs_close(handle);
}

void Preferences::putInt64(const std::string& key, int64_t value) {
    nvs_handle_t handle = open_nvs(namespace_);
    if (!handle) return;
    nvs_set_i64(handle, key.c_str(), value);
    nvs_commit(handle);
    nvs_close(handle);
    ESP_LOGD(TAG, "putInt64 key=%s value=%lld", key.c_str(), (long long)value);
}

void Preferences::putString(const std::string& key, const std::string& value) {
    nvs_handle_t handle = open_nvs(namespace_);
    if (!handle) return;
    nvs_set_str(handle, key.c_str(), value.c_str());
    nvs_commit(handle);
    nvs_close(handle);
}

} // namespace tt
#endif
