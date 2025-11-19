#include "Trackball.h"
#include <esp_log.h>

static const char* TAG = "Trackball";

namespace driver::trackball {

static TrackballConfig g_config;
static lv_indev_t* g_indev = nullptr;
static bool g_initialized = false;
static bool g_enabled = true;

// Track last GPIO states for edge detection
static bool g_lastState[5] = {false, false, false, false, false};
// Track cursor position
static int16_t g_cursorX = 160;  // Center of typical 320px screen
static int16_t g_cursorY = 120;  // Center of typical 240px screen

static void read_cb(lv_indev_t* indev, lv_indev_data_t* data) {
    if (!g_initialized || !g_enabled) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }
    
    const gpio_num_t pins[5] = {
        g_config.pinRight,
        g_config.pinUp,
        g_config.pinLeft,
        g_config.pinDown,
        g_config.pinClick
    };
    
    bool clickPressed = false;
    
    // Read GPIO states and detect changes
    for (int i = 0; i < 5; i++) {
        bool currentState = gpio_get_level(pins[i]) == 0; // Active low with pull-up
        
        if (currentState != g_lastState[i]) {
            g_lastState[i] = currentState;
            
            if (currentState) { // Button pressed
                switch (i) {
                    case 0: // Right
                        if (g_cursorX < (lv_disp_get_hor_res(nullptr) - 1)) {
                            g_cursorX += g_config.movementStep;
                        }
                        break;
                    case 1: // Up
                        if (g_cursorY > 0) {
                            g_cursorY -= g_config.movementStep;
                        }
                        break;
                    case 2: // Left
                        if (g_cursorX > 0) {
                            g_cursorX -= g_config.movementStep;
                        }
                        break;
                    case 3: // Down
                        if (g_cursorY < (lv_disp_get_ver_res(nullptr) - 1)) {
                            g_cursorY += g_config.movementStep;
                        }
                        break;
                    case 4: // Click
                        clickPressed = true;
                        break;
                }
            }
        }
    }
    
    // Clamp cursor to screen bounds
    if (g_cursorX < 0) g_cursorX = 0;
    if (g_cursorY < 0) g_cursorY = 0;
    if (g_cursorX >= lv_disp_get_hor_res(nullptr)) {
        g_cursorX = lv_disp_get_hor_res(nullptr) - 1;
    }
    if (g_cursorY >= lv_disp_get_ver_res(nullptr)) {
        g_cursorY = lv_disp_get_ver_res(nullptr) - 1;
    }
    
    // Update LVGL data
    data->point.x = g_cursorX;
    data->point.y = g_cursorY;
    data->state = clickPressed || g_lastState[4] ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

lv_indev_t* init(const TrackballConfig& config) {
    if (g_initialized) {
        ESP_LOGW(TAG, "Trackball already initialized");
        return g_indev;
    }
    
    g_config = config;
    
    // Set default movement step if not specified
    if (g_config.movementStep == 0) {
        g_config.movementStep = 10;
    }
    
    // Configure all GPIO pins as inputs with pull-ups (active low)
    const gpio_num_t pins[5] = {
        config.pinRight,
        config.pinUp,
        config.pinLeft,
        config.pinDown,
        config.pinClick
    };
    
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    
    for (int i = 0; i < 5; i++) {
        io_conf.pin_bit_mask = (1ULL << pins[i]);
        gpio_config(&io_conf);
        g_lastState[i] = gpio_get_level(pins[i]) == 0;
    }
    
    // Register as LVGL pointer input device
    g_indev = lv_indev_create();
    lv_indev_set_type(g_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(g_indev, read_cb);
    
    if (g_indev) {
        g_initialized = true;
        ESP_LOGI(TAG, "Trackball initialized (R:%d U:%d L:%d D:%d Click:%d, Step:%d)",
                 config.pinRight, config.pinUp, config.pinLeft, config.pinDown,
                 config.pinClick, config.movementStep);
        return g_indev;
    } else {
        ESP_LOGE(TAG, "Failed to register LVGL input device");
        return nullptr;
    }
}

void deinit() {
    if (g_indev) {
        lv_indev_delete(g_indev);
        g_indev = nullptr;
    }
    g_initialized = false;
    ESP_LOGI(TAG, "Trackball deinitialized");
}

void setMovementStep(uint8_t step) {
    if (step > 0) {
        g_config.movementStep = step;
        ESP_LOGD(TAG, "Movement step set to %d", step);
    }
}

void setEnabled(bool enabled) {
    g_enabled = enabled;
    ESP_LOGI(TAG, "Trackball %s", enabled ? "enabled" : "disabled");
}

}
