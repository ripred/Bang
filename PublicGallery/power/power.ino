/*
 * power.ino
 * 
 * arduinoCLI sketch to make your Mac, Linux, or Windows host
 * go to sleep, shutdown, or reboot when the Arduino tells it to.
 * 
 * IMPORTANT NOTE: For Mac and Linux hosts in order to use these functions
 * the Python Agent must be started with the "sudu" command:
 * 
 * $ sudo python arduino_exec.py
 */

// ==========================================================================================

// Uncomment ONE of the following lines to use this sketch for a Windows, Mac or Linux host:
//#define   USE_WINDOWS
#define   USE_MAC
//#define   USE_LINUX

// ==========================================================================================

#include <SoftwareSerial.h>

#define  RX_PIN     7
#define  TX_PIN     8

// Software Serial object to send the
// commands to the Python Agent
SoftwareSerial command_serial(RX_PIN, TX_PIN);  // RX, TX

// ==========================================================================================
#ifdef USE_WINDOWS
/*
 * sleep, reboot and shutdown commands for Windows hosts
 */

void sleep() {
    String command = "shutdown /h";     // on some Windows systems this tells the host to hybernate
                                        // on others it puts the host to sleep
    command_serial.println(command);
}

void reboot(int const delay_seconds = 300) {
    char command[64] = "";
    sprintf(command, "shutdown /r /t %d", delay_seconds);

    command_serial.println(command);
}

void shutdown(int const delay_seconds = 300) {
    char command[64] = "";
    sprintf(command, "shutdown /s /t %d", delay_seconds);

    command_serial.println(command);
}
#endif
// ==========================================================================================
#ifdef USE_MAC

/*
 * sleep, reboot and shutdown commands for Mac hosts
 */

void sleep() {
    String command = "pmset sleepnow";

    command_serial.println(command);
}

void reboot(int const delay_minutes = 5) {
    char command[64] = "";
    sprintf(command, "shutdown -r +%d", delay_minutes);

    command_serial.println(command);
}

void shutdown(int const delay_minutes = 5) {
    char command[64] = "";
    sprintf(command, "shutdown -h +%d", delay_minutes);

    command_serial.println(command);
}
#endif


// ==========================================================================================
#ifdef USE_LINUX
/*
 * sleep, reboot and shutdown commands for Linux hosts
 */

void sleep() {
    String command = "systemctl suspend";

    command_serial.println(command);
}

void reboot(int const delay_minutes = 5) {
    char command[64] = "";
    sprintf(command, "shutdown -r +%d", delay_minutes);

    command_serial.println(command);
}

void shutdown(int const delay_minutes = 5) {
    char command[64] = "";
    sprintf(command, "shutdown -h +%d", delay_minutes);

    command_serial.println(command);
}
#endif

// ==========================================================================================

void setup() {
    command_serial.begin(9600);

    sleep();        // tell the host to go to sleep

    // or
    //shutdown(0);  // tell the host to shut down

    // or
    //reboot(0);    // tell the host to reboot
}

void loop() { }
