[![Arduino CI](https://github.com/ripred/Bang/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![Arduino-lint](https://github.com/ripred/Bang/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/ripred/Bang/actions/workflows/arduino-lint.yml)
![code size:](https://img.shields.io/github/languages/code-size/ripred/Bang)
[![GitHub release](https://img.shields.io/github/release/ripred/Bang.svg?maxAge=3600)](https://github.com/ripred/Bang/releases)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/ripred/Bang/blob/master/LICENSE)

# The Bang Arduino Library
## Arduino based command line interface.
## (formerly known as ArduinoCLI)
Make your Windows, Mac, or Linux host act as a "service" for your Arduino (and any other serial-USB capable microcontroller such as the ESP32) and execute any commands on it's behalf and receive the captured results! Anything you can do at a terminal prompt can be done on behalf of the Arduino with any output captured and sent back to the Arduino to do whatever it wants with it!

Play, pause, and stop music files on the host, use the PC's large disk drive for Arduino accessible storage, get the current date & time, issue curl commands to post or retrieve anything on the web or to control your local intranet Hue Bridge and Lights, retrieve the current weather, tell the host machine to reboot, check if the host machine is asleep. The possibilities are endless! All using the simplest of Arduino's with no additional modules or connections needed besides the Serial-USB communications. 😃

So far I have written and added the following sketches to the **[examples](https://github.com/ripred/Bang/tree/main/examples)** folder:

-   **[bang.ino](https://github.com/ripred/Bang/blob/main/examples/bang/bang.ino)** example sketch that lets you send any command from the serial monitor window and receive the response.
-   **[datetime.ino](https://github.com/ripred/Bang/blob/main/examples/datetime/datetime.ino)** sketch to retrieve the current date and time from Windows, Mac, and Linux hosts!
-   **[fileIO.ino](https://github.com/ripred/Bang/blob/main/examples/fileIO/fileIO.ino)** example sketch for creating, reading, writing, and deleting text files including support for random access insert and read line by number, get number of lines in a file, etc. Perfect for logging.
-   **[power.ino](https://github.com/ripred/Bang/blob/main/examples/power/power.ino)** sketch for Windows, Mac, and Linux to tell the host machine to go to sleep, reboot, or shutdown
-   **[hue.ino](https://github.com/ripred/Bang/blob/main/examples/hue/hue.ino)** example sketch for controlling the lights in your home via the Hue Bridge using 'curl' commands.
-   **[openWeatherMap.ino](https://github.com/ripred/Bang/blob/main/examples/openWeatherMap/openWeatherMap.ino)** sketch to retrieve the city name, latitude, longitude, current conditions, temperature, 'feels like' temperature, humidity, wind speed, and wind direction for any zip code.
-   **[macFreeDiskSpace.ino](https://github.com/ripred/Bang/blob/main/examples/macFreeDiskSpace/macFreeDiskSpace.ino)** sketch to monitor and blink an LED if your Mac/Linux disk drive falls below a certain amount of free space
-   **[macPlayMusic.ino](https://github.com/ripred/Bang/blob/main/examples/macPlayMusic/macPlayMusic.ino)** sketch to play any song in your music library when your Arduino sketch tells it to play it
-   **[macSpeechSynthesizer.ino](https://github.com/ripred/Bang/blob/main/examples/macSpeechSynthesizer/macSpeechSynthesizer.ino)** sketch to make your Mac speak anything your Arduino tells it to
-   **[isMacAsleep.ino](https://github.com/ripred/Bang/blob/main/examples/isMacAsleep/isMacAsleep.ino)** sketch to retrieve whether the host machine is asleep or not!
-   **[pjlink.ino](https://github.com/ripred/Bang/blob/main/examples/pjlink/pjlink.ino)** sketch for an example of invoking the `pjlink` command line tool for controlling and retrieving information from projectors and other PJLINK capable devices

<!-- &#160; -->
___
## • Starting the Python Agent
Note that the Python module pyserial must be installed to allow the Python Agent to open the virtual serial port to talk to the Arduino. If you do not have it installed you can install it using the command:
```
pip install pyserial
```

In order to allow your Arduino to execute all programs that are requested, as well as to capture all of the output, you should start the Python Agent using the following command line. Replace 'COM3' with the COM port your Arduino is connected to

#### • For Windows Users:
```
runas /user:Administrator "cmd /c python3 arduino_exec.py -p COM3"
```

Note that if you don't plan on executing any commands that require the extra administrative permissons you can run the program directly just using Python alone without including the `runas /user:Administrator` prefix. Some of the sketches in the public gallery require administrative permissions, specifically those that allow the Arduino to shut down, reboot, or put the host machine to sleep:

```
python3 arduino_exec.py -p COM3
```

#### • For Mac and Linux Users:
Replace the device path '/dev/cu.usbserial-00100' with the path to your Arduino port.

```
sudo python3 arduino_exec.py -p /dev/cu.usbserial-00100
```

Note that if you don't plan on executing any commands that require the extra root permissons you can run the program directly just using Python alone without including the `sudo` prefix. Some of the sketches in the public gallery require root permissions, specifically those that allow the Arduino to shut down, reboot, or put the host machine to sleep. Replace the device path '/dev/cu.usbserial-00100' with the path to your Arduino port.

```
python3 arduino_exec.py -p /dev/cu.usbserial-00100
```

<!-- &#160; -->
___
## • Using Bang in your Arduino sketches

To use Bang in your sketches, simply use `Serial.println( "!command" )` to send the command to the USB (COM) port used by your Arduino.

For example, to tell the host to echo a string to the display you could issue the call:

`Serial.println("!echo 'hello, arduino!'");`

**Update:** With the changes to a library and the name there is now a `Bang` data type that let's you easily issue commands, macros, serial output, or dynamically compile and load new code (WIP).

To use the class include the Bang.h header file in your project and declare the one (or two) Stream objects that it will be working with for issuing commands (and for Serial output) respectively:
```
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Bang.h>

// (Optionally) declare a separate Stream object to talk to so we can still use the Serial monitor window:
#define  RX_PIN   7
#define  TX_PIN   8
SoftwareSerial command_serial(RX_PIN, TX_PIN);  // RX, TX

// class wrapper for the Bang api when just using the Serial port (without an additional FTDI module):
// Bang bang(Serial);

// class wrapper for the Bang api when using an additional FTDI module:
Bang bang(command_serial, Serial);

void setup() {
    Serial.begin(115200);
    command_serial.begin(38400);

    bang.serial("executable lines should start with a bang ! character as in:");
    bang.serial("    !echo hello, arduino!");

    bang.serial("macro lines should start with an @ character as in:");
    bang.serial("    @list_macros");

    bang.serial("'compile and reload' using the & character as in:");
    bang.serial("    &blink\n");
}

void loop() {
    // allow the user to talk directly to the host via the Serial monitor window:
    bang.sync();
}
```

**If you want to be able to use the Serial monitor separately from using Bang** then you will need to connect an FTDI USB-ttl adapter to your Arduino and specify its COM port on the command line when you run arduino_exec.py Python Agent. Most of the example sketches show the use of an FTDI USB-ttl adapter in their source. You do not *have* to use an FTDI adapter unless you want to continue to use the Serial monitor while the sketch is running.

<!-- &#160; -->
___
## • Future Possible Uses of Bang

The following are some of the ideas I have had that this technique can be used for. Items with a checkmark ✅ have already been implemeted in one way or another in one of the example sketches. 

* ✅ Invoke 'curl' commands to send internet requests and optionally retrieve the response back to your Arduino. Many of the following ideas are just expanded ideas of this basic ability.
* ✅ Play and stop music or movies on your host machine
* ✅ Retrieve weather info and control devices based on the results
* Retrieve sports updates using public API's such as NHL's and MLB's api's
* ✅ Write and retrieve data to files on your host machine and take advantage of it's much larger capacity versus an SD card! Basically all file functionality that you can do from the command line like creating files, appending to them, reading them back, and deleting them, etc.
* Post or retrieve posts with reddit without using a complex reddit api (and their limitations! 😉)
* Access and use a running database server on your host machine
* Submit sensor data to running machine learning training
* Submit Queries to a running machine learning model
* ✅ Retrieve the current time from the host machine
* Post updates or retrieve information from social media platforms.
* Monitor social media channels for specific keywords or trends.
* Retrieve and manipulate calendar events.
* Set reminders or schedule tasks on the host machine.
* ✅ Monitor system security logs for security events.
* Perform advanced text processing tasks, such as parsing log files or analyzing textual data.
* Perform Git operations like cloning repositories, pulling updates, and pushing changes.
* Integrate with version control systems for automated tasks.
* Send and receive emails using command-line tools.
* Monitor email accounts for specific conditions (e.g., new messages from a particular sender).
* Scrape data from websites for information retrieval.
* Automate form submissions on websites.
* Network: Ping a list of servers to check their availability.
* Perform traceroute to analyze network paths.
* Change network configurations dynamically.
* Image and Video Processing\*\*:\*\* Manipulate images or videos using command-line tools.
* Extract frames from videos or perform basic image recognition tasks.
* Implement basic intrusion detection or prevention mechanisms.
* ✅ Control your local intranet based lighting systems without complex software.
* ✅ Invoke the speech functionality supported by the macOS's "say" command or using Windows Powershell's ability to run the System.Speech.Synthesis.SpeechSynthesizer.
* ✅ Start any program on your host machine
* ✅ Turn your machine off by issuing a "shutdown" command
* ✅ Post and retrieve content from a Cloud service
* Print things on your local printer from your Arduino!
