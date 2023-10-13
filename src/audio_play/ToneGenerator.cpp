#include "ToneGenerator.h"

//const float _2_PI = 2.f * PI;
const float pi = PI;

float triangle(float f) {
    return 1-abs(abs(2*f / pi - 3)-2);
}

float square(float f) {
    return f <= pi ? 1 : -1;
}

float saw(float f) {
    return f / pi - (f > pi ? 2 : 0);
}

ToneGenerator::ToneGenerator(float frequency, float amplitude, Waveform type, float sample_rate) : _sample_rate(sample_rate), _frequency_high(sample_rate/2) {
    set_frequency(frequency);
    set_amplitude(amplitude);
    set_waveform(type);
}

ToneGenerator::ToneGenerator(Tone (*modulation)(), int call_every, Waveform type, float sample_rate) : _sample_rate(sample_rate), _frequency_high(sample_rate/2) {
    _modulation = modulation;
    this->call_every = call_every;
    set_waveform(type);
}

ToneGenerator::~ToneGenerator() {

}

void ToneGenerator::set_frequency(float frequency) {
    this->_tone.frequency = constrain(frequency, _frequency_low, _frequency_high);
}

void ToneGenerator::set_amplitude(float amplitude) {
    this->_tone.amplitude = constrain(amplitude, 0, 1);
}

void ToneGenerator::set_waveform(Waveform type) {
    switch (type)
    {
    case SINE:
        wave = sinf;
        break;
    case SQUARE:
        wave = square;
        break;
    case TRIANGLE:
        wave = triangle;
        break;
    case SAW:
        wave = saw;
        break;
    default:
        wave = sinf;
        break;
    }
}

bool ToneGenerator::begin() {
    if (!stream) return false;

    (*stream)->buffer.clear();
    (*stream)->open();

    _block_size = (*stream)->buffer.getBlockSize();

    _delta = 2.f * PI / _sample_rate;

    _t = 0;
    t_call = 0;

    const int _preload_blocks = 6;

    int cont = provide(_preload_blocks);

    Serial.print("preload blocks: ");
    Serial.println(cont);

    _available = true;

    return true;
}

bool ToneGenerator::available() {
    return _available;
}

void ToneGenerator::end() {
    _available = false;
    if (!stream) return;
    (*stream)->close();
}

void ToneGenerator::setStream(BufferedStream ** stream) {
    if (_available) end();
    this->stream = stream;
}

int ToneGenerator::provide(int n) {
    for(int i = 0; i < n; i++) {
        update();
    }
    return n;
}

void ToneGenerator::update() {
    if (!_available) return;
    //if ((*stream)->ready() < 2) return;

    int16_t * ptr = (int16_t*)((*stream)->buffer.getWritePointer());

    for (int i = 0; i < _block_size / sizeof(int16_t); i ++) {
        ptr[i] = MAX_INT16 * (_tone.amplitude * wave(_delta * _t));
        _t += _tone.frequency;
        _t = _t >= _sample_rate ? _t - _sample_rate : _t;
        if (_modulation && ++t_call >= call_every) {
            _tone = _modulation();
            t_call -= call_every;
        }
    }

    (*stream)->provide(1);
}

int ToneGenerator::get_max_frequency() {
    return _frequency_high;
}

int ToneGenerator::get_min_frequency() {
    return _frequency_low;
}

float ToneGenerator::getSampleRate() {
    return _sample_rate;
}

WAVConfigurationPacket ToneGenerator::get_config() {
    String _name = String(_tone.frequency) + "Hz";

    WAVConfigurationPacket wav_packet;
    wav_packet.state = 0;
    wav_packet.size = _name.length();

    for (int i=0; i<_name.length(); i++) {
        wav_packet.name[i] = _name[i];
    }
    return wav_packet;
}
