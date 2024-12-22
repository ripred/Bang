/*
  Sketch_1.ino

  Demonstrates using Bang to request that the host compile and upload
  a different sketch ("Sketch_2"), replacing the current firmware.

  Once "Sketch_1" is running, you can send it a command (e.g. over Serial)
  or just let it auto-trigger the upload to "Sketch_2" after a short delay.

  Make sure the Bang library is properly installed or in the include path.
*/

#include <Arduino.h>
#include "Bang.h"

// Forward declarations
void bang_host_out_callback(Bang* inst, uint8_t cmd, const char* data, uint16_t len);
void handle_upload_sketch_2(void);

// A simple struct to hold our Bang instance
struct bang_state_t {
  Bang bang;
};

bang_state_t g_bang;

void setup() {
  Serial.begin(38400);
  // Wait for USB serial if needed (on some boards); optional on a Nano.
  while (!Serial) {
    ; // do nothing
  }

  // Initialize Bang: 
  //  - reference to the underlying hardware Serial
  //  - optional callback for messages from host (we only use host_printf in this example)
  bang_init(&g_bang.bang, &Serial, bang_host_out_callback, NULL);

  // Optionally greet via the host
  bang_host_printf("#Sketch_1 is now running.\n");

  // Let the user know how to trigger an upload of Sketch_2
  bang_host_printf("#Send 'U' over serial to upload Sketch_2 (or wait 5 seconds)...\n");
}

void loop() {
  // Continuously process incoming Bang commands or data from the host
  bang_update(&g_bang.bang);

  // If you want to do an automatic upload to "Sketch_2" after a short delay, 
  // you could do it here once, say after 5 seconds:
  static unsigned long start_ms = millis();
  if (millis() - start_ms > 5000) {
    // Let's trigger the upload of Sketch_2 automatically
    handle_upload_sketch_2();
    // reset the timer so we only do this once
    start_ms = millis() + 999999UL;
  }

  // Alternatively, if you want a user input approach:
  // Press 'U' in the Serial Monitor to upload Sketch_2
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'U') {
      handle_upload_sketch_2();
    }
  }
}

void handle_upload_sketch_2(void) {
  // Use Bang's "host_printf" to send a special command to the Python agent.
  // The '&' character indicates "compile and upload" in your Python code.
  // "Sketch_2" is the folder name that must match the .ino name as well.
  bang_host_printf("&Sketch_2\n");
  bang_host_printf("#Attempting to upload Sketch_2 (replacing Sketch_1)...\n");
}

// Optional callback if you want to see messages that come back from the host
void bang_host_out_callback(Bang* inst, uint8_t cmd, const char* data, uint16_t len) {
  // In a more advanced scenario, you might parse or respond to messages here.
  // For now, we just let the default printing happen via bang_host_printf.
  (void)inst;
  (void)cmd;
  (void)data;
  (void)len;
}

