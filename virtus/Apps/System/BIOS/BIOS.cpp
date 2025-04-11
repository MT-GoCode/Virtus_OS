#include "Apps/System/BIOS/BIOS.h"
#include "Services/WorkManager/WorkManager.h"
#include "Services/TimeService/TimeService.h"

#include <lvgl.h>

BIOS bios;

Mailbox<int>* BIOS::system_mailbox = nullptr;

void BIOS::setup_system_comm(Mailbox<int>* mailbox){
    system_mailbox = mailbox;
}

void BIOS::ble_button_clicked(lv_event_t * e)
{
    bios.screen_state.selection = "BLE";
    bios.state_changed = true;
}

void BIOS::manual_button_clicked(lv_event_t * e)
{
    bios.screen_state.selection = "Manual";
    bios.state_changed = true;
}


void BIOS::ble_try_again_button_clicked(lv_event_t * e)
{
    work_manager.add_cpu_job([&]() {
        time_service.sync_via_bluetooth(30, bios.mail_time_service);
    });
    bios.screen_state.ble_busy = true;
    bios.state_changed = true;
}

void BIOS::continue_button_clicked(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (bios.screen_state.error) {
        es.println("writing 1 to system mailbox in continue");
        system_mailbox->write(1);
    }
    else {
        es.println("writing 0 to system mailbox in continue");
        system_mailbox->write(0);
    }    
}

