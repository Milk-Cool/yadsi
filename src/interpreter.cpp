#include "interpreter.h"
#include <Keyboard.h>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

#define ARGS code, pc

static bool is_end(String& code, unsigned int& pc) {
    return pc >= code.length();
}
static bool is_newline(String& code, unsigned int& pc) {
    return code[pc] == '\n' || code[pc] == '\r';
}
static bool is_space(String& code, unsigned int& pc) {
    return code[pc] == ' ' || is_newline(ARGS);
}
static void rm_spaces(String& code, unsigned int& pc) {
    while(!is_end(ARGS) && is_space(ARGS))
        pc++;
}
static String read_word(String& code, unsigned int& pc) {
    String out;
    while(!is_end(ARGS) && !is_space(ARGS))
        out += code[pc++];
    rm_spaces(ARGS);
    return out;
}
static String read_till_newline(String& code, unsigned int& pc) {
    String out;
    while(!is_end(ARGS) && !is_newline(ARGS))
        out += code[pc++];
    rm_spaces(ARGS);
    return out;
}

static void throw_error(String& code, unsigned int& pc, String error) {
    unsigned int line = 1, last_linebreak = 0;
    for(unsigned int i = 0; !is_end(code, i) && i < pc; i++)
        if(code[i] == '\n') {
            line++;
            last_linebreak = i;
        }
    Serial.println("SCRIPT ERROR!!!");
    Serial.printf("At %d:%d:\n", line, last_linebreak);
    Serial.println(error);
}

static const std::map<String, uint8_t> KEYS PROGMEM = {
    { "UPARROW", KEY_UP_ARROW },
    { "DOWNARROW", KEY_DOWN_ARROW },
    { "LEFTARROW", KEY_LEFT_ARROW },
    { "RIGHTARROW", KEY_RIGHT_ARROW },
    { "PAGEUP", KEY_PAGE_UP },
    { "PAGEDOWN", KEY_PAGE_DOWN },
    { "HOME", KEY_HOME },
    { "END", KEY_END },
    { "INSERT", KEY_INSERT },
    { "DELETE", KEY_DELETE },
    { "BACKSPACE", KEY_BACKSPACE },
    { "TAB", KEY_TAB },
    { "SPACE", ' ' },

    { "ENTER", KEY_RETURN },
    { "ESCAPE", KEY_ESC },
    { "PAUSE", KEY_PAUSE },
    { "BREAK", KEY_PAUSE },
    { "PRINTSCREEN", KEY_PRINT_SCREEN },
    { "MENU", KEY_MENU },
    { "APP", KEY_MENU },
    { "F1", KEY_F1 },
    { "F2", KEY_F2 },
    { "F3", KEY_F3 },
    { "F4", KEY_F4 },
    { "F5", KEY_F5 },
    { "F6", KEY_F6 },
    { "F7", KEY_F7 },
    { "F8", KEY_F8 },
    { "F9", KEY_F9 },
    { "F10", KEY_F10 },
    { "F11", KEY_F11 },
    { "F12", KEY_F12 },

    { "SHIFT", KEY_LEFT_SHIFT },
    { "ALT", KEY_LEFT_ALT },
    { "CONTROL", KEY_LEFT_CTRL },
    { "CTRL", KEY_LEFT_CTRL },
    { "COMMAND", KEY_LEFT_ALT },
    { "WINDOWS", KEY_LEFT_GUI },
    { "GUI", KEY_LEFT_GUI },

    { "LSHIFT", KEY_LEFT_SHIFT },
    { "LALT", KEY_LEFT_ALT },
    { "LCONTROL", KEY_LEFT_CTRL },
    { "LCTRL", KEY_LEFT_CTRL },
    { "LCOMMAND", KEY_LEFT_ALT },
    { "LWINDOWS", KEY_LEFT_GUI },
    { "LGUI", KEY_LEFT_GUI },

    { "RSHIFT", KEY_RIGHT_SHIFT },
    { "RALT", KEY_RIGHT_ALT },
    { "RCONTROL", KEY_RIGHT_CTRL },
    { "RCTRL", KEY_RIGHT_CTRL },
    { "RCOMMAND", KEY_RIGHT_ALT },
    { "RWINDOWS", KEY_RIGHT_GUI },
    { "RGUI", KEY_RIGHT_GUI },
};
static const vector<String> MODIFIERS PROGMEM = {
    "SHIFT",
    "ALT",
    "CONTROL", "CTRL",
    "COMMAND",
    "WINDOWS", "GUI",

    "LSHIFT",
    "LALT",
    "LCONTROL", "LCTRL",
    "LCOMMAND",
    "LWINDOWS", ":GUI",

    "RSHIFT",
    "RALT",
    "RCONTROL", "RCTRL",
    "RCOMMAND",
    "RWINDOWS", "RGUI",

    "INJECT_MOD" // no effect
};
static const String NORMAL_KEYS PROGMEM = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()-_=+[]{};:'\",.<>/?\\|";

void interpret(String code) {
    unsigned int pc = 0;
    while(true) {
        if(is_end(ARGS)) break;
        String cmd = read_word(ARGS);
        if(cmd == "STRING" || cmd.startsWith("STRING_")) { // STRING_POWERSHELL, STRING_BATCH, etc.
            String str = read_till_newline(ARGS);
            Keyboard.print(str);
        } else if(cmd == "STRINGLN" || cmd.startsWith("STRINGLN_")) {
            String str = read_till_newline(ARGS);
            Keyboard.println(str);
        } else if(cmd == "REM") read_till_newline(ARGS);
        else if(cmd == "DELAY") {
            String ms_str = read_till_newline(ARGS);
            int ms = ms_str.toInt();
            delay(ms);
        }
        else if(cmd == "WAIT_FOR_BUTTON_PRESS") while(!BOOTSEL) delay(1);
        #ifdef LED_BUILTIN
        else if(cmd == "LED_ON") digitalWrite(LED_BUILTIN, HIGH);
        else if(cmd == "LED_OFF") digitalWrite(LED_BUILTIN, LOW);
        #endif
        else if(cmd == "HOLD" || cmd == "RELEASE") {
            String key = read_till_newline(ARGS);
            uint8_t keycode;
            if(NORMAL_KEYS.indexOf(key) != -1) keycode = key[0];
            else if(KEYS.find(key) != KEYS.end()) keycode = KEYS.at(key);
            cmd == "HOLD" ? Keyboard.press(keycode) : Keyboard.release(keycode);
        }
        else if(KEYS.find(cmd) != KEYS.end()) {
            String keys_str = cmd + " " + read_till_newline(ARGS);
            vector<String> keys;
            String cur_key = "";
            for(unsigned int i = 0; i < keys_str.length(); i++) {
                if(keys_str[i] == ' ' || keys_str[i] == '-') {
                    keys.push_back(cur_key);
                    cur_key = "";
                } else cur_key += keys_str[i];
            }
            keys.push_back(cur_key);

            // Useful when using this with HOLD/RELEASE
            vector<uint8_t> held;
            for(unsigned int i = 0; i < keys.size(); i++) {
                String key = keys[i];
                uint8_t keycode;
                if(key == "INJECT_MOD") continue;
                else if(NORMAL_KEYS.indexOf(key) != -1) keycode = key[0];
                else if(KEYS.find(key) != KEYS.end()) keycode = KEYS.at(key);
                Keyboard.press(keycode);
                if(i != keys.size() - 1) held.push_back(keycode);
                else Keyboard.release(keycode);
            }
            for(uint8_t keycode : held) Keyboard.release(keycode);
        }
    }
}