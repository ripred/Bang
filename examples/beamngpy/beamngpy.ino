/*
 * beamngpy.ino
 *
 * Bang platform example sketch for talking to a running BeamNG instance
 * through the official BeamNGpy Python API.
 *
 * This example uses Bang's underlying serial-command protocol directly:
 * the Arduino sends `!python ...` commands to the Python agent and receives
 * JSON responses back over the same command serial channel.
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#define RX_PIN 7
#define TX_PIN 8

// Choose the host operating system that runs both BeamNG and arduino_exec.py.
#define USE_WINDOWS
// #define USE_LINUX

// Update this to the absolute path of beamngpy_bridge.py on the host.
// Forward slashes also work on Windows.
static const char BEAMNGPY_BRIDGE[] =
    "C:/Users/yourname/Documents/Arduino/libraries/Bang/examples/beamngpy/beamngpy_bridge.py";

static const char BEAMNG_HOST[] = "127.0.0.1";
static const uint16_t BEAMNG_PORT = 25252;
static const uint32_t AGENT_TIMEOUT_MS = 5000;

SoftwareSerial command_serial(RX_PIN, TX_PIN);

void clearAgentInput();
String readAgentLine(uint32_t timeout_ms);
String runHostCommand(const String& command, uint32_t timeout_ms = AGENT_TIMEOUT_MS);
String buildBridgeCommand(const __FlashStringHelper* subcommand, const String& extra = "");
void printHelp();
void printJsonError(const String& response);
void requestTelemetry();
void setAiMode(const __FlashStringHelper* mode);
void showBeamngMessage();

void setup() {
    Serial.begin(115200);
    command_serial.begin(38400);

    Serial.println(F("\nBeamNGpy Bang example"));
    Serial.println(F("---------------------"));
    printHelp();
}

void loop() {
    if (Serial.available() <= 0) {
        return;
    }

    const char input = static_cast<char>(tolower(Serial.read()));
    switch (input) {
        case 't':
            requestTelemetry();
            break;

        case 'r':
            setAiMode(F("traffic"));
            break;

        case 'd':
            setAiMode(F("disabled"));
            break;

        case 'm':
            showBeamngMessage();
            break;

        case 'h':
            printHelp();
            break;

        default:
            Serial.print(F("Unknown command: "));
            Serial.println(input);
            printHelp();
            break;
    }
}

void clearAgentInput() {
    while (command_serial.available() > 0) {
        command_serial.read();
    }
}

String readAgentLine(uint32_t timeout_ms) {
    String line;
    const uint32_t start = millis();

    while (millis() - start < timeout_ms) {
        while (command_serial.available() > 0) {
            const char c = static_cast<char>(command_serial.read());
            if (c == '\r') {
                continue;
            }
            if (c == '\n') {
                if (line.length() != 0) {
                    return line;
                }
                continue;
            }
            line += c;
        }
    }

    return line;
}

String runHostCommand(const String& command, uint32_t timeout_ms) {
    clearAgentInput();
    command_serial.print('!');
    command_serial.println(command);
    return readAgentLine(timeout_ms);
}

String buildBridgeCommand(const __FlashStringHelper* subcommand, const String& extra) {
    String command;

#if defined(USE_WINDOWS)
    command += F("py -3 ");
#elif defined(USE_LINUX)
    command += F("python3 ");
#else
  #error "Define either USE_WINDOWS or USE_LINUX for the BeamNG host."
#endif

    command += '"';
    command += BEAMNGPY_BRIDGE;
    command += '"';
    command += ' ';
    command += subcommand;
    command += F(" --host ");
    command += BEAMNG_HOST;
    command += F(" --port ");
    command += String(BEAMNG_PORT);

    if (extra.length() != 0) {
        command += ' ';
        command += extra;
    }

    return command;
}

void printHelp() {
    Serial.println(F("Serial commands:"));
    Serial.println(F("  t : fetch player vehicle telemetry"));
    Serial.println(F("  r : set player vehicle AI mode to traffic"));
    Serial.println(F("  d : disable player vehicle AI"));
    Serial.println(F("  m : display a BeamNG UI message"));
    Serial.println(F("  h : print this help text"));
    Serial.println();
}

void printJsonError(const String& response) {
    if (response.length() == 0) {
        Serial.println(F("No response from the Bang host agent."));
        return;
    }

    Serial.println(F("Host response was not valid JSON:"));
    Serial.println(response);
}

void requestTelemetry() {
    const String response = runHostCommand(buildBridgeCommand(F("telemetry")));
    if (response.length() == 0) {
        Serial.println(F("Timed out waiting for telemetry."));
        return;
    }

    DynamicJsonDocument doc(768);
    const DeserializationError error = deserializeJson(doc, response);
    if (error) {
        printJsonError(response);
        return;
    }

    if (!doc["ok"].as<bool>()) {
        Serial.print(F("BeamNGpy error: "));
        Serial.println(doc["error"].as<const char*>());
        return;
    }

    const char* vehicle = doc["vehicle"] | "unknown";
    const char* gear = doc["gear"] | "";
    const JsonArray pos = doc["pos"].as<JsonArray>();

    Serial.print(F("Vehicle: "));
    Serial.println(vehicle);

    Serial.print(F("Speed: "));
    Serial.print(doc["speed_mph"].as<float>(), 1);
    Serial.print(F(" mph / "));
    Serial.print(doc["speed_kph"].as<float>(), 1);
    Serial.println(F(" kph"));

    Serial.print(F("RPM: "));
    Serial.println(doc["rpm"].as<float>(), 1);

    Serial.print(F("Gear: "));
    Serial.println(gear);

    Serial.print(F("Throttle: "));
    Serial.println(doc["throttle"].as<float>(), 3);

    Serial.print(F("Brake: "));
    Serial.println(doc["brake"].as<float>(), 3);

    Serial.print(F("Steering: "));
    Serial.println(doc["steering"].as<float>(), 3);

    Serial.print(F("Running: "));
    Serial.println(doc["running"].as<bool>() ? F("yes") : F("no"));

    Serial.print(F("Position: "));
    if (!pos.isNull() && pos.size() >= 3) {
        Serial.print(pos[0].as<float>(), 3);
        Serial.print(F(", "));
        Serial.print(pos[1].as<float>(), 3);
        Serial.print(F(", "));
        Serial.println(pos[2].as<float>(), 3);
    } else {
        Serial.println(F("n/a"));
    }

    Serial.println();
}

void setAiMode(const __FlashStringHelper* mode) {
    String extra = F("--mode ");
    extra += mode;

    const String response = runHostCommand(buildBridgeCommand(F("ai"), extra));
    DynamicJsonDocument doc(256);
    const DeserializationError error = deserializeJson(doc, response);
    if (error) {
        printJsonError(response);
        return;
    }

    if (!doc["ok"].as<bool>()) {
        Serial.print(F("BeamNGpy error: "));
        Serial.println(doc["error"].as<const char*>());
        return;
    }

    Serial.print(F("AI mode set to: "));
    Serial.println(doc["ai_mode"].as<const char*>());
}

void showBeamngMessage() {
    const String response = runHostCommand(
        buildBridgeCommand(F("message"), F("--text \"Hello from Bang on Arduino\""))
    );

    DynamicJsonDocument doc(256);
    const DeserializationError error = deserializeJson(doc, response);
    if (error) {
        printJsonError(response);
        return;
    }

    if (!doc["ok"].as<bool>()) {
        Serial.print(F("BeamNGpy error: "));
        Serial.println(doc["error"].as<const char*>());
        return;
    }

    Serial.println(F("BeamNG message displayed."));
}