int BIOS::mount(lv_obj_t* canvas) {
    if (!canvas) {
        Serial.println("Error: BIOS::mount received null canvas");
        return -1;
    }

    mail_time_service = new Mailbox<int>();
    bios.state_changed = false;

    work_manager.add_cpu_job([&]() {
        time_service.sync_via_bluetooth(30, mail_time_service);
    });

    frame = lv_obj_create(canvas);
    lv_obj_remove_style_all(frame);
    lv_obj_set_width(frame, lv_pct(100));
    lv_obj_set_height(frame, lv_pct(100));
    lv_obj_set_pos(frame, 0, 0);
    lv_obj_set_align(frame, LV_ALIGN_TOP_LEFT);
    lv_obj_set_flex_flow(frame, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(frame, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(frame, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(frame, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(frame, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Splash screen
    splash_container = lv_obj_create(frame);
    lv_obj_remove_style_all(splash_container);
    lv_obj_set_height(splash_container, lv_pct(100));
    lv_obj_set_width(splash_container, lv_pct(100));
    lv_obj_set_align(splash_container, LV_ALIGN_CENTER);

    splash_label = lv_label_create(splash_container);
    lv_obj_set_width(splash_label, LV_SIZE_CONTENT);
    lv_obj_set_height(splash_label, LV_SIZE_CONTENT);
    lv_obj_set_align(splash_label, LV_ALIGN_CENTER);
    lv_label_set_text(splash_label, "Starting...");
    lv_obj_set_style_text_color(splash_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(splash_label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Form title
    form_title_container = lv_obj_create(frame);
    lv_obj_remove_style_all(form_title_container);
    lv_obj_set_height(form_title_container, 30);
    lv_obj_set_width(form_title_container, lv_pct(100));
    lv_obj_set_align(form_title_container, LV_ALIGN_CENTER);
    lv_obj_clear_flag(form_title_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    form_title_label = lv_label_create(form_title_container);
    lv_obj_set_width(form_title_label, LV_SIZE_CONTENT);
    lv_obj_set_height(form_title_label, LV_SIZE_CONTENT);
    lv_obj_set_align(form_title_label, LV_ALIGN_CENTER);
    lv_label_set_text(form_title_label, "Cold Boot Menu");
    lv_obj_clear_flag(form_title_label, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_text_font(form_title_label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(form_title_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Form subtitle
    form_subtitle_container = lv_obj_create(frame);
    lv_obj_remove_style_all(form_subtitle_container);
    lv_obj_set_height(form_subtitle_container, 30);
    lv_obj_set_width(form_subtitle_container, lv_pct(100));
    lv_obj_set_align(form_subtitle_container, LV_ALIGN_CENTER);
    lv_obj_clear_flag(form_subtitle_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    form_subtitle_label = lv_label_create(form_subtitle_container);
    lv_obj_set_width(form_subtitle_label, LV_SIZE_CONTENT);
    lv_obj_set_height(form_subtitle_label, LV_SIZE_CONTENT);
    lv_obj_set_align(form_subtitle_label, LV_ALIGN_CENTER);
    lv_label_set_text(form_subtitle_label, "Time Settings");
    lv_obj_set_style_text_color(form_subtitle_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Form error
    form_error_container = lv_obj_create(frame);
    lv_obj_remove_style_all(form_error_container);
    lv_obj_set_height(form_error_container, 30);
    lv_obj_set_width(form_error_container, lv_pct(100));
    lv_obj_set_align(form_error_container, LV_ALIGN_CENTER);
    lv_obj_clear_flag(form_error_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    form_error_label = lv_label_create(form_error_container);
    lv_obj_set_width(form_error_label, LV_SIZE_CONTENT);
    lv_obj_set_height(form_error_label, LV_SIZE_CONTENT);
    lv_obj_set_align(form_error_label, LV_ALIGN_CENTER);
    lv_label_set_text(form_error_label, "Automatic Sync failed.\nPlease explore options:");
    lv_obj_set_style_text_color(form_error_label, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(form_error_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // BLE button
    form_button_ble_container = lv_obj_create(frame);
    lv_obj_remove_style_all(form_button_ble_container);
    lv_obj_set_height(form_button_ble_container, 50);
    lv_obj_set_width(form_button_ble_container, lv_pct(100));
    lv_obj_set_align(form_button_ble_container, LV_ALIGN_CENTER);
    lv_obj_clear_flag(form_button_ble_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    form_button_ble_button = lv_btn_create(form_button_ble_container);
    lv_obj_set_height(form_button_ble_button, 30);
    lv_obj_set_width(form_button_ble_button, lv_pct(70));
    lv_obj_set_align(form_button_ble_button, LV_ALIGN_CENTER);
    lv_obj_add_flag(form_button_ble_button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(form_button_ble_button, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(form_button_ble_button, lv_color_hex(0x313131), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(form_button_ble_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    form_button_ble_button_label = lv_label_create(form_button_ble_button);
    lv_obj_set_width(form_button_ble_button_label, LV_SIZE_CONTENT);
    lv_obj_set_height(form_button_ble_button_label, LV_SIZE_CONTENT);
    lv_obj_set_align(form_button_ble_button_label, LV_ALIGN_CENTER);
    lv_label_set_text(form_button_ble_button_label, "Bluetooth Time Sync");
    lv_obj_set_style_text_color(form_button_ble_button_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Manual button
    form_button_manual_container = lv_obj_create(frame);
    lv_obj_remove_style_all(form_button_manual_container);
    lv_obj_set_height(form_button_manual_container, 50);
    lv_obj_set_width(form_button_manual_container, lv_pct(100));
    lv_obj_set_align(form_button_manual_container, LV_ALIGN_CENTER);
    lv_obj_clear_flag(form_button_manual_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    form_button_manual_button = lv_btn_create(form_button_manual_container);
    lv_obj_set_height(form_button_manual_button, 30);
    lv_obj_set_width(form_button_manual_button, lv_pct(70));
    lv_obj_set_align(form_button_manual_button, LV_ALIGN_CENTER);
    lv_obj_add_flag(form_button_manual_button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(form_button_manual_button, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(form_button_manual_button, lv_color_hex(0x313131), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(form_button_manual_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    form_button_manual_label = lv_label_create(form_button_manual_button);
    lv_obj_set_width(form_button_manual_label, LV_SIZE_CONTENT);
    lv_obj_set_height(form_button_manual_label, LV_SIZE_CONTENT);
    lv_obj_set_align(form_button_manual_label, LV_ALIGN_CENTER);
    lv_label_set_text(form_button_manual_label, "Manual Setting");
    lv_obj_set_style_text_color(form_button_manual_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Form testing area container
    form_testing_area = lv_obj_create(frame);
    lv_obj_remove_style_all(form_testing_area);
    lv_obj_set_width(form_testing_area, lv_pct(100));
    lv_obj_set_height(form_testing_area, LV_SIZE_CONTENT);
    lv_obj_set_align(form_testing_area, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(form_testing_area, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(form_testing_area, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(form_testing_area, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    // BLE testing area
    form_testing_area_ble = lv_obj_create(form_testing_area);
    lv_obj_remove_style_all(form_testing_area_ble);
    lv_obj_set_height(form_testing_area_ble, LV_SIZE_CONTENT);
    lv_obj_set_width(form_testing_area_ble, lv_pct(100));
    lv_obj_set_align(form_testing_area_ble, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(form_testing_area_ble, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(form_testing_area_ble, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    ble_info_container = lv_obj_create(form_testing_area_ble);
    lv_obj_remove_style_all(ble_info_container);
    lv_obj_set_height(ble_info_container, 50);
    lv_obj_set_width(ble_info_container, lv_pct(100));
    lv_obj_set_align(ble_info_container, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ble_info_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    ble_info_label = lv_label_create(ble_info_container);
    lv_obj_set_width(ble_info_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ble_info_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ble_info_label, LV_ALIGN_CENTER);
    lv_label_set_text(ble_info_label, "Please install the XYZ app on\nyour phone to to sync time.");
    lv_obj_set_style_text_color(ble_info_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);

    try_again_button_container = lv_obj_create(form_testing_area_ble);
    lv_obj_remove_style_all(try_again_button_container);
    lv_obj_set_height(try_again_button_container, 50);
    lv_obj_set_width(try_again_button_container, lv_pct(100));
    lv_obj_set_align(try_again_button_container, LV_ALIGN_CENTER);
    lv_obj_clear_flag(try_again_button_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    try_again_button_button = lv_btn_create(try_again_button_container);
    lv_obj_set_height(try_again_button_button, 30);
    lv_obj_set_width(try_again_button_button, lv_pct(70));
    lv_obj_set_align(try_again_button_button, LV_ALIGN_CENTER);
    lv_obj_add_flag(try_again_button_button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(try_again_button_button, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(try_again_button_button, lv_color_hex(0x313131), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(try_again_button_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    try_again_button_label = lv_label_create(try_again_button_button);
    lv_obj_set_width(try_again_button_label, LV_SIZE_CONTENT);
    lv_obj_set_height(try_again_button_label, LV_SIZE_CONTENT);
    lv_obj_set_align(try_again_button_label, LV_ALIGN_CENTER);
    lv_label_set_text(try_again_button_label, "Try again...");
    lv_obj_set_style_text_color(try_again_button_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Manual testing area
    form_testing_area_manual = lv_obj_create(form_testing_area);
    lv_obj_remove_style_all(form_testing_area_manual);
    lv_obj_set_height(form_testing_area_manual, LV_SIZE_CONTENT);
    lv_obj_set_width(form_testing_area_manual, lv_pct(100));
    lv_obj_set_align(form_testing_area_manual, LV_ALIGN_CENTER);
    lv_obj_clear_flag(form_testing_area_manual, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    manual_info_label = lv_label_create(form_testing_area_manual);
    lv_obj_set_width(manual_info_label, LV_SIZE_CONTENT);
    lv_obj_set_height(manual_info_label, LV_SIZE_CONTENT);
    lv_obj_set_align(manual_info_label, LV_ALIGN_CENTER);
    lv_label_set_text(manual_info_label, "Manual not yet supported");
    lv_obj_set_style_text_color(manual_info_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Continue button
    continue_button_container = lv_obj_create(frame);
    lv_obj_remove_style_all(continue_button_container);
    lv_obj_set_height(continue_button_container, 50);
    lv_obj_set_width(continue_button_container, lv_pct(100));
    lv_obj_set_align(continue_button_container, LV_ALIGN_CENTER);
    lv_obj_clear_flag(continue_button_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    continue_button_button = lv_btn_create(continue_button_container);
    lv_obj_set_height(continue_button_button, 30);
    lv_obj_set_width(continue_button_button, lv_pct(90));
    lv_obj_set_align(continue_button_button, LV_ALIGN_CENTER);
    lv_obj_add_flag(continue_button_button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(continue_button_button, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(continue_button_button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(continue_button_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    continue_button_label = lv_label_create(continue_button_button);
    lv_obj_set_width(continue_button_label, LV_SIZE_CONTENT);
    lv_obj_set_height(continue_button_label, LV_SIZE_CONTENT);
    lv_obj_set_align(continue_button_label, LV_ALIGN_CENTER);
    lv_label_set_text(continue_button_label, "Ignore error & continue");
    lv_obj_set_style_text_color(continue_button_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);



    lv_obj_add_event_cb(form_button_ble_button, ble_button_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(form_button_manual_button, manual_button_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(try_again_button_button, ble_try_again_button_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(continue_button_button, continue_button_clicked, LV_EVENT_CLICKED, NULL);

    return 0;
}


void BIOS::update_live_state() {
    int x = -1;
    if (mail_time_service->read(x, 0)) {
        es.println("READ!");
        es.println(x);
        if (x == 0 && screen_state.splash) {
            es.println("read successful time sync! now writing to system mailbox");
            system_mailbox->write(0);
        }
        else if (x == 1 && screen_state.splash) {
            screen_state.splash = false;
            state_changed = true;
        }
        else if (x == 0 && !screen_state.splash) {
            screen_state.error = false;
            screen_state.ble_busy = false;
            state_changed = true;
        }
        else if (x == 1 && !screen_state.splash) {
            screen_state.ble_busy = false;
            state_changed = true;
        }
    }

    if (state_changed) {
        // Splash screen handling
        if (screen_state.splash) {
            lv_obj_add_flag(form_title_container, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(form_subtitle_container, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(form_error_container, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(form_button_ble_container, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(form_button_manual_container, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(form_testing_area, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(continue_button_container, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(splash_container, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(splash_container, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(form_title_container, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(form_subtitle_container, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(form_button_ble_container, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(form_button_manual_container, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(form_testing_area, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(continue_button_container, LV_OBJ_FLAG_HIDDEN);

            // Error handling
            if (screen_state.error) {
                lv_obj_clear_flag(form_error_container, LV_OBJ_FLAG_HIDDEN);
                lv_obj_set_style_bg_color(continue_button_button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_label_set_text(continue_button_label, "Ignore error & continue");
            } else {
                lv_obj_add_flag(form_error_container, LV_OBJ_FLAG_HIDDEN);
                lv_obj_set_style_bg_color(continue_button_button, lv_color_hex(0x007A3D), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_label_set_text(continue_button_label, "Success! Press to Continue");
            }

            // Selection handling
            // Selection handling
            if (screen_state.selection == "BLE") {
                lv_obj_clear_flag(form_testing_area_ble, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(form_testing_area_manual, LV_OBJ_FLAG_HIDDEN);

                // Highlight BLE button
                lv_obj_set_style_bg_color(form_button_ble_button, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);  // Blue
                lv_obj_set_style_bg_color(form_button_manual_button, lv_color_hex(0x313131), LV_PART_MAIN | LV_STATE_DEFAULT); // Grey

                if (screen_state.ble_busy) {
                    lv_label_set_text(try_again_button_label, "Trying...");
                    lv_obj_add_state(try_again_button_button, LV_STATE_DISABLED);
                } else {
                    lv_label_set_text(try_again_button_label, "Try again...");
                    lv_obj_clear_state(try_again_button_button, LV_STATE_DISABLED);
                }
            }
            if (screen_state.selection == "Manual") {
                lv_obj_add_flag(form_testing_area_ble, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(form_testing_area_manual, LV_OBJ_FLAG_HIDDEN);

                // Highlight Manual button
                lv_obj_set_style_bg_color(form_button_manual_button, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);  // Blue
                lv_obj_set_style_bg_color(form_button_ble_button, lv_color_hex(0x313131), LV_PART_MAIN | LV_STATE_DEFAULT); // Grey
            }

        }
        state_changed = false;
    }
}

int BIOS::unmount() {
    delete mail_time_service;
    return 0;
}