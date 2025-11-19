#include <Tactility/Tactility.h>


#ifdef ESP_PLATFORM
#include <tt_init.h>
#include <nvs_flash.h>
#endif

// Each board project declares this variable
extern const tt::hal::Configuration hardwareConfiguration;

extern "C" {

void app_main() {
    static const tt::Configuration config = {
        /**
         * Auto-select a board based on the ./sdkconfig.board.* file
         * that you copied to ./sdkconfig before you opened this project.
         */
        .hardware = &hardwareConfiguration
    };

    #ifdef ESP_PLATFORM
    // Initialize NVS before any Preferences/NVS usage
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }
    tt_init_tactility_c(); // ELF bindings for side-loading on ESP32
    #endif

    tt::run(config);
}

} // extern