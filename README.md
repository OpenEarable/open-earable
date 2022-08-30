# open-earable

## Table of Contents
- [C++ Libraries](#C++-Libraries)
- [Python Libraries](#Python-Libraries)
- [Repository Content](#Repository-Content)
- [Arduino Nano 33 BLE - NINA 01](#Arduino-Nano-33-BLE---NINA-01)

## C++ Libraries
Recommended C++ Libraries are:
- STM32duino LSM6DSR
- Arduino_LPS22HB
- nRF52_MBED_PWM
- ArduinoBLE
- (NimBLE-Arduino)

## Python Libraries
Recommended Python Libraries are: 
- numpy
- matplotlib
- pyserial
- Flask
- bleak

## Repository Content
- EarableMEMS
- EarableMEMS_SerialSender
- Nina
- OpenEarable
- Ultra_Sonic_Scanner
- earable_web
- edge_ml_earable

### EarableMEMS
Contains the basic code operate the MEMS PDM mic and send data via serial connection to the Arduino Serial Plotter.
(Disclaimer: Code not optimized. Serial data will be lost. Testing only)

### EarableMEMS_SerialSender
Contains C/C++ code and Python script to record data of the MEMS PDM mic via a serial connection.

### Nina
Contains variant folder for the Arduino Nano 33 BLE.
(See section [Arduino Nano 33 BLE - NINA 01](#Arduino-Nano-33-BLE---NINA-01))

### OpenEarable
Contains demo code of for the internal sensors (IMU, BARO) and demo for the speaker.

### Ultra_Sonic_Scanner
Contains C/C++ code and Python script to record data of the MEMS PDM mic ultrasonic ear image via BLE.

### earable_web
Contains 2 web dashboards running with a python server.
The earable_dashboard offers a graphical dashboard with plots.
The earable_recorder offers a more simple, efficient UI with direct option to configure sensors.
Both posses recorder features.

### edge_ml_earable
Contains edge ml code for the earable.

## Arduino Nano 33 BLE - NINA 01

The original Arduino Nano 33 BLE uses the NINA-B306-00B module from u-blox.
The sister chip NINA-B306-01B is nearly identical but missing a quartz. Out of the box if the regular Arduino Nano 33 BLE bootloader is flashed, mbed os is checking for this specific quartz and since it is missing any sketch upload will brick.

The issue will be fixed long term by the Arduino devs.
At the moment there are 2 ways of fixing this issue.
One option is to manually recompile the `libmbed.a` by applying [Patch#509](https://github.com/arduino/ArduinoCore-mbed/pull/509).
However, this is a tedious process. It requires the cloning of the repositories [ArduinoCore-mbed](https://github.com/arduino/ArduinoCore-mbed), [ArduinoCore-API
](https://github.com/arduino/ArduinoCore-API), and the installation of the [Mbed CLI](https://os.mbed.com/docs/mbed-os/v6.15/quick-start/build-with-mbed-cli.html).

There is an easier quicker solution.
In the `Nina` folder is the entry of the `variants` folder for the mbed nano boards of Arduino.
By replacing the original folder of your Arduino installation with this one, you will be able to use the NINA-B306-01B without any issue.

A detailed tutorial will be provided in the following.
(Some steps may have been already performed and can be skipped)

### Install Nano Boards via Board Manager
Firstly you need to install the Arduino Mbed OS Nano Boards with the Board Manager of Arduino.
For this open Arduino and head to **Tools > Board > Board Manager**.
There search for **Arduino Mbed OS Nano Boards** and install the Arduino Mbed OS Nano Boards by Arduino.

### Flash Bootloader
In order to use the module at all you have to flash the Arduino Nano 33 BLE bootloader.
For this use a DAP Debugger tool of your choice, and connect it to your board according to the schematics.
Inside Arduino once you selected the correct board and programmer, you can simply flash the bootloader by pressing the **Burn Bootloader** button.
Now by connecting the board via USB it will show up as "Arduino Nano 33 BLE". If you attempt to flash a program to it before doing the next step, it will brick.

### Replace ARDUINO_NANO33BLE folder in variants
Download the `Nina` folder. Inside you will find the `ARDUINO_NANO33BLE` folder.

To find where Arduino installed the boards, open Arduino and head to **File > Preferences**.
There at the bottom there is a path to the `preferences.txt`, you need to head into the directory of that file.
Once you are inside this directory head to **packages > arduino > hardware > mbed_nano > [VERSION] > variants**.
(Whereas [VERSION] is the current version of the mbed nano boards firmware)

In the `variants` folder you will find the `ARDUINO_NANO33BLE` folder. Replace it with the provided folder inside the downloaded `Nina` folder.

Note that when you update the Arduino Mbed OS Nano Boards you need to repeat this procedure. This fix is compiled with the version 3.2.0 of the firmware.