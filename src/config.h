#pragma once
#include <Arduino.h>

typedef struct {
    String script_name;
    bool loop;
    bool remount;
    String scripts_dir;
} Config;
Config read_config();