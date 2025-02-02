#include "HEMISPHERE.hpp"


////////////////////////////////////////////////////////////////////////////////
//// Hemisphere Manager
////////////////////////////////////////////////////////////////////////////////

// TOTAL EEPROM SIZE: 4 * 26 bytes
SETTINGS_DECLARE(hemisphere::Preset, static_cast<size_t>(hemisphere::Setting::LAST)) {
    {0, 0, 255, "Applet ID L", NULL, settings::STORAGE_TYPE_U8},
    {0, 0, 255, "Applet ID R", NULL, settings::STORAGE_TYPE_U8},
    {0, 0, 65535, "Data L block 1", NULL, settings::STORAGE_TYPE_U16},
    {0, 0, 65535, "Data R block 1", NULL, settings::STORAGE_TYPE_U16},
    {0, 0, 65535, "Data L block 2", NULL, settings::STORAGE_TYPE_U16},
    {0, 0, 65535, "Data R block 2", NULL, settings::STORAGE_TYPE_U16},
    {0, 0, 65535, "Data L block 3", NULL, settings::STORAGE_TYPE_U16},
    {0, 0, 65535, "Data R block 3", NULL, settings::STORAGE_TYPE_U16},
    {0, 0, 65535, "Data L block 4", NULL, settings::STORAGE_TYPE_U16},
    {0, 0, 65535, "Data R block 4", NULL, settings::STORAGE_TYPE_U16},
    {0, 0, 65535, "Clock data 1", NULL, settings::STORAGE_TYPE_U16},
    {0, 0, 65535, "Clock data 2", NULL, settings::STORAGE_TYPE_U16},
    {0, 0, 65535, "Clock data 3", NULL, settings::STORAGE_TYPE_U16},
    {0, 0, 65535, "Clock data 4", NULL, settings::STORAGE_TYPE_U16}
};

void ReceiveManagerSysEx() {
    if (hemisphere::active_preset)
        hemisphere::active_preset->OnReceiveSysEx();
}

////////////////////////////////////////////////////////////////////////////////
//// O_C App Functions
////////////////////////////////////////////////////////////////////////////////

// App stubs
void HEMISPHERE_init() {
    manager.BaseStart();
}

size_t HEMISPHERE_storageSize() {
    return hemisphere::Preset::storageSize() * hemisphere::kNumPresets;
}

size_t HEMISPHERE_save(void *storage) {
    size_t used = 0;
    for (int i = 0; i < hemisphere::kNumPresets; ++i) {
        used += hemisphere::presets[i].Save(static_cast<char*>(storage) + used);
    }
    return used;
}

size_t HEMISPHERE_restore(const void *storage) {
    size_t used = 0;
    for (int i = 0; i < hemisphere::kNumPresets; ++i) {
        used += hemisphere::presets[i].Restore(static_cast<const char*>(storage) + used);
    }
    manager.Resume();
    return used;
}

void FASTRUN HEMISPHERE_isr() {
    manager.BaseController();
}

void HEMISPHERE_handleAppEvent(oc::AppEvent event) {
    if (event == oc::APP_EVENT_SUSPEND) {
        manager.Suspend();
    }
}

void HEMISPHERE_loop() {} // Essentially deprecated in favor of ISR

void HEMISPHERE_menu() {
    manager.View();
}

void HEMISPHERE_screensaver() {} // Deprecated in favor of screen blanking

void HEMISPHERE_handleButtonEvent(const UI::Event &event) {
    switch (event.type) {
    case UI::EVENT_BUTTON_DOWN:
    case UI::EVENT_BUTTON_PRESS:
        if (event.control == oc::CONTROL_BUTTON_UP || event.control == oc::CONTROL_BUTTON_DOWN) {
            manager.DelegateSelectButtonPush(event);
        } else if (event.control == oc::CONTROL_BUTTON_L || event.control == oc::CONTROL_BUTTON_R) {
            manager.DelegateEncoderPush(event);
        }
        break;

    case UI::EVENT_BUTTON_LONG_PRESS:
        if (event.control == oc::CONTROL_BUTTON_DOWN) manager.ToggleConfigMenu();
        if (event.control == oc::CONTROL_BUTTON_L) manager.ToggleClockRun();
        break;

    default: break;
    }
}

void HEMISPHERE_handleEncoderEvent(const UI::Event &event) {
    manager.DelegateEncoderMovement(event);
}
