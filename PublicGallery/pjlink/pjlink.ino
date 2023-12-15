/*
 * pjlink.ino
 *
 * arduinoCLI sketch to retrieve information about
 * any pjlink device to your Arduino and allow you to control them!
 * 
 * NOTE:
 * In order to use the pjlink command line tools you will need to install the Python pjlink library:
 * 
 * $ pip install pjlink
 * 
 * Full instructions for the use of the pjlink configuration and cli are available here:
 * https://blog.flowblok.id.au/2012-11/controlling-projectors-with-pjlink.html
 * 
 */

#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#define  RX_PIN     7
#define  TX_PIN     8

// Software Serial object to send the
// commands to the Python Agent
SoftwareSerial command_serial(RX_PIN, TX_PIN);  // RX, TX

void setup() {
    Serial.begin(115200);
    command_serial.begin(9600);

    String command = "pjlink --help";
    command_serial.println(command);

    uint32_t start = millis();
    while (millis() - start < 1500) {
        while (command_serial.available() > 0) {
            String output = command_serial.readString();
            output.trim();
            if (output.length() != 0) {
                Serial.println(output);
            }
        }
    }
}

void loop() { }
