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

    es.println("screen manager asked to mount ");

    if (current_app != -1) {
        app_registry[current_app]->unmount();
    }

    es.println(id);

    full_screen = lv_obj_create(NULL);
    lv_obj_clear_flag(full_screen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    tob_bar_container = lv_obj_create(full_screen);
    lv_obj_remove_style_all(tob_bar_container);
    lv_obj_set_width(tob_bar_container, lv_pct(100));
    lv_obj_set_height(tob_bar_container, lv_pct(100));
    lv_obj_set_align(tob_bar_container, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(tob_bar_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(tob_bar_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(tob_bar_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                      LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                      LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);     /// Flags

    top_bar = lv_obj_create(tob_bar_container);
    lv_obj_set_width(top_bar, lv_pct(100));
    lv_obj_set_height(top_bar, lv_pct(10));
    lv_obj_set_align(top_bar, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(top_bar, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(top_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(top_bar, lv_color_hex(0x424242), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(top_bar, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(top_bar, LV_BORDER_SIDE_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(top_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(top_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(top_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(top_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(top_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(top_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    time_label = lv_label_create(top_bar);
    lv_obj_set_width(time_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(time_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(time_label, LV_ALIGN_LEFT_MID);
    lv_obj_set_style_text_color(time_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);  // Set larger font

    battery_label = lv_label_create(top_bar);
    lv_obj_set_width(battery_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(battery_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(battery_label, LV_ALIGN_RIGHT_MID);
    lv_obj_set_style_text_color(battery_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(battery_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);  // Set larger font

    app_frame = lv_obj_create(tob_bar_container);
    lv_obj_remove_style_all(app_frame);
    lv_obj_set_width(app_frame, lv_pct(100));
    lv_obj_set_height(app_frame, lv_pct(90));
    lv_obj_set_align(app_frame, LV_ALIGN_CENTER);
    lv_obj_clear_flag(app_frame, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_pos(app_frame, 0, lv_pct(10));

    // Pass the content frame to the app for rendering
    app_registry[id]->mount(app_frame);

    lv_scr_load(full_screen);

    // Set the current app
    current_app = id;

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
        if (current_app != -1) {
            app_registry[current_app]->update_live_state();
        }
    }
}