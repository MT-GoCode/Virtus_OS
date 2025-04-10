#include "Services/ScreenManager/ScreenManager.h"
#include "Services/TimeService/TimeService.h"
#include "Drivers/Battery/Battery.h"

#include <lvgl.h>
#include "Apps/AppRegistry.h"
#include <Arduino.h>

#include "EasySerial.h"

ScreenManager screen_manager;

int ScreenManager::handle_basic_boot(bool first_time_boot_complete) {
    if (!first_time_boot_complete) {

        state = BASIC_BOOT;

        lv_obj_t* scr = lv_scr_act();      // Get the active screen
        lv_obj_clean(scr);                // Clear any default content

        // Set black background
        lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
        lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

        // Create centered label
        lv_obj_t* label = lv_label_create(scr);
        lv_label_set_text(label, "Virtus OS");
        lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);  // Larger font
        lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_GREEN), 0);  // Green
        lv_obj_center(label);

        // Flush to display
        lv_task_handler();
        delay(2000);
        return 0;
    }
    return 0;
}

int ScreenManager::mount_app(int id) {
    state = APP;

    // Set the current app
    current_app = id;

    // Create a parent frame for the full screen
    lv_obj_t* parent_frame = lv_obj_create(lv_scr_act());
    lv_obj_set_size(parent_frame, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(parent_frame, 0, 0);
    lv_obj_clear_flag(parent_frame, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(parent_frame, 0, 0);
    lv_obj_set_style_border_width(parent_frame, 0, 0);
    lv_obj_set_style_radius(parent_frame, 0, 0);

    // Create a grey bar at the top (30 pixels high)
    lv_obj_t* top_bar = lv_obj_create(parent_frame);
    lv_obj_set_size(top_bar, LV_HOR_RES, 30);
    lv_obj_set_pos(top_bar, 0, 0);
    lv_obj_set_style_bg_color(top_bar, lv_color_hex(0x808080), 0);
    lv_obj_set_style_bg_opa(top_bar, LV_OPA_COVER, 0);
    lv_obj_clear_flag(top_bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(top_bar, 0, 0);
    lv_obj_set_style_radius(top_bar, 0, 0);
    lv_obj_set_style_pad_all(top_bar, 0, 0);

    // Create a left-aligned time label with larger font
    this->time_label = lv_label_create(top_bar);
    struct tm time_info = time_service.get_time(true);
    char time_str[6];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", time_info.tm_hour, time_info.tm_min);
    lv_label_set_text(this->time_label, time_str);
    lv_obj_align(this->time_label, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_set_style_text_color(this->time_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(this->time_label, &lv_font_montserrat_16, 0); // Larger font

    // Create a right-aligned battery label with larger font
    this->battery_label = lv_label_create(top_bar);
    int battery_percent = battery.getBatteryPercent();
    bool is_vbus_in = battery.isVbusIn();
    char battery_str[7];
    snprintf(battery_str, sizeof(battery_str), "%s%d%%", is_vbus_in ? "^" : "", battery_percent);
    lv_label_set_text(this->battery_label, battery_str);
    lv_obj_align(this->battery_label, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_set_style_text_color(this->battery_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(this->battery_label, &lv_font_montserrat_16, 0); // Larger font

    // Create a content frame below the bar for the app
    lv_obj_t* content_frame = lv_obj_create(parent_frame);
    lv_obj_set_size(content_frame, LV_HOR_RES, LV_VER_RES - 30);
    lv_obj_set_pos(content_frame, 0, 30);
    lv_obj_clear_flag(content_frame, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(content_frame, 0, 0);
    lv_obj_set_style_radius(content_frame, 0, 0);
    lv_obj_set_style_pad_all(content_frame, 0, 0);

    // Pass the content frame to the app for rendering
    app_registry[id]->mount(content_frame);
    return 0;
}

void ScreenManager::update_live_state() {
    // Update time
    struct tm time_info = time_service.get_time(true);
    char time_str[6];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", time_info.tm_hour, time_info.tm_min);
    lv_label_set_text(this->time_label, time_str);

    // Update battery
    int battery_percent = battery.getBatteryPercent();
    bool is_vbus_in = battery.isVbusIn();
    char battery_str[7];
    snprintf(battery_str, sizeof(battery_str), "%s%d%%", is_vbus_in ? "^" : "", battery_percent);
    lv_label_set_text(this->battery_label, battery_str);
}


void ScreenManager::step() {
    lv_disp_trig_activity(NULL);
    lv_task_handler();

    if (state == APP) {
        update_live_state();
        app_registry[current_app]->update_live_state();
    }
}