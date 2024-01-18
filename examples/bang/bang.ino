/*
 * bang.ino
 * 
 * Example use of using your PC/Mac/Linux machine
 * as a service for your Arduino projects.
 * 
 */

#include <Arduino.h>
#include <Bang.h>

// Instructions For Use:
// Define the manifest constant USE_FTDI if you want to use a separate FTDI module to issue commands on
//   which frees the Serial output for normal debugging output
// 
// You can use just the Serial port by itself to issue commands and retreive the output
// but then you cannot use the Serial Monitor window for debugging.

// Un-comment to use an FTDI port for commands and the Serial port for debugging output
#define  USE_FTDI

#ifdef  USE_FTDI
#include <SoftwareSerial.h>
enum { RX_PIN = 7, TX_PIN = 8 };
SoftwareSerial command_serial(RX_PIN, TX_PIN);

// class wrapper for the Bang api so far using two serial ports:
Bang bang(command_serial, Serial);

#else

// class wrapper for the Bang api so far using one serial port:
Bang bang(Serial);

#endif

void setup() {
    Serial.begin(115200);
    command_serial.begin(38400);

    bang.exec("echo \"hello, arduino\"");
}

void loop() {
    bang.sync();
}
