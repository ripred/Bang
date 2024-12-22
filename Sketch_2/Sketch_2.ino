/*
  Sketch_2.ino

  This sketch can upload "Sketch_1" back onto the board using the same
  Bang-based mechanism. That way, you can bounce back and forth.

  Once "Sketch_2" is running, it can command the host to compile and upload
  "Sketch_1".
*/

#include <Arduino.h>
#include "Bang.h"

// Forward declarations
void bang_host_out_callback(Bang* inst, uint8_t cmd, const char* data, uint16_t len);
void handle_upload_sketch_1(void);

struct bang_state_t {
  Bang bang;
};

bang_state_t g_bang;

void setup() {
  Serial.begin(38400);
  while (!Serial) {
    ; // On a Nano, typically not needed, but let's keep it for completeness.
  }

  bang_init(&g_bang.bang, &Serial, bang_host_out_callback, NULL);
  bang_host_printf("#Sketch_2 is now running.\n");
  bang_host_printf("#Send 'R' over serial to return (upload Sketch_1), or wait 5 seconds...\n");
}

void loop() {
  bang_update(&g_bang.bang);

  static unsigned long start_ms = millis();
  if (millis() - start_ms > 5000) {
    // Automatically upload Sketch_1 after 5 seconds
    handle_upload_sketch_1();
    start_ms = millis() + 999999UL;
  }

  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'R') {
      handle_upload_sketch_1();
    }
  }
}

void handle_upload_sketch_1(void) {
  // Send the special "&Sketch_1" command to the Python agent via Bang
  bang_host_printf("&Sketch_1\n");
  bang_host_printf("#Attempting to upload Sketch_1 (replacing Sketch_2)...\n");
}

void bang_host_out_callback(Bang* inst, uint8_t cmd, const char* data, uint16_t len) {
  (void)inst;
  (void)cmd;
  (void)data;
  (void)len;
  // No special handling needed here, but you could parse host responses if desired.
}

