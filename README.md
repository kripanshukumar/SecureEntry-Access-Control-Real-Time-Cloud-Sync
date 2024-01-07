# Description
An ESP32-based authentication system that combines RFID and fingerprint technologies to authenticate users and control access. The system supports WIFI connectivity for remote communication and includes LED indicators for visual feedback. It consists of various modules and libraries responsible for storing variables, managing HTML pages, handling storage, server responses, wireless connectivity, RFID functionality, SDK functions, LED control, and fingerprint sensor integration. In the setup () function, the system initializes communication, storage, WIFI, and the fingerprint sensor if enabled, while also loading HTML pages and requesting keys. The loop () function handles WebSocket cleanup, reads RFID card information, verifies fingerprints (if enabled), and sends the corresponding access status to the server. It also sends RFID key information to the server and manages key requests and system information printing. With its comprehensive features, this firmware provides a reliable ESP32-based authentication system, empowering users to authenticate using RFID cards and fingerprints, ultimately controlling access to a specific environment or application.

# Schematic:

<p align="center"><img src="https://github.com/kripanshukumar/SecureEntry-Access-Control-Real-Time-Cloud-Sync/blob/main/Schematic/Schematic.png" width=100% height=100%></p>

# Components Used:

1.	Piezo Speaker - Used for producing audio signals, often as an audible alert or feedback mechanism.
2.	Green LED - Rect 5x2x7mm - Green light-emitting diode (LED) with a rectangular shape, commonly used for status indication.
3.	Red LED - Rect 5x2x7mm - Red light-emitting diode (LED) with a rectangular shape, commonly used for status indication and alerts.
4.	Blue LED - Rect 5x2x7mm - Blue light-emitting diode (LED) with a rectangular shape, used for status indication and visual notifications.
5.	Orange (620nm) LED - Orange light-emitting diode (LED) with a wavelength of 620nm, used for status indication and visual feedback.
6.	OLED 128x64 I2C Monochrome Display - A monochrome OLED display with 128x64 resolution, using I2C communication for displaying information.
7.	RFID-RC522 - RFID (Radio-Frequency Identification) module used for reading RFID cards or tags for user authentication.
8.	Biometric Sensor - A sensor for capturing biometric data, such as fingerprints, for biometric authentication.
9.	NPN Transistor (Package: TO-92) - A type of bipolar junction transistor with NPN configuration, commonly used for amplification and switching purposes.
10.	220Ω Resistor - A resistor with a resistance value of 220Ω, used to limit current flow or set voltage levels in circuits.
11.	Momentary Switch (Package: KSA Sealed TAC Switch) - A momentary push-button switch with a sealed design for temporary electrical connections.
12.	SD Card Module - A module that allows an SD card to be interfaced with the system for data storage and retrieval.
13.	LuaNode32 / ESP32 - The LuaNode32 board based on the ESP32 chip, providing WiFi capabilities and processing power for the system.
14.	SPDT Switch (Package: KPS-1290) - A Single Pole Double Throw (SPDT) switch with KPS-1290 package, used for selecting between two different electrical paths.
15.	USB Connector (Type: USB-A, Package: USB-A-S-NOSILK) - A USB connector in USB-A type with a USB-A-S-NOSILK package, used for data transfer and power supply.

# Working:
* The firmware is for an ESP32-based authentication system utilizing RFID and fingerprint technologies.
* In the setup() function:
  - Initializes components such as LED indicators, storage, WiFi, RFID, and fingerprint sensors.
  - Creates tasks for LED control, key updates, relay operation, and WiFi reconnection using FreeRTOS.
* In the loop() function:
  - Manages WebSocket clients for network connections.
  - Reads RFID data and stores it in 'KeyOnRFID.'
  - Performs fingerprint authentication if enabled.
  - Sends key and access status to the server based on RFID or fingerprint detection.
  - Requests keys from the server at specified intervals.
  - Updates system information and prints it periodically.
* Variables header file includes:
  - String variables for network info, URLs, and API responses.
  - uint64_t arrays for authentication keys and fingerprint positions.
  - Integers for system control.
  - Timers, semaphore, data structures, file objects, booleans, and pointers.
* HTML Code:
  - Two templates: Restart Page and Main Setup Page.
  - Restart Page displays a restart message and includes a "BACK" button.
  - Main Setup Page for configuring WiFi and device settings with "Submit," "RESET," and "RESTART" options.
  - Includes a "FILE UPLOAD" section for uploading an HTML file with a password.
