#define HEM_COMPARE_MAX_VALUE 255

class Compare : public HemisphereApplet {
public:

    const char* applet_name() {
        return "Compare";
    }

    void Start() {
        level = 128;
        mod_cv = 0;
    }

    void Controller() {
        int cv_level = Proportion(level, HEM_COMPARE_MAX_VALUE, HEMISPHERE_MAX_CV);
        mod_cv = cv_level + In(1);
        mod_cv = constrain(mod_cv, 0, HEMISPHERE_MAX_CV);

        if (In(0) > mod_cv) {
            in_greater = 1;
            GateOut(0, 1);
            GateOut(1, 0);
        } else {
            in_greater = 0;
            GateOut(1, 1);
            GateOut(0, 0);
        }
    }

    void View() {
        gfxHeader(applet_name());
        DrawInterface();
    }

    void ScreensaverView() {
        DrawInterface();
    }

    void OnButtonPress() {
    }

    void OnEncoderMove(int direction) {
        level = constrain(level += direction, 0, HEM_COMPARE_MAX_VALUE);
    }
        
    uint32_t OnDataRequest() {
        uint32_t data = 0;
        Pack(data, PackLocation {0,8}, level);
        return data;
    }

    void OnDataReceive(uint32_t data) {
        level = Unpack(data, PackLocation {0,8});
    }

protected:
    void SetHelp() {
        //                               "------------------" <-- Size Guide
        help[HEMISPHERE_HELP_DIGITALS] = "";
        help[HEMISPHERE_HELP_CVS]      = "1=CV1 2=Mod Level";
        help[HEMISPHERE_HELP_OUTS]     = "A=CV1>Lv B=Lv>=CV1";
        help[HEMISPHERE_HELP_ENCODER]  = "Set Level";
        //                               "------------------" <-- Size Guide
    }
    
private:
    int level;
    int mod_cv; // Modified CV used in comparison
    bool in_greater; // Result of last comparison

    void DrawInterface() {
        // Draw currently-selected level
        gfxFrame(1, 15, 62, 6);
        int x = Proportion(level, HEM_COMPARE_MAX_VALUE, 62);
        gfxLine(x, 15, x, 20);

        // Draw comparison
        if (in_greater) gfxRect(1, 35, ProportionCV(ViewIn(0), 62), 6);
        else gfxFrame(1, 35, ProportionCV(ViewIn(0), 62), 6);

        if (!in_greater) gfxRect(1, 45, ProportionCV(mod_cv, 62), 6);
        else gfxFrame(1, 45, ProportionCV(mod_cv, 62), 6);
    }
};


////////////////////////////////////////////////////////////////////////////////
//// Hemisphere Applet Functions
///
///  Once you run the find-and-replace to make these refer to Compare,
///  it's usually not necessary to do anything with these functions. You
///  should prefer to handle things in the HemisphereApplet child class
///  above.
////////////////////////////////////////////////////////////////////////////////
Compare Compare_instance[2];

void Compare_Start(int hemisphere) {
    Compare_instance[hemisphere].BaseStart(hemisphere);
}

void Compare_Controller(int hemisphere, bool forwarding) {
    Compare_instance[hemisphere].BaseController(forwarding);
}

void Compare_View(int hemisphere) {
    Compare_instance[hemisphere].BaseView();
}

void Compare_Screensaver(int hemisphere) {
    Compare_instance[hemisphere].BaseScreensaverView();
}

void Compare_OnButtonPress(int hemisphere) {
    Compare_instance[hemisphere].OnButtonPress();
}

void Compare_OnEncoderMove(int hemisphere, int direction) {
    Compare_instance[hemisphere].OnEncoderMove(direction);
}

void Compare_ToggleHelpScreen(int hemisphere) {
    Compare_instance[hemisphere].HelpScreen();
}

uint32_t Compare_OnDataRequest(int hemisphere) {
    return Compare_instance[hemisphere].OnDataRequest();
}

void Compare_OnDataReceive(int hemisphere, uint32_t data) {
    Compare_instance[hemisphere].OnDataReceive(data);
}
