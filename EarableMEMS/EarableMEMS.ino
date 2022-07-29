#include "PDM2.h"

// default number of output channels
static const char channels = 1;

// default PCM output frequency
static const int frequency = 16000;

// Buffer to read samples into, each sample is 16-bits
short sampleBuffer[512];

// Number of audio samples read
volatile int samplesRead;

const int pin_pdm_in = 22;
const int pin_pdm_clk = 29;

unsigned long loop_count = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  PDM2.init(pin_pdm_in, pin_pdm_clk);

  // Configure the data receive callback
  PDM2.onReceive(onPDMdata);

  // Optionally set the gain
  // Defaults to 20 on the BLE Sense and 24 on the Portenta Vision Shield
  // PDM.setGain(30);

  // Initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate for the Arduino Nano 33 BLE Sense
  // - a 32 kHz or 64 kHz sample rate for the Arduino Portenta Vision Shield
  if (!PDM2.begin(channels, frequency, RATIO64)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }
}

void loop() {
  // Wait for samples to be read
  loop_count++;
  if (samplesRead) {

    // Print samples to the serial monitor or plotter
    for (int i = 0; i < samplesRead; i++) {
      if(channels == 2) {
        if (i < samplesRead/2){
          Serial.print("L:");
          Serial.print(sampleBuffer[i]);
          Serial.print(" R:");
          Serial.println(0);
        
        } else {
          Serial.print("L:");
          Serial.print(0);
          Serial.print(" R:");
          Serial.println(sampleBuffer[i]);
        }
      } else {
        Serial.println(sampleBuffer[i]);
      }
    }
    loop_count = 0;

    //Serial.println(samplesRead);

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
