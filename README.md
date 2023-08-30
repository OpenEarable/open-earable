# open-earable

OpenEarable is a new, open-source, Arduino-based platform for ear-based sensing applications. It provides a versatile prototyping platform with support for various sensors and actuators, making it suitable for earable research and development.

## Table of Contents
- [Introduction](#Introduction)
- [C++ Libraries](#C++-Libraries)
- [Earable EdgeML](#Earable-EdgeML)
- [SD Setup](#SD-Setup)
  - [SD Card Setup](#SD-Card-Setup)
  - [SPI Setup](#SPI-Setup)
  - [sdfat Library Setup](#sdfat-Library-Setup)
- [Pin Configuration](#Pin-Configuration)
- [Usage and functionality](#Usage-and-functionality)
  - [Button](#Button)
- [Sensor Configuration](#Sensor-Configuration)
- [Cite](#Cite)

## Introduction

OpenEarable is designed to enable ear-based sensing applications by offering a flexible and open-source hardware platform. It incorporates a range of sensors, including a 9-axis inertial measurement unit, an ear canal pressure and temperature sensor, an inward-facing ultrasound microphone, a speaker, a push button, and a controllable LED. With these features, OpenEarable provides researchers and developers with the ability to explore various application scenarios. 
For more information visit the [OpenEarable](https://open-earable.teco.edu/) website.

All interaction with the microcontroller takes place via BLE (Bluetooth Low Energy) with [EdgeML](https://edge-ml.org/) or the provided Dashboard {Dashboard Link}.

The Open Earable is running the Arduino Nano 33 BLE bootloader with Mbed OS. 

## C++ Libraries
Needed C++ Libraries:
- EdgeML-Arduino
- Adafruit_BMP280
- DFRobot_BMX160
- ArduinoBLE
- SdFat Adafruit Fork (Bill Greiman)


## Earable EdgeML

Open Earable is seamlessly integrated with EdgeML, an open-source and browser-based toolchain for machine learning on microcontrollers.
For more detailed information on how to use EdgeML, refer to the [EdgeML](https://edge-ml.org/) website.

The easiest way to use open earable with edge-ml is with the provided App sketch. The absolute minimum needed to run the code successfully is the following:

```c++
#include "EdgeML_Earable.h"

void setup() {
  edge_ml_earable.begin();
}

void loop() {
  edge_ml_earable.update();
}
```

However, there are a few more functionalities, which the EdgeML Earable code offers. Those will be further discussed in the following chapters.


## SD Setup
For the following functionality an SD card is required. 
For its use with Open Earable, the SD card needs to be prepared as well as the SdFat library configuration changed.

### SD Card Setup
In order to be compatible with this library the SD card needs to be formatted with the exFAT format.
Make sure to have a sufficiently fast SD card. (For prototyping tests SandDisk class 10 and class A30 were used)


### SPI Setup

The default implementation of the SPI library does not meet the required speed. To address this, optimized SPI files are provided. Follow the steps below to integrate these files into Arduino.
All referenced files can be found in the "additional" folder in the "spi_files" subfolder.

##### Updating Arduino Nano 33 BLE Board Files

To fully integrate the optimized SPI files, changes to the Arduino Nano 33 BLE board files have to be made. Follow the steps below to accomplish this:

1. Open the Arduino IDE.

2. In the top left corner, click on "File."

3. From the dropdown menu, select "Preferences."

4. At the bottom of the new window, there is a hyperlink labeled under "More preferences can be edited directly in the file". Click on this hyperlink to access the file explorer.

5. Inside the "packages" folder, navigate to the following directory: `arduino/hardware/mbed_nano/4.0.4/libraries`.

6. Locate the existing SPI library folder within this directory. It needs to be swapped with the provided folder named "SPI".

7. Similarly, navigate to the following directory: `arduino/hardware/mbed_nano/4.0.4/cores/arduino/mbed/targets/TARGET_NORDIC/TARGET_NRF5x/TARGET_SDK_15_0/modules/nrfx/drivers/include`.

8. In this directory, locate the file named "nrfx_spi.h" and replace it with the provided "nrfx_spi.h" file from the "spi_files" folder.

9. Additionally, paste the "nrfx_spim.c" file from the "spi_files" subfolder into the same directory.


### sdfat Library Setup
One of the library requirements is the Adafruit Fork of the SdFat library from Bill Greiman.
This library is used to send data to the SD card.
By default, on the Open Earable data transfer is very slow topping about 33kbps.
This is not sufficient to record audio.

However, with the SdFat library it is possible to get write speeds of up to 400kbps.
For this, the SdFatConfig of the SdFat needs to be modified.

Go to the SdFat library folder and find the `SdFatConfig.h`.
Replace it with the provided config file found under "additional/sdfat_config"

The most notable change was the activation of the `USE_SPI_ARRAY_TRANSFER` flag.

## Pin Configuration

A handy header file with all the pin definition is provided. For reference look at the "Earable_Pins.h".

## Usage and functionality

The easiest way to use edge-ml is with the provided `App` sketch.

However, there are a few more functionalities.

Note: The majority of the following functions should be called BEFORE the `edge_ml_earable.begin()` function.

#### `void debug(Stream &stream)`

Set a IO stream for debugging purposes.

```c++
edge_ml_earable.debug(Serial);
```

#### `void enable_sd_logging()`

Enables SD card data logging. Recorded sensor values are automatically saved to the SD card. (disabled by default)

When enabled, Open Earable creates a ".csv" file on the SD card with a standardized header format consisting of:

`ID, TIMESTAMP, Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data8, Data9`

#### `void set_logger_file_name(String name)`

Set name of logging file. (".csv" file preferred)

#### `void set_playerfile_prefix(String name)`

Set prefix of playback file.

```c++
edge_ml_earable.set_playerfile_prefix("Play");
```

Due to the file selecting feature the files on the SD card should be named in the following scheme:
<br>
"Play_1.wav"
<br>
"Play_2.wav"
<br>
"Play_3.wav"

Currently up the 10 files are supported. (1-10)

#### `void set_recorder_file_name(String name)`

Set name of recording file. (".wav" file preferred)

#### `void setSampleRate(int sampleRate)`

Sets the sampling rate of the PDM Mic. The default value is 41667Hz.

The available sampling rates are:

- 16000Hz
- 20000Hz
- 25000Hz
- 31250Hz
- 33333Hz
- 40000Hz
- 41667Hz
- 50000Hz
- 62500Hz

Most recommended are:
- 16000Hz
- 41667Hz
- 62500Hz

#### `void setGain(int gain)`

Sets the gain of the PDM Mic. The default value is `20` and the maximum is `80`.

#### `void enable_serial_data()`

Instead of saving the data to an SD card it can also be sent via the USB Serial port.
The raw PDM stream consisting of shorts will be sent via USB Serial.

By default seral sending is disabled.

#### `void disable_serial_data()`

Disable USB serial streaming of audio recording, if enabled.

#### `void configure_sensor(SensorConfigurationPacket& config)`

Send a configuration package from within the code.

### Button

The earable features a button at its side. A software debounced interface is already included with the `earable_btn` Button instance.

it includes the following functionality: 

#### `bool get_pressed()`

Get state of the button.

#### `bool get_held()`

If the button is held down for more than a predetermined time, `true` is returned. (Default time: 1s)

#### `bool get_pressed_single()`

Get state of button once otherwise returns `false`.

#### `bool get_held_single()`

Get hold state of button once otherwise returns `false`.

#### `void setDebounceTime(unsigned long debounceTime)`

Set debounce time in ms. (Default 50ms)

#### `void setHoldTime(unsigned long holdTime)`

Set hold time in ms. (Default 1000ms)

## Sensor Configuration

Via the EdgeML pipeline or the `configure_sensor` function the sensor, PDM mic, and audio playback can be started or stopped.

The sensor IDs can be found in the "src/custom_sensor/SensorID_Earable.h".

A configuration packet is a struct following the following structure:

```c++
struct SensorConfigurationPacket {
    uint8_t sensorId{};
    float sampleRate{};
    uint32_t latency{};
};
```

sensorId hold the ID of the sensor.<br>
sampleRate holds the sample rate. <br>
latency is a legacy field and can be mostly ignored. However, it has been repurposed as shown later.

Each sensor or audio IO can be enabled individually or together at the same time with predefined configurations.
It is recommended to use the predefined configurations.

The available predefined Sensors:
### IMU
ID: 0

The IMU provides acceleration, gyroscope, and magnetometer values in xyz.
<br>
(max. 50Hz alone; max. 30Hz with other sensors)


### BME280
ID: 1

The BME provides in ear air pressure measurements as well as temperature data of the earable.
<br>
(max. 50Hz alone; max. 30Hz with other sensors)

### PDM MIC
ID: 2

The PDM Microphone provides audio data up to 62.5kHz.
The sample rate files of the configuration package is the audio sample rate of the sensor.

### PLAYER
ID: 3

The Player sensor controls the playback of audio.
The sample rate files serves multiple purposes depending on its value:

- 0: Stops the current playback
- 1-10: Select and play a file corresponding to the selected number
- 300-22000: Plays a constant tone with the sample rate representing the integer frequency. (Min. 300Hz, max. 22000kHz)

### CONFIGURATION
ID: 4

With the "virtual" configuration sensor a predefined configuration of different activated sensors can be  selected.
This is NECESSARY if the audio elements are supposed to run alongside the other sensors.

The sample rate represents the chosen configuration.

Here the latency field becomes important. If any configuration with audio playback is selected, the latency field hold the selection parameter analogous to the "PLAYER" sensors settings.
The only difference being that a latency field set to 0 will always trigger the audio file 1 instead of stopping all playback.

Note: Once a new configuration is received all sensors will be stopped before the new configuration is started.

Available configurations

| CONFIG | IMU    | BME280 | PDM      | PLAYER |
|--------|--------|--------|----------|--------|
| 0      | OFF    | OFF    | OFF      | OFF    |
| 1      | 30 Hz  | 30 Hz  | 62500 Hz | -      |
| 2      | 30 Hz  | 30 Hz  | 41667 Hz | -      |
| 3      | 30 Hz  | 30 Hz  | -        | -      |
| 4      | 20 Hz  | 20 Hz  | 62500 Hz | -      |
| 5      | 20 Hz  | 20 Hz  | 41667 Hz | -      |
| 6      | 20 Hz  | 20 Hz  | -        | -      |
| 7      | 30 Hz  | 30 Hz  | 41667    | ON     |
| 8      | 30 Hz  | 30 Hz  | -        | ON     |
| 9      | 20 Hz  | 20 Hz  | 41667    | ON     |
| 10     | 20 Hz  | 20 Hz  | -        | ON     |
| 11     | -      | -      | 62500 Hz | ON     |
| 12     | -      | -      | 41667 Hz | ON     |
| 13     | -      | -      | -        | ON     |
| (14)   | 30 Hz  | 30 Hz  | 62500 Hz | ON     |
| (15)   | 20 Hz  | 20 Hz  | 62500 Hz | ON     |


__NOTE: Config 14 and 15 are unstable and are NOT recommended__

## Cite
```bib
@inproceedings{10.1145/3544793.3563415,
	title        = {OpenEarable: Open Hardware Earable Sensing Platform},
	author       = {Röddiger, Tobias and King, Tobias and Roodt, Dylan Ray and Clarke, Christopher and Beigl, Michael},
	year         = 2023,
	booktitle    = {Proceedings of the 1st International Workshop on Earable Computing},
	location     = {Cambridge, United Kingdom},
	publisher    = {Association for Computing Machinery},
	address      = {New York, NY, USA},
	series       = {EarComp’22},
	pages        = {29–34},
	doi          = {10.1145/3544793.3563415},
	url          = {https://doi.org/10.1145/3544793.3563415},
	numpages     = 6,
	keywords     = {In-Ear Headphones, IMU, Monitoring}
}
```
