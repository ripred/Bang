/*
 * datetime.ino
 *
 * arduinoCLI sketch to retrieve the current date and time
 * to your Arduino.
 */

#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#define  RX_PIN     7
#define  TX_PIN     8

// Software Serial object to send the
// commands to the Python Agent
SoftwareSerial command_serial(RX_PIN, TX_PIN);  // RX, TX

// Define ONLY ONE of the following
//#define WINDOWS
#define MAC

void setup() {
    Serial.begin(115200);
    command_serial.begin(9600);

#ifdef WINDOWS
    command_serial.println(F("echo %DATE:~10,4%-%DATE:~4,2%-%DATE:~7,2% %TIME:~0,2%:%TIME:~3,2%:%TIME:~6,2%"));
#endif

#ifdef MAC
    command_serial.println(F("date '+%Y-%m-%d %H:%M:%S'"));
#endif

    String response = command_serial.readString();
    response.trim();

    char const *datetime = response.c_str();
    int const year = atoi(datetime);
    int const month = atoi(datetime + 5);
    int const day = atoi(datetime + 8);
    int const hour = atoi(datetime + 11);
    int const min = atoi(datetime + 14);
    int const sec = atoi(datetime + 17);
    
    char buff[32] = "";
    sprintf(buff, "%4d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, min, sec);
    Serial.println(buff);
}

void loop() { }
