#include "interpreter.h"
#include <Keyboard.h>

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
        }
    }
}