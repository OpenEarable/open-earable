#ifndef _PDM2_H_INCLUDED
#define _PDM2_H_INCLUDED

#include <Arduino.h>
#include <pinDefinitions.h>

#include "PDMDoubleBuffer.h"

class PDMClass2
{
public:
  PDMClass2();
  virtual ~PDMClass2();

  void init(int dinPin, int clkPin, int pwrPin);
  int begin(int channels, int sampleRate);
  void end();

  virtual int available();
  virtual int read(void* buffer, size_t size);

  void onReceive(void(*)(void));

  //PORTENTA_H7 min -12 max 51
  //NANO 33 BLE SENSe min 0 max 80
  //NICLA_VISION min 0 max 8
  void setGain(int gain);
  void setBufferSize(int bufferSize);
  size_t getBufferSize();

// private:
  void IrqHandler(bool halftranfer);

private:
  int _dinPin;
  int _clkPin;

  int _channels;
  int _samplerate;

  int _gain;
  int _init;

  int _cutSamples;

  PDMDoubleBuffer _doubleBuffer;

  void (*_onReceive)(void);
};

extern PDMClass2 PDM2;

#endif
