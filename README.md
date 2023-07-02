# open-earable

OpenEarable is a new, open-source, Arduino-based platform for ear-based sensing applications. It provides a versatile prototyping platform with support for various sensors and actuators, making it suitable for earable research and development. 

## Table of Contents
- [Introduction](#Introduction)
- [C++ Libraries](#C++-Libraries)
- [Earable EdgeML](#Earable-EdgeML)
- [SD Setup](#SD-Setup)
  - [SD Card Setup](#SD-Card-Setup)
  - [sdfat Library Setup](#sdfat-Library-Setup)
- [SD Data Logging](#SD-Data-Logging)
- [PDM Mic Recording](#PDM-Mic-Recording)
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
- SdFat (Bill Greiman)


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

### sdfat Library Setup
One of the library requirements is the SdFat library from Bill Greiman.
This library is used to send data to the SD card.
By default, on the Open Earable data transfer is very slow topping about 33kbps.
This is not sufficient to record audio.

However, with the SdFat library it is possible to get write speeds of up to 400kbps.
For this, the SdFatConfig of the SdFat needs to be modified.

Go to the SdFat library folder and find the `SdFatConfig.h`.
Here enable `USE_SPI_ARRAY_TRANSFER` by setting the `0` to `1`.

## PDM Mic Recording
Open Earable is equipped with an Ultrasonic MEMS microphone that enables high-quality audio recording.

By default the PDM Mic is disabled.
To enable it the `edge_ml_earable.enable_audio()` methods needs to be called before the `begin()` is executed.

The Microphone is regarded as its own sensor under the EdgeML framework. Setting its sampling to a non-zero value will start the audio recording. The returned sensor value with EdgeML is the number of currently active sensors.

__Note:__ PDM Mic and SD Data Logging are mutually exclusive.

__Note:__ Using other sensors with high sample rate with the PDM Mic may result in unexpected behavior and inconsistencies in the recorded data.

Further functionality regarding the PDM Mic must be called after `enable_audio()` and before `begin()` and includes:

#### `void set_audio_file_name(String name)`

Sets the name of the audio file. The default name is "Recording.wav".

```c++
edge_ml_earable.set_audio_file_name("AudioFile.csv");
```

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


```c++
edge_ml_earable.setSampleRate(16000);
```

#### `void setGain(int gain)`

Sets the gain of the PDM Mic. The default value is `20` and the maximum is `80`.

```c++
edge_ml_earable.setGain(0);
```

#### `void enable_serial_data()`

Instead of saving the data to an SD card it can also be sent via the USB Serial port.
The raw PDM stream consisting of shorts will be sent via USB Serial.

By default seral sending is disabled.

```c++
edge_ml_earable.enable_serial_data();
```

#### `void disable_serial_data()`

Disables serial data sending, if enabled.

```c++
edge_ml_earable.disable_serial_data();
```

#### `void enable_chunks()`

Allows to send all Audio data chunks at once to SD card, instead of one chunk per loop.

This feature is by default enabled.

```c++
edge_ml_earable.enable_chunks();
```

#### `void disable_chunks()`

Sends all only one chunk of audio data to SD card per loop.

```c++
edge_ml_earable.disable_chunks();
```

## SD Data Logging

Open Earable supports SD data logging, allowing you to store recorded sensor values to an SD card.

When enabled, Open Earable creates a ".csv" file on the SD card with a standardized header format consisting of:

`ID, TIMESTAMP, Data1, Data2, Data3`

To enable the sd card logging call the `edge_ml_earable.enable_sd_logging()` method before the `begin()` method.

__Note:__ PDM Mic and SD Data Logging are mutually exclusive.

Further functionality regarding the SD Logging must be called after `enable_sd_logging()` and before `begin()` and includes:

#### `void set_logger_file_name(String name)`

Sets the name of the logging file. The default name is `Logging.csv`.

```c++
edge_ml_earable.set_logger_file_name("LoggingFile.csv");
```

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
