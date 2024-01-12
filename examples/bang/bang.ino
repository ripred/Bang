/*
 * bang.ino
 * 
 * Example use of using your PC/Mac/Linux machine
 * as a service for your Arduino projects.
 * 
 */

#include <SoftwareSerial.h>
#include <Bang.h>

#define  RX_PIN   7
#define  TX_PIN   8

SoftwareSerial command_serial(RX_PIN, TX_PIN);  // RX, TX

// class wrapper for the Bang api so far:
Bang bang(command_serial, Serial);

void setup() {
    Serial.begin(115200);
    command_serial.begin(9600);

    bang.serial("\nexecutable lines should start with a bang ! character as in:");
    bang.serial("    !echo hello, arduino!");

    bang.serial("macro lines should start with an @ character as in:");
    bang.serial("    @list_macros");

    bang.serial("'compile and reload' using the & character as in:");
    bang.serial("    &blink\n");
}

void loop() {
    bang.sync();
}
