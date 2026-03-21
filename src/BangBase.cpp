#include "Bang.h"

// A simple buffer size for read data or for formatting text
#ifndef BANG_BUFFER_SIZE
#define BANG_BUFFER_SIZE 256
#endif

// This static global pointer will allow bang_host_printf(...) to know which Bang instance to use.
// Alternatively, you could store a pointer to a "current" Bang in user_data. 
static Bang* s_currentBang = nullptr;

void bang_init(Bang* b, Stream* s, BangCallback cb, void* user_data) {
    if (!b) return;
    b->stream   = s;
    b->callback = cb;
    b->user_data = user_data;
#ifdef __cplusplus
    b->serial_stream = nullptr;
    b->timeout_ms = 1000;
#endif
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

#ifdef __cplusplus
namespace {

static bool bang_has_prefix(const char* command) {
    if (!command || !command[0]) {
        return false;
    }

    switch (command[0]) {
        case '!':
        case '@':
        case '#':
        case '&':
            return true;

        default:
            return false;
    }
}

static void bang_print_line(Stream* target, const char* text) {
    if (!target || !text) {
        return;
    }

    target->print(text);
    size_t const len = strlen(text);
    if (len == 0 || (text[len - 1] != '\n' && text[len - 1] != '\r')) {
        target->print('\n');
    }
}

}  // namespace

Bang::Bang() : stream(nullptr), callback(nullptr), user_data(nullptr),
               serial_stream(nullptr), timeout_ms(1000) {}

Bang::Bang(Stream& s) : Bang() {
    bang_init(this, &s, nullptr, nullptr);
}

Bang::Bang(Stream& s, Stream& serial) : Bang() {
    bang_init(this, &s, nullptr, nullptr);
    serial_stream = &serial;
}

void Bang::setTimeout(unsigned long timeout) {
    timeout_ms = timeout;
}

String Bang::exec(const String& command) {
    return exec(command.c_str());
}

String Bang::exec(const char* command) {
    if (!stream || !command) {
        return String();
    }

    while (stream->available() > 0) {
        stream->read();
    }

    if (!bang_has_prefix(command)) {
        stream->print('!');
    }
    stream->println(command);

    String response;
    unsigned long const start_ms = millis();
    unsigned long last_rx_ms = start_ms;

    while (millis() - start_ms < timeout_ms) {
        while (stream->available() > 0) {
            response += static_cast<char>(stream->read());
            last_rx_ms = millis();
        }

        if (response.length() > 0 && millis() - last_rx_ms > 25) {
            break;
        }
    }

    return response;
}

void Bang::serial(const String& text) {
    serial(text.c_str());
}

void Bang::serial(const char* text) {
    Stream* target = serial_stream ? serial_stream : stream;
    bang_print_line(target, text);
}

void Bang::sync() {
    if (!stream) {
        return;
    }

    if (!serial_stream || serial_stream == stream) {
        bang_update(this);
        return;
    }

    while (serial_stream->available() > 0) {
        stream->write(serial_stream->read());
    }

    while (stream->available() > 0) {
        serial_stream->write(stream->read());
    }
}
#endif
