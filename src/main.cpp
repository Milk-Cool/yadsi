#include <Arduino.h>
#include <Keyboard.h>
#include <FatFS.h>
#include <FatFSUSB.h>

#include "files.h"

bool is_executing = false;
bool mountable = true;
bool usb_mounted = false;

void on_plug(uint32_t) {
    FatFS.end();
    usb_mounted = true;
}
void on_unplug(uint32_t) {
    FatFS.begin();
    usb_mounted = false;
}

void init_files() {
    if(!FatFS.exists(CONFIG_NAME)) {
        File f = FatFS.open(CONFIG_NAME, "w");
        f.write(CONFIG_CONTENTS);
        f.close();
    }
    if(!FatFS.exists(SCRIPTS_NAME))
        FatFS.mkdir(SCRIPTS_NAME);
}

void setup() {
    Serial.begin(115200);
    FatFS.begin();
    init_files();
    FatFSUSB.onPlug(on_plug);
    FatFSUSB.onUnplug(on_unplug);
    FatFSUSB.begin();
}

void loop() {
    if(!is_executing && BOOTSEL) {
        is_executing = true;
        if(mountable) {
            FatFSUSB.unplug();
            FatFSUSB.end();
            mountable = false;
        }
    }
    delay(5);
}