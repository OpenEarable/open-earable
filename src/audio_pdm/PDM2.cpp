#include "PDM2.h"

#if defined(ARDUINO_ARCH_NRF52840)

//#include <hal/nrf_pdm.h>
#include "nrf_pdm.h"

#define DEFAULT_PDM_GAIN     20
#define PDM_IRQ_PRIORITY     7

#define NRF_PDM_FREQ_1280K  (nrf_pdm_freq_t)(0x0A000000UL)               ///< PDM_CLK= 1.280 MHz (32 MHz / 25) => Fs= 20000 Hz [Ratio80 => Fs= 16000 Hz]
#define NRF_PDM_FREQ_2000K  (nrf_pdm_freq_t)(0x10000000UL)               ///< PDM_CLK= 2.000 MHz (32 MHz / 16) => Fs= 31250 Hz [Ratio80 => Fs= 25000 Hz]
#define NRF_PDM_FREQ_2667K  (nrf_pdm_freq_t)(0x15000000UL)               ///< PDM_CLK= 2.667 MHz (32 MHz / 12) => Fs= 41667 Hz [Ratio80 => Fs= 33333 Hz]
#define NRF_PDM_FREQ_3200K  (nrf_pdm_freq_t)(0x19000000UL)               ///< PDM_CLK= 3.200 MHz (32 MHz / 10) => Fs= 50000 Hz [Ratio80 => Fs= 40000 Hz]
#define NRF_PDM_FREQ_4000K  (nrf_pdm_freq_t)(0x20000000UL)               ///< PDM_CLK= 4.000 MHz (32 MHz /  8) => Fs= 62500 Hz [Ratio80 => Fs= 50000 Hz]

PDMClass2::PDMClass2() {
    _onReceive = NULL;
}

PDMClass2::~PDMClass2() {
}

void PDMClass2::init() {
    _blockBuffer.reset();
}

int PDMClass2::start(bool high) {
    // Enable high frequency oscillator if not already enabled
    if (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
        NRF_CLOCK->TASKS_HFCLKSTART = 1;
        while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) { }
    }

    // configure the sample rate and channels
    switch (_sampleRate) {
        case 16000:
            NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
            nrf_pdm_clock_set(NRF_PDM_FREQ_1280K);
            break;
        case 20000:
            // Untested
            nrf_pdm_clock_set(NRF_PDM_FREQ_1280K);
            break;
        case 25000:
            // Untested
            NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
            nrf_pdm_clock_set(NRF_PDM_FREQ_2000K);
            break;
        case 31250:
            // Untested
            nrf_pdm_clock_set(NRF_PDM_FREQ_2000K);
            break;
        case 33333:
            // Untested
            NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
            nrf_pdm_clock_set(NRF_PDM_FREQ_2667K);
            break;
        case 40000:
            // Untested
            NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
            nrf_pdm_clock_set(NRF_PDM_FREQ_3200K);
            break;
        case 41667:
            nrf_pdm_clock_set(NRF_PDM_FREQ_2667K);
            break;
        case 50000:
            // Untested
            if (high) {
                nrf_pdm_clock_set(NRF_PDM_FREQ_3200K);
            } else {
                NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
                nrf_pdm_clock_set(NRF_PDM_FREQ_4000K);
            }
            break;
        case 62500:
            // Untested
            nrf_pdm_clock_set(NRF_PDM_FREQ_4000K);
            break;
        default:
            return 0; // unsupported
    }

    switch (_channels) {
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
    _blockBuffer.reset();

    nrf_pdm_buffer_set((uint32_t*)_blockBuffer.getCurWritePointer(), _blockBuffer.getBlockSize() / (sizeof(int16_t) * _channels));

    // set the PDM IRQ priority and enable
    NVIC_SetPriority(PDM_IRQn, PDM_IRQ_PRIORITY);
    NVIC_ClearPendingIRQ(PDM_IRQn);
    NVIC_EnableIRQ(PDM_IRQn);

    // enable and trigger start task
    nrf_pdm_enable();
    nrf_pdm_event_clear(NRF_PDM_EVENT_STARTED);
    nrf_pdm_task_trigger(NRF_PDM_TASK_START);

    return 1;
}

int PDMClass2::start(int channels, int sampleRate, bool high) {
    _channels = channels;
    _sampleRate = sampleRate;

    return start(high);
}

void PDMClass2::end() {
    // disable PDM and IRQ
    nrf_pdm_disable();

    NVIC_DisableIRQ(PDM_IRQn);

    // Don't disable high frequency oscillator since it could be in use by RADIO

    // unconfigure the I/O and un-mux
    nrf_pdm_psel_disconnect();

    pinMode(_clkPin, INPUT);
}

int PDMClass2::available() {
    //NVIC_DisableIRQ(PDM_IRQn);

    size_t avail = _blockBuffer.available();

    //NVIC_EnableIRQ(PDM_IRQn);

    return avail;
}

int PDMClass2::read(void* buffer, size_t size) {
    //NVIC_DisableIRQ(PDM_IRQn);

    int read = (int)_blockBuffer.readBlock((uint8_t*)buffer, size);

    //NVIC_EnableIRQ(PDM_IRQn);

    return read;
}

void PDMClass2::onReceive(void(*function)(void)) {
    _onReceive = function;
}

void PDMClass2::setPins(int dinPin, int clkPin) {
    _dinPin = dinPin;
    _clkPin = clkPin;
}

void PDMClass2::setChannels(int channels) {
    _channels = channels;
}

void PDMClass2::setSampleRate(int sampleRate) {
    _sampleRate = sampleRate;
}

void PDMClass2::setGain(int gain) {
    _gain = gain;
    nrf_pdm_gain_set(_gain, _gain);
}

void PDMClass2::setBlockBufferSizes(int blockSize, int blockCount) {
    _blockBuffer.setSizes(blockSize, blockCount);
}

size_t PDMClass2::getTotalSize() const {
    return _blockBuffer.getTotalSize();
}

size_t PDMClass2::getBlockSize() const {
    return _blockBuffer.getBlockSize();
}

size_t PDMClass2::getBlockCount() const {
    return _blockBuffer.getBlockCount();
}

void PDMClass2::IrqHandler(bool halftranfer) {
    if (nrf_pdm_event_check(NRF_PDM_EVENT_STARTED)) {
        nrf_pdm_event_clear(NRF_PDM_EVENT_STARTED);
        if (!_blockBuffer.check_collision_error()) {

            if (!_first) {
                _blockBuffer.incrementWritePointer();
            } else {
                _first = false;
            }

            // switch to the next buffer
            nrf_pdm_buffer_set((uint32_t*)_blockBuffer.getNextWritePointer(), _blockBuffer.getBlockSize() / (sizeof(int16_t) * _channels));

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

void PDMClass2::resetBuffer() {
    _blockBuffer.reset();
    _first = true;
}

void PDMClass2::clearBuffer() {
    _blockBuffer.clear();
}

bool PDMClass2::checkCollision() {
    return _blockBuffer.check_collision_error();
}

uint8_t *PDMClass2::getReadPointer() {
    return _blockBuffer.getReadPointer();
}

void PDMClass2::incrementReadPointer() {
    _blockBuffer.incrementReadPointer();
}

bool PDMClass2::checkSampleRateValid(int sampleRate) {
    for (int rate : valid_sample_rates) {
        if (rate == sampleRate) {
            return true;
        }
    }
    return false;
}

extern "C" {
__attribute__((__used__)) void PDM_IRQHandler_v(void)
{
    PDM2.IrqHandler(true);
}
}

PDMClass2 PDM2;

#endif
