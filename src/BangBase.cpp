#include "Bang.h"

// A simple buffer size for read data or for formatting text
#ifndef BANG_BUFFER_SIZE
#define BANG_BUFFER_SIZE 64
#endif

// This static global pointer will allow bang_host_printf(...) to know which Bang instance to use.
// Alternatively, you could store a pointer to a "current" Bang in user_data. 
static Bang* s_currentBang = nullptr;

void bang_init(Bang* b, Stream* s, BangCallback cb, void* user_data) {
    if (!b) return;
    b->stream   = s;
    b->callback = cb;
    b->user_data = user_data;
    s_currentBang = b; 
}

// A trivial parser example. You might expand this to look for command IDs, etc.
void bang_update(Bang* b) {
    if (!b || !b->stream) return;

    // Read available characters and, for demo, call the callback if you want
    while (b->stream->available()) {
        char c = (char)(b->stream->read());
        // For demonstration, if we get '!' as a command ID, call the callback:
        if (c == '!') {
            // Example: read the rest of the line
            char buffer[BANG_BUFFER_SIZE];
            size_t idx = 0;
            while (b->stream->available() && idx < (BANG_BUFFER_SIZE - 1)) {
                char nextC = (char)(b->stream->peek());
                if (nextC == '\n' || nextC == '\r') break;
                buffer[idx++] = b->stream->read();
            }
            buffer[idx] = '\0';
            // Call the callback with a made-up command byte and the data
            if (b->callback) {
                b->callback(b, /*cmd=*/123, buffer, (uint16_t)idx);
            }
        }
    }
}

// A printf-style function that writes out to the host (b->stream).
// If you only ever have one Bang instance, you can rely on s_currentBang.
// Otherwise, you'd add an argument so you know which instance to write to.
void bang_host_printf(const char* fmt, ...) {
    if (!s_currentBang || !s_currentBang->stream) return;

    char buffer[BANG_BUFFER_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    s_currentBang->stream->print(buffer);
}

