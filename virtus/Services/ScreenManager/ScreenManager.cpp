#include "Services/ScreenManager/ScreenManager.h"

#include <lvgl.h>
#include "Apps/AppRegistry.h"
#include <Arduino.h>

#include "EasySerial.h"

int ScreenManager::handle_basic_boot(bool first_time_boot_complete) {
    es.println("enter ScreenManager::handle_basic_boot");
    if (!first_time_boot_complete) {
        es.println("enter ScreenManager::handle_basic_boot's if statement");

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
    es.println("entering ScreenManager::mount_app");

    lv_obj_t* frame = lv_obj_create(lv_scr_act());
    lv_obj_set_size(frame, LV_PCT(100), LV_PCT(100));
    lv_obj_clear_flag(frame, LV_OBJ_FLAG_SCROLLABLE);

    app_registry[id]->mount(frame);
    es.println("exiting ScreenManager::mount_app");
    return 0;
}

void ScreenManager::step() {
    lv_disp_trig_activity(NULL);
    lv_task_handler();
}