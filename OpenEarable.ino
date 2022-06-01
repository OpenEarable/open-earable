#include <LSM6DSRSensor.h>

#include <Wire.h>
#include <Arduino_LPS22HB.h>

#include "pitches.h"
// change this to make the song slower or faster
int tempo = 144;

// change this to whichever pin you want to use
int buzzer = D13;

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[] = {
  REST, 2, NOTE_D4, 4,
  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_G4, 2, NOTE_D5, 4,
  NOTE_C5, -2, 
  NOTE_A4, -2,
  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_F4, 2, NOTE_GS4, 4,
  NOTE_D4, -1 
//  NOTE_D4, 4
};

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms (60s/tempo)*4 beats
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;

// I2C bus
extern TwoWire Wire1;
LSM6DSRSensor AccGyr(&Wire1, LSM6DSR_I2C_ADD_L);

void setup() {
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzer, melody[thisNote], noteDuration*0.9);
    analogWrite(A0, (int)(float) melody[thisNote] / 800.0f * 255);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);
    
    // stop the waveform generation before the next note.
    noTone(buzzer);
  }

  analogWrite(A0, 0);
  
  while (!Serial);
  
  // start barometer
  if (!BARO.begin()) {
    Serial.println("Failed to initialize pressure sensor!");
    while (1);
  }

  AccGyr.begin();
  AccGyr.Enable_X();
  AccGyr.Enable_G();
}

void loop() {
  // read the sensor value
  float pressure = BARO.readPressure();
  Serial.print(pressure);
  Serial.print(" ");

  float temperature = BARO.readTemperature();
  //Serial.print(temperature);
  Serial.print(" ");

  int32_t accelerometer[3];
  int32_t gyroscope[3];
  AccGyr.Get_X_Axes(accelerometer);
  AccGyr.Get_G_Axes(gyroscope);

  //Serial.print(accelerometer[0]);
  Serial.print(" ");
  //Serial.print(accelerometer[1]);
  Serial.print(" ");
  //Serial.print(accelerometer[2]);
  Serial.print(" ");
  //Serial.print(gyroscope[0]);
  Serial.print(" ");
  //Serial.print(gyroscope[1]);
  Serial.print(" ");
  //Serial.println(gyroscope[2]);
}
