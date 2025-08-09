#include "config.h"
#include <FatFS.h>
#include "files.h"

static bool is_true(String value) {
    value.toLowerCase();
    return value == "true";
}

static void parse_config_line(Config& config, String& line) {
    if(line.length() == 0 || line[0] == '#') return;
    int equals = line.indexOf('=');
    if(equals == -1) return;
    String optname = line.substring(0, equals);
    String value = line.substring(equals + 1);
    if(optname == "name") config.script_name = value;
    else if(optname == "loop") config.loop = is_true(value);
    else if(optname == "remount") config.remount = is_true(value);
    else if(optname == "scripts_dir") config.scripts_dir = value;
}

Config read_config() {
    Config config = {
        .script_name = "",
        .loop = false,
        .remount = false,
        .scripts_dir = "scripts",
    };

    File f = FatFS.open(CONFIG_NAME, "r");
    String contents = f.readString();
    f.close();

    String line = "";
    unsigned int index = 0;
    while(index < contents.length()) {
        if(contents[index] == '\r') {}
        else if(contents[index] == '\n') {
            parse_config_line(config, line);
            line = "";
        }
        else line += contents[index];
        index++;
    }
    parse_config_line(config, line);
    return config;
}