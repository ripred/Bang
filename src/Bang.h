#ifndef BANG_H
#define BANG_H

#include <Arduino.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward-declare the Bang struct (defined below)
typedef struct Bang Bang;

// Callback signature used when we receive certain commands from the host
typedef void (*BangCallback)(Bang* inst, uint8_t cmd, const char* data, uint16_t len);

// The main Bang struct, storing necessary state
struct Bang {
    Stream* stream;          // The hardware (or software) serial
    BangCallback callback;   // Optional callback for received commands
    void* user_data;         // Optional user context
};

// Initializes a Bang instance with the given parameters
void bang_init(Bang* b, Stream* s, BangCallback cb, void* user_data);

// Periodically called in loop() to read incoming data, parse commands, etc.
void bang_update(Bang* b);

// Utility function to send text back to the host (printf-style).
// E.g., bang_host_printf("Hello: %d\n", someValue);
void bang_host_printf(const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // BANG_H

