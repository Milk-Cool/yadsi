#include <Arduino.h>
#include <Keyboard.h>
#include <FatFS.h>
#include <FatFSUSB.h>

#include "files.h"
#include "config.h"
#include "interpreter.h"

bool is_executing = false;
bool mountable = true;
bool usb_mounted = false;
bool keyboard_init = false;

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

Config config;

void setup() {
    Serial.begin(115200);
    FatFS.begin();
    init_files();

    config = read_config();

    FatFSUSB.onPlug(on_plug);
    FatFSUSB.onUnplug(on_unplug);
    FatFSUSB.begin();
}

static void throw_error(String error) {
    Serial.println("SYSTEM ERROR!!!");
    Serial.println(error);
}

void loop() {
    if(!is_executing && BOOTSEL) {
        is_executing = true;
        if(mountable) {
            FatFSUSB.unplug();
            FatFSUSB.end();
            mountable = false;
        }
        if(!keyboard_init) {
            keyboard_init = true;
            Keyboard.begin();
        }
        
        String path = "/" + config.scripts_dir + "/" + config.script_name; // TODO: make scripts dir customizable
        if(!FatFS.exists(path)) throw_error("Script doesn't exist!");
        else {
            File f = FatFS.open(path, "r");
            String code = f.readString();
            f.close();
            do {
                interpret(code);
            } while(config.loop);
        }
        if(config.remount) {
            Keyboard.end();
            keyboard_init = false;

            FatFSUSB.begin();
            FatFSUSB.plug();
            mountable = true;
        }
        is_executing = false;
    }
    delay(5);
}