/*
 * macFreeDiskSpace.ino
 * 
 * arduinoCLI sketch to retrieve the free disk space of the host machine
 * 
 * flashes the built in LED if it drops below the configured amount
 * 
 */

#include <SoftwareSerial.h>

#define  RX_PIN     7
#define  TX_PIN     8
#define  ALERT_PIN  LED_BUILTIN

// Use 10GB as the minimum few disk space.
// Change as needed.
double constexpr MinFreeSpace = 10UL * 1024UL * 1024UL * 1024UL;

// the last free space reported
double free_space = 100UL * 1024UL * 1024UL * 1024UL;

// flag containing the "low space" status
uint8_t  space_is_low = false;

// Software Serial object to send the
// commands to the Python Agent
SoftwareSerial command_serial(RX_PIN, TX_PIN);  // RX, TX

// function prototypes
void flash_led();
void flash_if_low();
void longPause(uint32_t minutes);

void setup() {
    Serial.begin(115200);
    command_serial.begin(9600);
    pinMode(ALERT_PIN, OUTPUT);
}

void loop() {
    char const command[] PROGMEM = "!df -P / | awk 'NR==2 {print $4}' | tr -d 'G'";
    command_serial.println(command);
    delay(1000);
    if (command_serial.available() > 5) {
        // get number of free 512-byte blocks
        String response = command_serial.readString();
        response.trim();
        
        char blk_str[16] = "";
        double const free_blocks = atol(response.c_str());
        dtostrf(free_blocks, 5,2, blk_str);

        char bytes_str[16] = "";
        free_space = free_blocks * 512.0;
        dtostrf(free_space, 5,2, bytes_str);

        char gb_str[16] = "";
        double free_gb = free_space / (1024.0 * 1024.0 * 1024.0);
        dtostrf(free_gb, 5,2, gb_str);

        char buff[128] = "";
        sprintf(buff, "Free Disk Space %s blocks, %s bytes (%s GB)",
            blk_str, bytes_str, gb_str);
        Serial.println(buff);

        space_is_low = free_space < MinFreeSpace;
    }

    uint32_t constexpr pause_minutes = 10;
    longPause(pause_minutes) ;
}

// ======================================================================
// support functions
// ======================================================================

void flash_led() {
    static uint32_t last_toggle = 0;
    static uint8_t led_state = 0;
    uint32_t flash_rate = 200;
    uint32_t now = millis();
    if (now - last_toggle >= flash_rate) {
        last_toggle = now;
        led_state = !led_state;
        digitalWrite(ALERT_PIN, led_state);
    }
}

void flash_if_low() {
    if (space_is_low) {
        flash_led();
    }
}

void longPause(uint32_t minutes) {
    uint32_t const pause_minutes_ms = minutes * 60000UL;
    uint32_t start = millis();
    while (millis() - start < pause_minutes_ms) {
        flash_if_low();
    }
}
