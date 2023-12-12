/*
 * isMacAsleep.ino
 *
 * arduinoCLI sketch to determine if your mac is alseep or not
 * and returnthe results to your Arduino.
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
}

void loop() {
    command_serial.println(F("pmset -g ps | grep -q 'Sleeping' && echo 'Sleeping' || echo 'Not sleeping'"));

    String response = command_serial.readString();
    response.trim();

    bool const asleep = (memcmp(response.c_str(), "Not", 3) == 0) ? 0 : 1;
    digitalWrite(LED_BUILTIN, asleep);

//    Serial.println(response);

    delay(1000);
}
