#ifdef ESP_PLATFORM

#include <Tactility/app/AppManifest.h>
#include <Tactility/app/alertdialog/AlertDialog.h>
#include <Tactility/lvgl/Lvgl.h>
#include <Tactility/lvgl/LvglSync.h>
#include <Tactility/lvgl/Style.h>
#include <Tactility/lvgl/Toolbar.h>
#include <Tactility/service/development/DevelopmentService.h>
#include <Tactility/service/development/DevelopmentSettings.h>
#include <Tactility/service/loader/Loader.h>
#include <Tactility/service/wifi/Wifi.h>
#include <Tactility/Tactility.h>
#include <Tactility/Timer.h>

#include <cstring>
#include <lvgl.h>
#include <esp_system.h>

namespace tt::app::development {

constexpr const char* TAG = "Development";
extern const AppManifest manifest;

class DevelopmentApp final : public App {

    lv_obj_t* enableSwitch = nullptr;
    lv_obj_t* enableOnBootSwitch = nullptr;
    lv_obj_t* idleCheckSlider = nullptr;
    lv_obj_t* idleCheckValueLabel = nullptr;
    lv_obj_t* statusLabel = nullptr;
    std::shared_ptr<service::development::DevelopmentService> service;
    uint32_t originalIdleCheckInterval = 0;
    LaunchId rebootDialogLaunchId = 0;

    Timer timer = Timer(Timer::Type::Periodic, [this] {
        auto lock = lvgl::getSyncLock()->asScopedLock();
        // TODO: There's a crash when this is called when the app is being destroyed
        if (lock.lock(lvgl::defaultLockTime) && lvgl::isStarted()) {
            updateViewState();
        }
    });

    static void onEnableSwitchChanged(lv_event_t* event) {
        lv_event_code_t code = lv_event_get_code(event);
        auto* widget = static_cast<lv_obj_t*>(lv_event_get_target(event));
        if (code == LV_EVENT_VALUE_CHANGED) {
            bool is_on = lv_obj_has_state(widget, LV_STATE_CHECKED);
            auto* app = static_cast<DevelopmentApp*>(lv_event_get_user_data(event));
            bool is_changed = is_on != app->service->isEnabled();
            if (is_changed) {
                app->service->setEnabled(is_on);
            }
        }
    }

    static void onEnableOnBootSwitchChanged(lv_event_t* event) {
        lv_event_code_t code = lv_event_get_code(event);
        auto* widget = static_cast<lv_obj_t*>(lv_event_get_target(event));
        if (code == LV_EVENT_VALUE_CHANGED) {
            bool is_on = lv_obj_has_state(widget, LV_STATE_CHECKED);
            bool is_changed = is_on != service::development::shouldEnableOnBoot();
            if (is_changed) {
                // Dispatch it, so file IO doesn't block the UI
                getMainDispatcher().dispatch([is_on] {
                    service::development::setEnableOnBoot(is_on);
                });
            }
        }
    }

    static void onIdleCheckSliderChanged(lv_event_t* event) {
        lv_event_code_t code = lv_event_get_code(event);
        auto* slider = static_cast<lv_obj_t*>(lv_event_get_target(event));
        auto* app = static_cast<DevelopmentApp*>(lv_event_get_user_data(event));
        
        if (code == LV_EVENT_VALUE_CHANGED) {
            int32_t value = lv_slider_get_value(slider);
            lv_label_set_text_fmt(app->idleCheckValueLabel, "%ld seconds", value);
        } else if (code == LV_EVENT_RELEASED) {
            int32_t value = lv_slider_get_value(slider);
            uint32_t newInterval = static_cast<uint32_t>(value);
            
            if (newInterval != app->originalIdleCheckInterval) {
                // Save the setting
                getMainDispatcher().dispatch([newInterval, app] {
                    service::development::setIdleCheckIntervalSeconds(newInterval);
                    
                    // Show reboot dialog
                    app->rebootDialogLaunchId = alertdialog::start(
                        "Reboot Required",
                        "Idle check interval changed. Reboot to apply the new setting.",
                        std::vector<const char*>{"Reboot Now", "Later"}
                    );
                });
            }
        }
    }

    void updateViewState() {
        if (!service->isEnabled()) {
            lv_label_set_text(statusLabel, "Service disabled");
        } else if (service::wifi::getRadioState() != service::wifi::RadioState::ConnectionActive) {
            lv_label_set_text(statusLabel, "Waiting for connection...");
        } else { // enabled and connected to wifi
            auto ip = service::wifi::getIp();
            if (ip.empty()) {
                lv_label_set_text(statusLabel, "Waiting for IP...");
            } else {
                const std::string status = std::format("Available at {}", ip);
                lv_label_set_text(statusLabel, status.c_str());
            }
        }
    }

public:

    void onCreate(AppContext& appContext) override {
        service = service::development::findService();
        if (service == nullptr) {
            TT_LOG_E(TAG, "Service not found");
            stop(manifest.appId);
        }
        
        // Store the original idle check interval
        originalIdleCheckInterval = service::development::getIdleCheckIntervalSeconds();
    }

