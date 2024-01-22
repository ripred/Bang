/*
 * datetime.ino
 *
 * Bang platform sketch to retrieve the current date and time
 * to your Arduino.
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
Bang bang(command_serial, Serial);  // class wrapper for the Bang api using two serial ports
#else
Bang bang(Serial);  // class wrapper for the Bang api using one serial port
#endif

#define    WINDOWS_CMD    "echo %DATE:~10,4%-%DATE:~4,2%-%DATE:~7,2% %TIME:~0,2%:%TIME:~3,2%:%TIME:~6,2%"
#define    MAC_CMD        "date '+%Y-%m-%d %H:%M:%S'"

void setup() {
    Serial.begin(115200);
    command_serial.begin(38400);

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
