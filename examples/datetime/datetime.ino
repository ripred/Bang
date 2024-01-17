/*
 * datetime.ino
 *
 * arduinoCLI sketch to retrieve the current date and time
 * to your Arduino.
 */

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Bang.h>

#define  RX_PIN   7
#define  TX_PIN   8

SoftwareSerial command_serial(RX_PIN, TX_PIN);  // RX, TX

// class wrapper for the Bang api so far.
// This declaration uses both the Serial port and the FTDI serial port:
Bang bang(command_serial, Serial);

// This declaration uses only the Serial port but then we cannot use the Serial Monitor:
//Bang bang(Serial);

#define    WINDOWS_CMD    "echo %DATE:~10,4%-%DATE:~4,2%-%DATE:~7,2% %TIME:~0,2%:%TIME:~3,2%:%TIME:~6,2%"
#define    MAC_CMD        "date '+%Y-%m-%d %H:%M:%S'"

void setup() {
    Serial.begin(115200);
    command_serial.begin(9600);

    // Un-comment one of the following depending on your operating system:
    //String response = bang.exec(WINDOWS_CMD);
    String response = bang.exec(MAC_CMD);
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
