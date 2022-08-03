#include "PDM2.h"

// default number of output channels
static const char channels = 1;

// default PCM output frequency
static const int frequency = 16000;

const int buffer_size = 1024;

// Buffer to read samples into, each sample is 16-bits
byte sampleBuffer[buffer_size];

// Number of audio samples read
volatile int samplesRead;

const int pin_pdm_in = 22;
const int pin_pdm_clk = 29;

bool stream = 0;

void setup() {
  Serial.begin(1000000);
  while (!Serial);
  
  PDM2.init(pin_pdm_in, pin_pdm_clk);
  PDM2.setBufferSize(buffer_size);

  // Configure the data receive callback
  PDM2.onReceive(onPDMdata);

  // Optionally set the gain
  // Defaults to 20 on the BLE Sense and 24 on the Portenta Vision Shield
  // PDM.setGain(30);

  // Initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate for the Arduino Nano 33 BLE Sense
  // - a 32 kHz or 64 kHz sample rate for the Arduino Portenta Vision Shield
  if (!PDM2.begin(channels, frequency)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }
}

void loop() {
  if (Serial.available()) {
    char data = Serial.read();
    if (data == '1') {
      Serial.print('1');
      stream = true;
    } else {
      stream = false;  
    }
  }
  
  // Wait for samples to be read
  if (samplesRead && stream) {
    Serial.write(sampleBuffer, sizeof(sampleBuffer));
    // Clear the read count
    samplesRead = 0;
  }
}

/**
 * Callback function to process the data from the PDM microphone.
 * NOTE: This callback is executed as part of an ISR.
 * Therefore using `Serial` to print messages inside this function isn't supported.
 * */
void onPDMdata() {
  // Query the number of available bytes
  int bytesAvailable = PDM2.available();

  // Read into the sample buffer
  PDM2.read(sampleBuffer, bytesAvailable);

  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}
