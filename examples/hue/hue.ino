/*
 * hue.ino
 *
 * Bang library example sketch to control Hue lights using curl commands.
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
//#define  USE_FTDI

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

#define HUE_BRIDGE_IP "192.168.1.2"    // Replace with your Hue Bridge IP
#define YOUR_USERNAME "your_username"  // Replace with your Hue Bridge username

// change to match your Hue Bridge group numbers
#define LIGHT_ID_1 1
#define LIGHT_ID_2 2
#define GROUP_ID 1

// Function to create and format the command string
String formatCommand(String apiEndpoint, String method, String additionalParams = "") {
    return "curl -s -X " + method + " -H \"Content-Type: application/json\" -d '" + additionalParams +
           "' http://" + HUE_BRIDGE_IP + "/api/" + YOUR_USERNAME + "/" + apiEndpoint;
}

// Function to control the state of a light
void controlLightState(int lightId, String command) {
    String apiEndpoint = "lights/" + String(lightId) + "/state";
    bang.exec(formatCommand(apiEndpoint, "PUT", command).c_str());
}

// Function to control the state of a group
void controlGroupState(int groupId, String command) {
    String apiEndpoint = "groups/" + String(groupId) + "/action";
    bang.exec(formatCommand(apiEndpoint, "PUT", command).c_str());
}

// Function to get information about the Hue Bridge
void getBridgeInfo() {
    bang.exec(formatCommand("", "GET").c_str());
}

void setup() {
    Serial.begin(38400);

    // Turn on light 1
    controlLightState(LIGHT_ID_1, "{\"on\":true}");
    delay(5000);

    // Dim light 1
    controlLightState(LIGHT_ID_1, "{\"bri\":100}");
    delay(5000);

    // Turn off light 1
    controlLightState(LIGHT_ID_1, "{\"on\":false}");
    delay(5000);

    // Turn on light 2
    controlLightState(LIGHT_ID_2, "{\"on\":true}");
    delay(5000);

    // Dim light 2
    controlLightState(LIGHT_ID_2, "{\"bri\":100}");
    delay(5000);

    // Turn off light 2
    controlLightState(LIGHT_ID_2, "{\"on\":false}");
    delay(5000);

    // Get information about the Hue Bridge
    getBridgeInfo();
    delay(5000);

    // Set state of group 1 (turn on)
    controlGroupState(GROUP_ID, "{\"on\":true}");
    delay(5000);

    // Set state of group 1 (dim)
    controlGroupState(GROUP_ID, "{\"bri\":50}");
    delay(5000);

    // Set state of group 1 (turn off)
    controlGroupState(GROUP_ID, "{\"on\":false}");
}

void loop() {}
