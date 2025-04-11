#include "Apps/System/DefaultWatchFace/DefaultWatchFace.h"
#include "Services/TimeService/TimeService.h"

#include <lvgl.h>

DefaultWatchFace default_watch_face;

int DefaultWatchFace::mount(lv_obj_t* canvas) {

    frame = lv_obj_create(canvas);
    lv_obj_remove_style_all(frame);
    lv_obj_set_width(frame, lv_pct(100));
    lv_obj_set_height(frame, lv_pct(100));
    lv_obj_set_pos(frame, 0, 0);
    lv_obj_set_align(frame, LV_ALIGN_TOP_LEFT);
    lv_obj_clear_flag(frame, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                      LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);     /// Flags
    lv_obj_set_style_bg_color(frame, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(frame, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    full_time_container = lv_obj_create(frame);
    lv_obj_remove_style_all(full_time_container);
    lv_obj_set_width(full_time_container, lv_pct(100));
    lv_obj_set_height(full_time_container, lv_pct(30));
    lv_obj_set_x(full_time_container, 0);
    lv_obj_set_y(full_time_container, lv_pct(-10));
    lv_obj_set_align(full_time_container, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(full_time_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(full_time_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(full_time_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    hh_mm_label = lv_label_create(full_time_container);
    lv_obj_set_x(hh_mm_label, -30);
    lv_obj_set_y(hh_mm_label, 0);
    lv_obj_set_align(hh_mm_label, LV_ALIGN_CENTER);
    lv_label_set_text(hh_mm_label, "44:48");
    lv_obj_set_style_text_color(hh_mm_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(hh_mm_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(hh_mm_label, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);

    am_pm_label = lv_label_create(full_time_container);
    lv_obj_set_x(am_pm_label, -30);
    lv_obj_set_y(am_pm_label, 0);
    lv_obj_set_align(am_pm_label, LV_ALIGN_CENTER);
    lv_label_set_text(am_pm_label, "PM");
    lv_obj_set_style_text_color(am_pm_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(am_pm_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(am_pm_label, &lv_font_montserrat_36, LV_PART_MAIN | LV_STATE_DEFAULT);

    day_date_container = lv_obj_create(frame);
    lv_obj_remove_style_all(day_date_container);
    lv_obj_set_width(day_date_container, lv_pct(100));
    lv_obj_set_height(day_date_container, lv_pct(30));
    lv_obj_set_x(day_date_container, lv_pct(0));
    lv_obj_set_y(day_date_container, lv_pct(20));
    lv_obj_set_align(day_date_container, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(day_date_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(day_date_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(day_date_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    day_label = lv_label_create(day_date_container);
    lv_obj_set_x(day_label, 8);
    lv_obj_set_y(day_label, -1);
    lv_obj_set_align(day_label, LV_ALIGN_TOP_RIGHT);
    lv_label_set_text(day_label, "Monday");
    lv_obj_set_style_text_color(day_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(day_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(day_label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    date_label = lv_label_create(day_date_container);
    lv_obj_set_x(date_label, 8);
    lv_obj_set_y(date_label, -1);
    lv_obj_set_align(date_label, LV_ALIGN_TOP_RIGHT);
    lv_label_set_text(date_label, "11.11.11");
    lv_obj_set_style_text_color(date_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(date_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(date_label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    return 0;
}

void DefaultWatchFace::update_live_state() {
    struct tm time_info = time_service.get_time(true);

    // Format HH:MM (12-hour format, no zero padding)
    char hhmm[12];
    int display_hour = time_info.tm_hour % 12;
    if (display_hour == 0) display_hour = 12;  // handle midnight/noon
    snprintf(hhmm, sizeof(hhmm), "%d:%d", display_hour, time_info.tm_min);
    lv_label_set_text(hh_mm_label, hhmm);

    // Set AM/PM
    lv_label_set_text(am_pm_label, (time_info.tm_hour >= 12) ? "PM" : "AM");

    // Set Day (e.g., Monday)
    const char* day_names[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
    lv_label_set_text(day_label, day_names[time_info.tm_wday]);

    // Format and set date (e.g., 4.11.25) as month.day.year (no padding)
    char date[16];
    snprintf(date, sizeof(date), "%d.%d.%d", time_info.tm_mon + 1, time_info.tm_mday, (time_info.tm_year + 1900) % 100);
    lv_label_set_text(date_label, date);
}

int DefaultWatchFace::unmount() {
    return 0;
}