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

// Software Serial object to send the
// commands to the Python Agent
SoftwareSerial command_serial(RX_PIN, TX_PIN);  // RX, TX

void create_applescript_file(char const *song) {
    char buff[512] = "";

    command_serial.println(F("!echo 'tell application \"Music\"' > play_and_stop.scpt"));

    sprintf(buff, "!echo '    play track \"%s\"' >> play_and_stop.scpt", song);
    command_serial.println(buff);
    command_serial.println(F("!echo \"    repeat until player state is stopped\" >> play_and_stop.scpt"));
    command_serial.println(F("!echo \"        delay 1\" >> play_and_stop.scpt"));
    command_serial.println(F("!echo \"    end repeat\" >> play_and_stop.scpt"));
    command_serial.println(F("!echo \"    stop\" >> play_and_stop.scpt"));
    command_serial.println(F("!echo \"end tell\" >> play_and_stop.scpt"));
}

void play_song(char const *song) {
    // create an applescript file to play the song
    create_applescript_file(song);
    
    // Play the song!
    command_serial.println("!osascript play_and_stop.scpt");

    // quit the Music.app if the user stopped it themselves
    command_serial.println("!osascript -e 'tell application \"Music\" to quit'");

    // remove the applescript file
    command_serial.println("!rm play_and_stop.scpt");
}


void setup() {
    Serial.begin(115200);
    command_serial.begin(9600);

    // name the song to play (Moby's "Porcelain" in this case)
    char const *song = "Porcelain";
    play_song(song);
}

void loop() { }
