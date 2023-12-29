/*
 * hue.ino
 *
 * ArduinoCLI sketch to control Hue lights using curl commands.
 * 
 */

#include <SoftwareSerial.h>

#define RX_PIN 7
#define TX_PIN 8
#define HUE_BRIDGE_IP "192.168.1.2"  // Replace with your Hue Bridge IP

#define YOUR_USERNAME "your_username"  // Replace with your Hue Bridge username

// change to match your Hue Bridge group numbers
#define LIGHT_ID_1 1
#define LIGHT_ID_2 2
#define GROUP_ID 1

SoftwareSerial command_serial(RX_PIN, TX_PIN);  // RX, TX

// Function to create and format the command string
String formatCommand(String apiEndpoint, String method, String additionalParams = "") {
    return "!curl -s -X " + method + " -H \"Content-Type: application/json\" -d '" + additionalParams +
           "' http://" + HUE_BRIDGE_IP + "/api/" + YOUR_USERNAME + "/" + apiEndpoint;
}

// Function to control the state of a light
void controlLightState(int lightId, String command) {
    String apiEndpoint = "lights/" + String(lightId) + "/state";
    command_serial.println(formatCommand(apiEndpoint, "PUT", command));
}

// Function to control the state of a group
void controlGroupState(int groupId, String command) {
    String apiEndpoint = "groups/" + String(groupId) + "/action";
    command_serial.println(formatCommand(apiEndpoint, "PUT", command));
}

// Function to get information about the Hue Bridge
void getBridgeInfo() {
    command_serial.println(formatCommand("", "GET"));
}

void setup() {
    command_serial.begin(9600);

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
