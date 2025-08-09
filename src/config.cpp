#include "config.h"
#include <FatFS.h>
#include "files.h"

void parse_config_line(Config& config, String& line) {
    if(line.length() == 0 || line[0] == '#') return;
    int equals = line.indexOf('=');
    if(equals == -1) return;
    String optname = line.substring(0, equals);
    String value = line.substring(equals + 1);
    if(optname == "name") config.script_name = value;
}

Config read_config() {
    Config config = {
        .script_name = "",
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