#include "PDM2.h"

#if defined(ARDUINO_ARCH_NRF52840)

#include <hal/nrf_pdm.h>

#define DEFAULT_PDM_GAIN     20
#define PDM_IRQ_PRIORITY     7

#define NRF_PDM_FREQ_1280K  (nrf_pdm_freq_t)(0x0A000000UL)               ///< PDM_CLK= 1.280 MHz (32 MHz / 25) => Fs= 20000 Hz
#define NRF_PDM_FREQ_2000K  (nrf_pdm_freq_t)(0x10000000UL)               ///< PDM_CLK= 2.000 MHz (32 MHz / 16) => Fs= 31250 Hz
#define NRF_PDM_FREQ_2667K  (nrf_pdm_freq_t)(0x15000000UL)               ///< PDM_CLK= 2.667 MHz (32 MHz / 12) => Fs= 41667 Hz
#define NRF_PDM_FREQ_3200K  (nrf_pdm_freq_t)(0x19000000UL)               ///< PDM_CLK= 3.200 MHz (32 MHz / 10) => Fs= 50000 Hz
#define NRF_PDM_FREQ_4000K  (nrf_pdm_freq_t)(0x20000000UL)               ///< PDM_CLK= 4.000 MHz (32 MHz /  8) => Fs= 62500 Hz


PDMClass2::PDMClass2() {
  _onReceive = NULL;
  _gain = -1;
  _channels = -1;
  _samplerate = -1;
  _init = -1;
  _cutSamples = 0;
}

PDMClass2::~PDMClass2()
{
}

void PDMClass2::init(int dinPin, int clkPin)
{
  _dinPin = dinPin;
  _clkPin = clkPin;
}

int PDMClass2::begin(int channels, int sampleRate, int ratio)
{
  _channels = channels;
  _samplerate = sampleRate;

  // Enable high frequency oscillator if not already enabled
  if (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) { }
  }

  // configure the sample rate and channels
  switch (sampleRate) {
    case 16000:
      // Detrimine ratio to use
      int rat;
      switch (ratio) {
        case RATIO80: {
          rat = PDM_RATIO_RATIO_Ratio80;
          break;
        }
        case RATIO64: {
          rat = PDM_RATIO_RATIO_Ratio64;
          break;
        }
        default: {
          rat = PDM_RATIO_RATIO_Ratio80;
          break;
        }
      }

      NRF_PDM->RATIO = ((rat << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
      nrf_pdm_clock_set(NRF_PDM_FREQ_1280K);
      break;
    case 41667:
      nrf_pdm_clock_set(NRF_PDM_FREQ_2667K);
      break;
    default:
      return 0; // unsupported
  }

  switch (channels) {
    case 2:
      nrf_pdm_mode_set(NRF_PDM_MODE_STEREO, NRF_PDM_EDGE_LEFTFALLING);
      break;

    case 1:
      nrf_pdm_mode_set(NRF_PDM_MODE_MONO, NRF_PDM_EDGE_LEFTFALLING);
      break;

    default:
      return 0; // unsupported
  }

  if(_gain == -1) {
    _gain = DEFAULT_PDM_GAIN;
  }
  nrf_pdm_gain_set(_gain, _gain);

  // configure the I/O and mux
  pinMode(_clkPin, OUTPUT);
  digitalWrite(_clkPin, LOW);

  pinMode(_dinPin, INPUT);

  nrf_pdm_psel_connect(digitalPinToPinName(_clkPin), digitalPinToPinName(_dinPin));

  // clear events and enable PDM interrupts
  nrf_pdm_event_clear(NRF_PDM_EVENT_STARTED);
  nrf_pdm_event_clear(NRF_PDM_EVENT_END);
  nrf_pdm_event_clear(NRF_PDM_EVENT_STOPPED);
  nrf_pdm_int_enable(NRF_PDM_INT_STARTED | NRF_PDM_INT_STOPPED);

  // clear the buffer
  _doubleBuffer.reset();

  // set the PDM IRQ priority and enable
  NVIC_SetPriority(PDM_IRQn, PDM_IRQ_PRIORITY);
  NVIC_ClearPendingIRQ(PDM_IRQn);
  NVIC_EnableIRQ(PDM_IRQn);

  // set the buffer for transfer
  // nrf_pdm_buffer_set((uint32_t*)_doubleBuffer.data(), _doubleBuffer.availableForWrite() / (sizeof(int16_t) * _channels));
  // _doubleBuffer.swap();

  // enable and trigger start task
  nrf_pdm_enable();
  nrf_pdm_event_clear(NRF_PDM_EVENT_STARTED);
  nrf_pdm_task_trigger(NRF_PDM_TASK_START);

  return 1;
}

void PDMClass2::end()
{
  // disable PDM and IRQ
  nrf_pdm_disable();

  NVIC_DisableIRQ(PDM_IRQn);

  // Don't disable high frequency oscillator since it could be in use by RADIO

  // unconfigure the I/O and un-mux
  nrf_pdm_psel_disconnect();

  pinMode(_clkPin, INPUT);
}

int PDMClass2::available()
{
  NVIC_DisableIRQ(PDM_IRQn);

  size_t avail = _doubleBuffer.available();

  NVIC_EnableIRQ(PDM_IRQn);

  return avail;
}

int PDMClass2::read(void* buffer, size_t size)
{
  NVIC_DisableIRQ(PDM_IRQn);

  int read = _doubleBuffer.read(buffer, size);

  NVIC_EnableIRQ(PDM_IRQn);

  return read;
}

void PDMClass2::onReceive(void(*function)(void))
{
  _onReceive = function;
}

void PDMClass2::setGain(int gain)
{
  _gain = gain;
  nrf_pdm_gain_set(_gain, _gain);
}

void PDMClass2::setBufferSize(int bufferSize)
{
  _doubleBuffer.setSize(bufferSize);
}

void PDMClass2::IrqHandler(bool halftranfer)
{
  if (nrf_pdm_event_check(NRF_PDM_EVENT_STARTED)) {
    nrf_pdm_event_clear(NRF_PDM_EVENT_STARTED);

    if (_doubleBuffer.available() == 0) {
      // switch to the next buffer
      nrf_pdm_buffer_set((uint32_t*)_doubleBuffer.data(), _doubleBuffer.availableForWrite() / (sizeof(int16_t) * _channels));

      // make the current one available for reading
      _doubleBuffer.swap(_doubleBuffer.availableForWrite());

      // call receive callback if provided
      if (_onReceive) {
        _onReceive();
      }
    } else {
      // buffer overflow, stop
      nrf_pdm_disable();
    }
  } else if (nrf_pdm_event_check(NRF_PDM_EVENT_STOPPED)) {
    nrf_pdm_event_clear(NRF_PDM_EVENT_STOPPED);
  } else if (nrf_pdm_event_check(NRF_PDM_EVENT_END)) {
    nrf_pdm_event_clear(NRF_PDM_EVENT_END);
  }
}

extern "C" {
  __attribute__((__used__)) void PDM_IRQHandler_v(void)
  {
    PDM2.IrqHandler(true);
  }
}

PDMClass2 PDM2;

#endif
