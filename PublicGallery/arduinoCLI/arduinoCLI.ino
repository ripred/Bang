/*
 * ArduinoCLI.ino
 * 
 * Example use of using your PC/Mac/Linux machine
 * as a service for your Arduino projects.
 * 
 */

#include <SoftwareSerial.h>

#define  RX_PIN   7
#define  TX_PIN   8

SoftwareSerial command_serial(RX_PIN, TX_PIN);  // RX, TX

String cmd = "";

void setup() {
    Serial.begin(115200);
    command_serial.begin(9600);
}

void loop() {
    if (Serial.available()) {
        cmd = Serial.readString();
        cmd.trim();
        if (cmd.length()) {
            Serial.print(F("User command \""));
            Serial.print(cmd);
            Serial.println(F("\" received"));
            command_serial.println(cmd);
        }
    }

   while (command_serial.available() > 0) {
        String output = command_serial.readString();
        output.trim();
        if (output.length() != 0) {
            Serial.println(output);
        }
    }
}
