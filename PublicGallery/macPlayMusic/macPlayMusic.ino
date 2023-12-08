/*
 * macPlayMusic.ino
 * 
 * arduinoCLI sketch to play a specific song using the Music.app 
 * application on the mac
 * 
 */

#include <SoftwareSerial.h>

#define  RX_PIN     7
#define  TX_PIN     8
#define  ALERT_PIN  LED_BUILTIN

// Software Serial object to send the
// commands to the Python Agent
SoftwareSerial command_serial(RX_PIN, TX_PIN);  // RX, TX

void setup() {
    Serial.begin(115200);
    command_serial.begin(9600);

    char command[128] = "";

    // name the song to play (Moby's "Porcelain" in this case)
    char const *song = "Porcelain";
    char const *fmt = "osascript -e 'tell application \"Music\" to play track \"Porcelain\"'";
    snprintf(command, 128, fmt, song);

    // Play the song!
    command_serial.println(command);
}

void loop() { }
