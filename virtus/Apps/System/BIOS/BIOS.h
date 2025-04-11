#ifndef BIOS_H
#define BIOS_H

#include "Apps/BaseApp.h"
#include "Services/Mailbox/Mailbox.h"

class BIOS : public BaseApp {
public:
    virtual int mount(lv_obj_t* canvas);
    virtual int unmount();
    virtual void update_live_state();

    void setup_system_comm(Mailbox<int>* mailbox);

private:
    static Mailbox<int>* system_mailbox;

    struct SCREEN_STATE {
        bool splash = true;

        bool error = true;
        String selection = "BLE"; // 'BLE' or 'Manual' for now
            
        bool ble_busy = false;

    } screen_state;
    bool state_changed;
    Mailbox<int>* mail_time_service;

    static void ble_button_clicked(lv_event_t * e);
    static void manual_button_clicked(lv_event_t * e);
    static void ble_try_again_button_clicked(lv_event_t * e);
    static void continue_button_clicked(lv_event_t * e);
    
    lv_obj_t* frame;
    lv_obj_t* splash_container;
    lv_obj_t* splash_label;
    lv_obj_t* form_title_container;
    lv_obj_t* form_title_label;
    lv_obj_t* form_subtitle_container;
    lv_obj_t* form_subtitle_label;
    lv_obj_t* form_error_container;
    lv_obj_t* form_error_label;
    lv_obj_t* form_button_ble_container;
    lv_obj_t* form_button_ble_button;
    lv_obj_t* form_button_ble_button_label;
    lv_obj_t* form_button_manual_container;
    lv_obj_t* form_button_manual_button;
    lv_obj_t* form_button_manual_label;
    lv_obj_t* form_testing_area;
    lv_obj_t* form_testing_area_ble;
    lv_obj_t* ble_info_container;
    lv_obj_t* ble_info_label;
    lv_obj_t* try_again_button_container;
    lv_obj_t* try_again_button_button;
    lv_obj_t* try_again_button_label;
    lv_obj_t* form_testing_area_manual;
    lv_obj_t* manual_info_label;
    lv_obj_t* continue_button_container;
    lv_obj_t* continue_button_button;
    lv_obj_t* continue_button_label;
};

extern BIOS bios;

#endif