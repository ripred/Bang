# ArduinoCLI
Make your PC a "service" for your Arduino and execute any commands on it's behalf and receive the captured results! Play music, use the PC's disk drive, issue curl commands to post or retrieve anything on the web! The possibilities are endless!

So far I have written and added the following sketches to the PublicGallery folder:

-   **[ArduinoCLI.ino](https://github.com/ripred/ArduinoCLI/blob/main/PublicGallery/arduinoCLI/arduinoCLI.ino)** example sketch
-   **[macFreeDiskSpace.ino](https://github.com/ripred/ArduinoCLI/blob/main/PublicGallery/macFreeDiskSpace/macFreeDiskSpace.ino)** sketch to monitor and blink an LED if your PC/Mac/Linux disk drive falls below a a certain amount of free space
-   **[macPlayMusic.ino](https://github.com/ripred/ArduinoCLI/blob/main/PublicGallery/macPlayMusic/macPlayMusic.ino)** sketch to play any song in your music library when your Arduino sketch tells it to play it
-   **[macSpeechSynthesizer.ino](https://github.com/ripred/ArduinoCLI/blob/main/PublicGallery/macSpeechSynthesizer/macSpeechSynthesizer.ino)** sketch to make your Mac speak anything your Arduino tells it to
-   **[openWeatherMap.ino](https://github.com/ripred/ArduinoCLI/blob/main/PublicGallery/openWeatherMap/openWeatherMap.ino)** sketch to retrieve the city name, latitude, longitude, current conditions, temperature, feels like temperature, humidity, wind speed, and wind direction for any zip code.
-   **[datetime.ino](https://github.com/ripred/ArduinoCLI/blob/main/PublicGallery/datetime/datetime.ino)** sketch to retrieve the current date and time from Windows, Mac, and Linux hosts!
-   **[isMacAsleep.ino](https://github.com/ripred/ArduinoCLI/blob/main/PublicGallery/isMacAsleep/isMacAsleep.ino)** sketch to retrieve whether the host machine is asleep or not! Special thanks to reddit user u/NoBodyDroid for the idea!
-   **[power.ino](https://github.com/ripred/ArduinoCLI/blob/main/PublicGallery/power/power.ino)** sketch for Windows, Mac, and Linux to tell the host macine to go to sleep, reboot, or shutdown

The following are some of the ideas I have had that this technique can be used for:

* Invoke 'curl' commands to send internet requests and optionally retrieve the response back to your Arduino. Many of the following ideas are just expanded ideas of this basic ability.
* Play and stop music or movies on your host machine
* Retrieve weather info and control devices based on the results
* Retrieve sports updates using public API's such as NHL's and MLB's api's
* Write and retrieve data to files on your host machine and take advantage of it's muchlarger capacity versus an SD card! Basically all file functionality that you can do from the command line like creating files, appending to them, reading them back, and deleting them, etc.
* Post or retrieve posts with reddit without using a complex reddit api (and their limitations! 😉)
* Access and use a running database server on your host machine
* Submit sensor data to running machine learning training
* Submit Queries to a running machine learning model
* Retrieve the current time from the host machine
* Post updates or retrieve information from social media platforms.
* Monitor social media channels for specific keywords or trends.
* Retrieve and manipulate calendar events.
* Set reminders or schedule tasks on the host machine.
* Monitor system security logs for security events.
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
* Control your local intranet based lighting systems without complex software.
* Invoke the speech functionality supported by the macOS's "say" command or using Windows Powershell's ability to run the System.Speech.Synthesis.SpeechSynthesizer.
* Start any program on your host machine
* Turn your machine off by issuing a "shutdown" command
* Post and retrieve content from a Cloud service
* Print things on your local printer from your Arduino!