    void onShow(AppContext& app, lv_obj_t* parent) override {
        lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_row(parent, 0, LV_STATE_DEFAULT);

        lv_obj_t* toolbar = lvgl::toolbar_create(parent, app);

        enableSwitch = lvgl::toolbar_add_switch_action(toolbar);
        lv_obj_add_event_cb(enableSwitch, onEnableSwitchChanged, LV_EVENT_VALUE_CHANGED, this);

        if (service->isEnabled()) {
            lv_obj_add_state(enableSwitch, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(enableSwitch, LV_STATE_CHECKED);
        }

        // Wrappers

        lv_obj_t* content_wrapper = lv_obj_create(parent);
        lv_obj_set_width(content_wrapper, LV_PCT(100));
        lv_obj_set_flex_grow(content_wrapper, 1);
        lv_obj_set_flex_flow(content_wrapper, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_border_width(content_wrapper, 0, LV_STATE_DEFAULT);
        lvgl::obj_set_style_bg_invisible(content_wrapper);

        // Enable on boot

        lv_obj_t* enable_wrapper = lv_obj_create(content_wrapper);
        lv_obj_set_size(enable_wrapper, LV_PCT(100), LV_SIZE_CONTENT);
        lvgl::obj_set_style_bg_invisible(enable_wrapper);
        lv_obj_set_style_border_width(enable_wrapper, 0, LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(enable_wrapper, 0, LV_STATE_DEFAULT);

        lv_obj_t* enable_label = lv_label_create(enable_wrapper);
        lv_label_set_text(enable_label, "Enable on boot");
        lv_obj_align(enable_label, LV_ALIGN_LEFT_MID, 0, 0);

        enableOnBootSwitch = lv_switch_create(enable_wrapper);
        lv_obj_add_event_cb(enableOnBootSwitch, onEnableOnBootSwitchChanged, LV_EVENT_VALUE_CHANGED, this);
        lv_obj_align(enableOnBootSwitch, LV_ALIGN_RIGHT_MID, 0, 0);
        if (service::development::shouldEnableOnBoot()) {
            lv_obj_add_state(enableOnBootSwitch, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(enableOnBootSwitch, LV_STATE_CHECKED);
        }

        // Idle check interval slider

        lv_obj_t* idle_check_wrapper = lv_obj_create(content_wrapper);
        lv_obj_set_size(idle_check_wrapper, LV_PCT(100), LV_SIZE_CONTENT);
        lvgl::obj_set_style_bg_invisible(idle_check_wrapper);
        lv_obj_set_style_border_width(idle_check_wrapper, 0, LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(idle_check_wrapper, 4, LV_STATE_DEFAULT);
        lv_obj_set_flex_flow(idle_check_wrapper, LV_FLEX_FLOW_COLUMN);

        lv_obj_t* idle_check_label_wrapper = lv_obj_create(idle_check_wrapper);
        lv_obj_set_size(idle_check_label_wrapper, LV_PCT(100), LV_SIZE_CONTENT);
        lvgl::obj_set_style_bg_invisible(idle_check_label_wrapper);
        lv_obj_set_style_border_width(idle_check_label_wrapper, 0, LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(idle_check_label_wrapper, 0, LV_STATE_DEFAULT);

        lv_obj_t* idle_check_label = lv_label_create(idle_check_label_wrapper);
        lv_label_set_text(idle_check_label, "Idle check interval");
        lv_obj_align(idle_check_label, LV_ALIGN_LEFT_MID, 0, 0);

        idleCheckValueLabel = lv_label_create(idle_check_label_wrapper);
        lv_label_set_text_fmt(idleCheckValueLabel, "%lu seconds", originalIdleCheckInterval);
        lv_obj_align(idleCheckValueLabel, LV_ALIGN_RIGHT_MID, 0, 0);

        idleCheckSlider = lv_slider_create(idle_check_wrapper);
        lv_obj_set_width(idleCheckSlider, LV_PCT(100));
        lv_slider_set_range(idleCheckSlider, 1, 60); // 1 to 60 seconds
        lv_slider_set_value(idleCheckSlider, originalIdleCheckInterval, LV_ANIM_OFF);
        lv_obj_add_event_cb(idleCheckSlider, onIdleCheckSliderChanged, LV_EVENT_VALUE_CHANGED, this);
        lv_obj_add_event_cb(idleCheckSlider, onIdleCheckSliderChanged, LV_EVENT_RELEASED, this);

        // Status

        statusLabel = lv_label_create(content_wrapper);

        // Warning

        auto warning_label = lv_label_create(content_wrapper);
        lv_label_set_text(warning_label, "This feature is experimental and uses an unsecured http connection.");
        lv_obj_set_width(warning_label, LV_PCT(100));
        lv_label_set_long_mode(warning_label, LV_LABEL_LONG_WRAP);
        lv_obj_set_style_text_color(warning_label, lv_color_make(0xff, 0xff, 00), LV_STATE_DEFAULT);

        updateViewState();

        timer.start(1000);
    }

    void onHide(AppContext& appContext) override {
        auto lock = lvgl::getSyncLock()->asScopedLock();
        // Ensure that the update isn't already happening
        lock.lock();
        timer.stop();
    }

    void onResult(TT_UNUSED AppContext& appContext, LaunchId launchId, TT_UNUSED Result result, std::unique_ptr<Bundle> bundle) override {
        if (launchId != rebootDialogLaunchId || result != Result::Ok || bundle == nullptr) {
            return;
        }

        int32_t buttonIndex = alertdialog::getResultIndex(*bundle);
        if (buttonIndex == 0) {
            // User selected "Reboot Now"
            esp_restart();
        } else {
            // User selected "Later" - update the original value so we don't keep prompting
            originalIdleCheckInterval = service::development::getIdleCheckIntervalSeconds();
        }
    }
};

extern const AppManifest manifest = {
    .appId = "Development",
    .appName = "Development",
    .appCategory = Category::Settings,
    .createApp = create<DevelopmentApp>
};

void start() {
    app::start(manifest.appId);
}

} // namespace

#endif // ESP_PLATFORM