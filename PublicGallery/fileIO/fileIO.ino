/*
 * fileIO.ino
 * 
 * Example sketch using ArduinoCLI as 
 * a line editor for text file I/O
 * 
 */

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Stream.h>
#include "bang.h"

#define  RX_PIN     7
#define  TX_PIN     8

// Software Serial object to send the
// commands to the Python Agent
SoftwareSerial command_serial(RX_PIN, TX_PIN);  // RX, TX

// class wrapper for the ArduinoCLI api so far:
Bang bang(command_serial, Serial);

// un-comment ONE of the following lines to choose which 
// operating system you are using:

// #define USE_WINDOWS
#define USE_MACLINUX

// function prototypes so we can get right into setup() and loop()
void create_file(char const *filename, char const *text);
void delete_file(char const *filename);
void get_line(char const *filename, int const line_num);
void insert_line(char const *filename, int const line_num, char const *text);
void append_line(char const *filename, char const *text);
int get_num_lines(char const *filename);

void setup() {
    Serial.begin(115200);
    command_serial.begin(9600);
    command_serial.setTimeout(200);

    char const *filename = "testing.txt";
    char text[32] = "line 1";

    int const max_lines = 10;

    create_file(filename, text);
    for (int line=2; line <= max_lines; line++) {
        snprintf(text, sizeof(text), "line %d", line);
        append_line(filename, text);
        delay(10);
    }

    delay(30);

    int const num_lines = get_num_lines(filename);

    delay(30);

    Serial.print("get_num_lines(...) returned: ");
    Serial.println(num_lines, DEC);

    for (int line=0; line < num_lines; line++) {
        get_line(filename, line + 1);
        delay(100);
        while (command_serial.available()) {
            String text = command_serial.readString();
            text.trim();
            Serial.print(text);
            int line_num = atoi(text.c_str() + 5);
            if ((line + 1) == line_num) {
                Serial.println(": passed");
            }
            else {
                Serial.println(": failed");
            }
        }
    }

    delete_file(filename);
}

void loop() {
    bang.sync();
}

// ======================================================================
#ifdef USE_MACLINUX
void create_file(char const *filename, char const *text) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "echo \"%s\" > %s", text, filename);
    bang.exec(cmd);
}

void delete_file(char const *filename) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "rm %s", filename);
    bang.exec(cmd);
}

void get_line(char const *filename, int const line_num) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "sed -n '%dp' %s", line_num, filename);
    bang.exec(cmd);
}

void insert_line(char const *filename, int const line_num, char const *text) {
  // sed -i '7i\new line content' filename.txt
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "sed -i '%di\\%s' %s", line_num, text, filename);
    bang.exec(cmd);
}

void append_line(char const *filename, char const *text) {
  // echo "new line content" >> filename.txt
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "echo \"%s\" >> %s", text, filename);
    bang.exec(cmd);
}

int get_num_lines(char const *filename) {
  // wc -l filename.txt
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "wc -l %s", filename);
    String response = bang.exec(cmd);
    response.trim();
    return atoi(response.c_str());
}
#endif

#ifdef USE_WINDOWS
void create_file(char const *filename, char const *text) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "echo %s > %s", text, filename);
    bang.exec(cmd);
}

void delete_file(char const *filename) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "del %s", filename);
    bang.exec(cmd);
}

void get_line(char const *filename, int const line_num) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "more +%d %s", line_num, filename);
    bang.exec(cmd);
}

void insert_line(char const *filename, int const line_num, char const *text) {
  // (for /f "delims=" %a in ('more +7 filename.txt') do echo new line content&echo %a) > filename.txt
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "(for /f \"delims=\" \%a in ('more +%d %s') do echo %s&echo \%a) > %s", line_num, filename, text, filename);
    bang.exec(cmd);
}

void append_line(char const *filename, char const *text) {
  // echo new line content >> filename.txt
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "echo %s >> %s", text, filename);
    bang.exec(cmd);
}

int get_num_lines(char const *filename) {
    // find /v /c "" filename.txt
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "find /v /c \"\" %s", filename);
    String response = bang.exec(cmd);
    response.trim();
    return atoi(response.c_str());
}
#endif
