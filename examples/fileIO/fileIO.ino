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
#include "Bang.h"

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
String create_file(char const *filename, char const *text);
String delete_file(char const *filename);
String get_line(char const *filename, int const line_num);
String insert_line(char const *filename, int const line_num, char const *text);
String append_line(char const *filename, char const *text);
int get_num_lines(char const *filename);

void format_uint32_with_commas(uint32_t value, bool const newline=true) {
    char buffer[32]; // Adjust the size based on your maximum expected number
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%lu", value);
    
    for (int i = strlen(buffer) - 3; i > 0; i -= 3) {
        memmove(buffer + i + 1, buffer + i, strlen(buffer) - i);
        buffer[i] = ',';
    }
    
    Serial.print(buffer);
    if (newline) {
        Serial.write('\n');
    }
}

void format_double_with_commas(double value, int precision = 2, bool const newline=true) {
    uint32_t intValue = static_cast<uint32_t>(value);
    format_uint32_with_commas(intValue, false);
    
    char buffer[32]; // Adjust the size based on your maximum expected number
    memset(buffer, 0, sizeof(buffer));
    dtostrf(value - intValue, 0, precision, buffer);
    Serial.print(buffer + 1);
    if (newline) {
        Serial.write('\n');
    }
}

void setup() {
    Serial.begin(115200);
    command_serial.begin(38400);
    command_serial.setTimeout(1);

    Serial.println(F("\nrunning host file I/O tests...\n"));

    char const *filename = "testing.txt";
    char text[128] = "line 1 ******************************************************************************************";

    // Create the test file on the host machine
    create_file(filename, text);

    int const max_lines = 50;

    char const fmt[] = "line %d ******************************************************************************************";

    // fill in a total of 50 lines containing 100 characters each
    double start = micros();
    for (int line=2; line <= max_lines; line++) {
        snprintf(text, sizeof(text), fmt, line);
        append_line(filename, text);
    }
    double stop = micros();

    // Calculate and display some statistics about the transfer
    uint32_t const total_bits = (strlen(fmt) + 2) * max_lines * 10UL;
    double const time_spent = stop - start;

    Serial.print(F("total bits transferred: "));
    format_uint32_with_commas(total_bits, false);
    Serial.print(F(" = "));
    format_uint32_with_commas((strlen(fmt) + 2) * max_lines, false);
    Serial.println(F(" bytes"));

    Serial.print(F("total time spent: "));
    format_double_with_commas(time_spent / 1000000.0, 2, false);
    Serial.println(F(" seconds"));

    double const bps = (double(total_bits) / time_spent) * 1000000.0;
    format_double_with_commas(bps, 2, false);
    Serial.println(F(" bps transfer rate"));

    // Read the number of lines back from the file and verify it
    int const num_lines = get_num_lines(filename);

    Serial.print(F("get_num_lines(...) returned: "));
    Serial.println(num_lines, DEC);

    Serial.print(F("this was "));
    if (num_lines != max_lines) {
        Serial.print(F("not "));
    }
    Serial.println(F("the value that was expected"));

    // Read the lines back from the file and verify them
    Serial.println(F("Testing line contents..."));
    for (int line=0; line < num_lines; line++) {
        String text = get_line(filename, line + 1);
        int line_num = atoi(text.c_str() + 5);

        if ((line + 1) != line_num) {
            text.trim();
            Serial.print(text);
            Serial.println(F(": failed"));
        }
    }

    delete_file(filename);
    Serial.println(F("\ntests completed.\n"));
}

void loop() {
    bang.sync();
}

// ======================================================================
#ifdef USE_MACLINUX
String create_file(char const *filename, char const *text) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "echo \"%s\" > %s", text, filename);
    return bang.exec(cmd);
}

String delete_file(char const *filename) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "rm %s", filename);
    return bang.exec(cmd);
}

String get_line(char const *filename, int const line_num) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "sed -n '%dp' %s", line_num, filename);
    return bang.exec(cmd);
}

String insert_line(char const *filename, int const line_num, char const *text) {
  // sed -i '7i\new line content' filename.txt
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "sed -i '%di\\%s' %s", line_num, text, filename);
    return bang.exec(cmd);
}

String append_line(char const *filename, char const *text) {
  // echo "new line content" >> filename.txt
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "echo \"%s\" >> %s", text, filename);
    return bang.exec(cmd);
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
String create_file(char const *filename, char const *text) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "echo %s > %s", text, filename);
    return bang.exec(cmd);
}

String delete_file(char const *filename) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "del %s", filename);
    return bang.exec(cmd);
}

String get_line(char const *filename, int const line_num) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "more +%d %s", line_num, filename);
    return bang.exec(cmd);
}

String insert_line(char const *filename, int const line_num, char const *text) {
  // (for /f "delims=" %a in ('more +7 filename.txt') do echo new line content&echo %a) > filename.txt
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "(for /f \"delims=\" \%a in ('more +%d %s') do echo %s&echo \%a) > %s", line_num, filename, text, filename);
    return bang.exec(cmd);
}

String append_line(char const *filename, char const *text) {
  // echo new line content >> filename.txt
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "echo %s >> %s", text, filename);
    return bang.exec(cmd);
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
