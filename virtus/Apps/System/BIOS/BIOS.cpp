#include "Apps/System/BIOS/BIOS.h"
#include "Services/WorkManager/WorkManager.h"
#include "Services/TimeService/TimeService.h"

#include <lvgl.h>

int BIOS::mount(lv_obj_t* canvas) {
    work_manager.add_cpu_job([&]() {
        time_service.sync_via_bluetooth(10);
    });

    // lv_obj_set_size(canvas, LV_HOR_RES, LV_VER_RES);
    // lv_obj_center(canvas);
    lv_obj_set_flex_flow(canvas, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(canvas, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(canvas, 12, 0);

    lv_obj_t* label = lv_label_create(canvas);
    lv_label_set_text(label, "Cold Boot Menu");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(label, LV_PCT(100));

    // Create the button
    lv_obj_t* button = lv_btn_create(canvas);
    lv_obj_set_size(button, 120, 40);

    // Define a style for the button
    static lv_style_t btn_style;
    lv_style_init(&btn_style);
    lv_style_set_bg_opa(&btn_style, LV_OPA_COVER);          // Fully opaque background
    lv_style_set_bg_color(&btn_style, lv_color_hex(0x007BFF)); // Default: bright blue (#007BFF)
    lv_style_set_radius(&btn_style, 8);                     // Rounded corners
    lv_style_set_shadow_width(&btn_style, 10);              // Slight shadow for depth
    lv_style_set_shadow_color(&btn_style, lv_color_hex(0x808080)); // Medium gray shadow (#808080)
    lv_style_set_shadow_ofs_y(&btn_style, 2);

    // Define a pressed state style
    static lv_style_t btn_pressed_style;
    lv_style_init(&btn_pressed_style);
    lv_style_set_bg_opa(&btn_pressed_style, LV_OPA_COVER);
    lv_style_set_bg_color(&btn_pressed_style, lv_color_hex(0x0059B3)); // Darker blue on press (#0059B3)

    // Apply styles to the button
    lv_obj_add_style(button, &btn_style, LV_STATE_DEFAULT);        // Default state
    lv_obj_add_style(button, &btn_pressed_style, LV_STATE_PRESSED); // Pressed state

    // Add label to the button
    lv_obj_t* btn_label = lv_label_create(button);
    lv_label_set_text(btn_label, "Get Started");
    lv_obj_center(btn_label);
    lv_obj_set_style_text_color(btn_label, lv_color_white(), 0);   // White text for contrast

    return 0;
}

int BIOS::unmount(lv_obj_t* canvas) {
    lv_obj_clean(canvas);
    return 0;
}

