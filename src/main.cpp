#include <Arduino.h>

void setup() {
    Serial.begin(115200);
}

void loop() {
    if(BOOTSEL) Serial.println("BOOTSEL!!!");
    delay(200);
}