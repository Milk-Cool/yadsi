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

Config config;

void init_config_files() {
    if(!FatFS.exists(CONFIG_NAME)) {
        File f = FatFS.open(CONFIG_NAME, "w");
        String str = CONFIG_CONTENTS;
        for(unsigned int i = 0; i < str.length(); i += 512)
            f.write(str.substring(i, i + 512).c_str()); // f.write writes only 512 bytes
        f.close();
    }
}
void init_scripts_dir() {
    if(!FatFS.exists("/" + config.scripts_dir))
        FatFS.mkdir("/" + config.scripts_dir);
}

void setup() {
    Serial.begin(115200);
    FatFS.begin();

    init_config_files();
    config = read_config();
    init_scripts_dir();

    FatFSUSB.onPlug(on_plug);
    FatFSUSB.onUnplug(on_unplug);
    FatFSUSB.begin();

    #ifdef LED_BUILTIN
    pinMode(LED_BUILTIN, OUTPUT);
    #endif
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