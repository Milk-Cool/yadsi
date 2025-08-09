#pragma once
#include <Arduino.h>

typedef struct {
    String script_name;
} Config;
Config read_config();