* SPIFFS:
  - Functions for handling file operations and updating HTML content.
  - Includes listing directories, initializing internal storage, updating credentials and API, and searching for specific strings.
* SD CARD:
  - Code for reading and writing data to an SD card.
  - Functions for initializing storage, listing directories, reading files, loading and saving keys.
* HTML Response:
  - Various functions for handling HTTP requests and serving HTML responses.
  - Functions for setting SSID, API, restarting ESP, updating HTML content, resetting the device, and serving the homepage.
* RFID:
  - Functions for RFID card reading and initialization.
  - Includes functions to initialize, restart, and read RFID cards.
  - Steps involve card detection, UID reading, authentication, and data retrieval.
* MAIN:
  - Functions managing RFID keys and server updates.
  - Includes sorting, enrollment, relay activation, verification of fingerprints and RFID keys.
  - Tasks for updating keys from the server and monitoring WiFi connection status.
* Fingerprint Sensor:
  - Functions for initializing, capturing, searching, enrolling, and deleting fingerprints.
  - Provides information on sensor parameters and template count.
  - Functions to capture images, search for matches, enroll fingerprints, and delete templates.

 # Learnings / Skills gained:
1.	Embedded Systems Development: Working on an ESP32-based project involves understanding and programming embedded systems, which are specialized computer systems designed to perform dedicated functions. You will gain experience in working with low-level hardware components, such as GPIO pins, sensors, and communication interfaces like SPI and I2C.
2.	IoT and Wireless Connectivity: The project incorporates Wi-Fi connectivity to enable remote communication with the authentication system. This provides hands-on experience in IoT (Internet of Things) development and understanding wireless communication protocols for data exchange.
3.	RFID Technology: You will learn about RFID (Radio Frequency Identification) technology and its implementation for user authentication. This includes reading RFID cards and interpreting the unique identifiers (UIDs) to grant or deny access.
4.	Biometric Sensor Integration: The project integrates a biometric sensor to capture and verify fingerprints for authentication purposes. You'll gain experience in working with biometric sensors and understanding the processes involved in fingerprint recognition.
5.	HTML and Web Server Management: The firmware uses HTML templates to create a web-based interface for configuration and interaction. You will learn how to manage web servers, handle HTTP requests, and serve HTML pages to clients.
6.	SPIFFS and SD Card Operations: You'll gain knowledge of file systems for data storage on the ESP32, including SPIFFS (SPI Flash File System) and SD card operations. This includes reading and writing data to and from files stored on both internal and external storage.
7.	FreeRTOS and Multitasking: The use of FreeRTOS demonstrates multitasking capabilities, enabling the system to handle multiple tasks concurrently. This experience will enhance your understanding of real-time operating systems and task scheduling.
8.	Circuit Design and Component Integration: The project involves integrating various electronic components like LEDs, resistors, transistors, and a piezo speaker. You'll learn about circuit design, connecting components to microcontrollers, and proper use of different electronic components.
9.	Debugging and Troubleshooting: While developing the firmware, you might encounter issues or errors. Troubleshooting and debugging skills will be essential to identify and resolve problems in the code and hardware connections.
10.	Data Handling and Processing: The project involves processing data from various sources like RFID cards, biometric sensors, and web servers. You will learn how to handle and process different data formats for authentication and communication purposes.
11.	Version Control and Collaboration: Working on a project of this scale might involve collaboration with team members. Experience with version control systems like Git will facilitate efficient collaboration and code management.
12.	Project Management: Developing a comprehensive firmware with multiple modules requires proper project management, including planning, organization, and tracking progress.

# Images:

![image](https://github.com/kripanshukumar/SecureEntry-Access-Control-Real-Time-Cloud-Sync/assets/66938295/1084b883-8730-4a45-9616-efac604d18b9)

<p align="center"><img src="https://github.com/kripanshukumar/SecureEntry-Access-Control-Real-Time-Cloud-Sync/blob/main/Images/IMG-20210908-WA0019.jpeg" width=100% height=100%></p>

<p align="center"><img src="https://github.com/kripanshukumar/SecureEntry-Access-Control-Real-Time-Cloud-Sync/blob/main/Images/IMG_20210909_183955.jpg" width=100% height=100%></p>
