/*
 * macSpeechSynthesizer.ino
 * 
 * arduinoCLI sketch to make your Mac speak anything your 
 * Arduino tells it to! 
 * 
 */

#include <SoftwareSerial.h>

#define  RX_PIN     7
#define  TX_PIN     8

// Software Serial object to send the
// commands to the Python Agent
SoftwareSerial command_serial(RX_PIN, TX_PIN);  // RX, TX

void setup() {
    Serial.begin(115200);
    command_serial.begin(9600);

    // the text you want your Mac to speak
    String text = "hello, I am an Arduino.";
    String command = "say " + text;
    command_serial.println(command);
}

void loop() { }
