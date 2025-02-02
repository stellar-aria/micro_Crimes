// Hemisphere Applet Boilerplate. Follow these steps to add a Hemisphere app:
//
// (1) Save this file as HEM_ClassName.ino
// (2) Find and replace "ClassName" with the name of your Applet class
// (3) Implement all of the public methods below
// (4) Add text to the help section below in SetHelp()
// (5) Add a declare line in hemisphere_config.h, which looks like this:
//     DECLARE_APPLET(id, categories, ClassName), \
// (6) Increment HEMISPHERE_AVAILABLE_APPLETS in hemisphere_config.h
// (7) Add your name and any additional copyright info to the block below

// Copyright (c) 2018, __________
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include "hemisphere/applet_base.hpp"
using namespace hemisphere;

class ICONS : public AppletBase {
public:

    const char* applet_name() {
        return "ICONS";
    }

    void Start() {
    }

    void Controller() {
    }

    void View() {
        gfxHeader(applet_name());
        DrawInterface();
    }

    void OnButtonPress() {
    }

    void OnEncoderMove(int direction) {
    }
        
    uint64_t OnDataRequest() {
        uint64_t data = 0;
        // example: pack property_name at bit 0, with size of 8 bits
        // Pack(data, PackLocation {0,8}, property_name); 
        return data;
    }

    void OnDataReceive(uint64_t data) {
        // example: unpack value at bit 0 with size of 8 bits to property_name
        // property_name = Unpack(data, PackLocation {0,8}); 
    }

protected:
    void SetHelp() {
        //                               "------------------" <-- Size Guide
        help[HEMISPHERE_HELP_DIGITALS] = "Digital in help";
        help[HEMISPHERE_HELP_CVS]      = "CV in help";
        help[HEMISPHERE_HELP_OUTS]     = "Out help";
        help[HEMISPHERE_HELP_ENCODER]  = "123456789012345678";
        //                               "------------------" <-- Size Guide
    }
    
private:
    int cursor;
    const uint8_t *icons[38] = {
        METER_ICON,
        CLOCK_ICON,
        MOD_ICON,
        BEND_ICON,
        AFTERTOUCH_ICON,
        MIDI_ICON,
        CV_ICON,
        SCALE_ICON,
        LOCK_ICON,
        FAVORITE_ICON,
        ROTATE_L_ICON,
        ROTATE_R_ICON,
        MONITOR_ICON,
        AUDITION_ICON,
        LINK_ICON,
        CHECK_OFF_ICON,
        CHECK_ON_ICON,
        CHECK_ICON,
        LOOP_ICON,
        PLAYONCE_ICON,
        PLAY_ICON,
        PAUSE_ICON,
        RESET_ICON,
        RECORD_ICON,
        STOP_ICON,
        UP_BTN_ICON,
        DOWN_BTN_ICON,
        LEFT_BTN_ICON,
        RIGHT_BTN_ICON,
        METRO_L_ICON,
        METRO_R_ICON,
        X_NOTE_ICON,
        NOTE_ICON,
        NOTE4_ICON,
        UP_DOWN_ICON,
        LEFT_RIGHT_ICON,
        SEGMENT_ICON,
        WAVEFORM_ICON
    };
    
    void DrawInterface() {
        gfxSkyline();
        for (int i = 0; i < 35; i++) {
          int x = i * 9 % 63;
          int y = (i / 7 * 9) + 14;
          gfxIcon(x, y, icons[i]);
        }
    }
};


////////////////////////////////////////////////////////////////////////////////
//// Hemisphere Applet Functions
///
///  Once you run the find-and-replace to make these refer to ClassName,
///  it's usually not necessary to do anything with these functions. You
///  should prefer to handle things in the HemisphereApplet child class
///  above.
////////////////////////////////////////////////////////////////////////////////
ICONS ICONS_instance[2];

void ICONS_Start(bool hemisphere) {ICONS_instance[hemisphere].BaseStart(hemisphere);}
void ICONS_Controller(bool hemisphere, bool forwarding) {ICONS_instance[hemisphere].BaseController(forwarding);}
void ICONS_View(bool hemisphere) {ICONS_instance[hemisphere].BaseView();}
void ICONS_OnButtonPress(bool hemisphere) {ICONS_instance[hemisphere].OnButtonPress();}
void ICONS_OnEncoderMove(bool hemisphere, int direction) {ICONS_instance[hemisphere].OnEncoderMove(direction);}
void ICONS_ToggleHelpScreen(bool hemisphere) {ICONS_instance[hemisphere].HelpScreen();}
uint64_t ICONS_OnDataRequest(bool hemisphere) {return ICONS_instance[hemisphere].OnDataRequest();}
void ICONS_OnDataReceive(bool hemisphere, uint64_t data) {ICONS_instance[hemisphere].OnDataReceive(data);}
