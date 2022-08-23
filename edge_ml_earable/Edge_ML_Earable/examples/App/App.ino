/* 
 * This sketch allows you to connect your Earable to Edge-ML over Web Bluetooth.
 * 
 * app.edge-ml.org
 * 
 * Firmware-version: 1.0.0
 * Release-date: 23.08.2022
*/

#include "Arduino.h"
#include "EdgeML-Earable.h"

// Set DEBUG to true in order to enable debug print
#define DEBUG false

void setup()
{
#if DEBUG
  Serial.begin(115200);
  edge_ml.debug(Serial);
#endif

  edge_ml.begin();
}

void loop()
{
  // Update and then sleep
  edge_ml.update();
}