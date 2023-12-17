/*
 * openWeatherMap.ino
 *
 * arduinoCLI sketch to retrieve the current weather for
 * your zip code and return the information to your
 * Arduino!
 *
 */

#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include "openweathermap.h"

#define  RX_PIN     7
#define  TX_PIN     8

// Software Serial object to send the
// commands to the Python Agent
SoftwareSerial command_serial(RX_PIN, TX_PIN);  // RX, TX

void get_weather(char const *zipcode, char const *api_key, char *buff) {
    char const *fmt = "curl -s -m 60 'http://api.openweathermap.org/data/2.5/weather?zip=%s&units=imperial&appid=%s'";
    sprintf(buff, fmt, zipcode, api_key);
}

void setup() {
    Serial.begin(115200);
    command_serial.begin(9600);

    {
        char command[160] = "";
        get_weather(ZIPCODE, API_KEY, command);
        command_serial.println(command);
    }

    char json[512];
    memset(json, 0, sizeof(json));
    int index = 0;
    char c = 0;
    unsigned long timeout = 1000;
    unsigned long startTime = millis();
    while ((millis() - startTime < timeout) && (-1 != c)) {
        while (command_serial.available()) {
            char const c = command_serial.read();
            if (-1 == c) {
                break;
            }
            json[index++] = c;
        }
    }

    // Parse JSON string
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, json);

    // Check for parsing errors
    if (error) {
        Serial.print(F("Failed to parse JSON: "));
        Serial.println(error.c_str());
        return;
    }

    char const *coordstr = "coord";
    char const *mainstr = "main";
    char const *windstr = "wind";
    double const lat = doc[coordstr]["lat"];
    double const lon = doc[coordstr]["lon"];
    char const *city = doc["name"];
    char const *condition = doc["weather"][0]["description"];
    int const temp = doc[mainstr]["temp"];
    int const feels_like = doc[mainstr]["feels_like"];
    int const humidity = doc[mainstr]["humidity"];
    double const wind = doc[windstr]["speed"];
    int wind_dir = doc[windstr]["deg"];

    char const *dir_str[8] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW" };

    // quantize the wind direction into 1 of 8 directions (0-7)
    wind_dir /= 45;

    Serial.print(F("Weather for "));
    Serial.println(city);

    Serial.print(F("latitude: "));
    Serial.println(lat);

    Serial.print(F("longitude: "));
    Serial.println(lon);

    Serial.print(F("Condition: "));
    Serial.println(condition);

    Serial.print(F("Temp: "));
    Serial.println(temp);

    Serial.print(F("Feels like: "));
    Serial.println(feels_like);

    Serial.print(F("Humidity: "));
    Serial.println(humidity);

    Serial.print(F("Wind: "));
    Serial.println(wind);

    Serial.print(F("Wind Direction: "));
    Serial.println(dir_str[wind_dir]);
}

void loop() { }
