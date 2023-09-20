#include "Tone.h"

Tone::Tone(float frequency, float amplitude) {
    //stream = &(i2s_player.stream);
    set_frequency(frequency);
    set_amplitude(amplitude);
}

Tone::Tone(ToneSetting (*modulation)(), int call_every) {
    _modulation = modulation;
    this->call_every = call_every;
}

Tone::~Tone() {

}

void Tone::set_frequency(float frequency) {
    this->_tone.frequency = constrain(frequency, _frequency_low, _frequency_high);
}

void Tone::set_amplitude(float amplitude) {
    this->_tone.amplitude = constrain(amplitude, 0, 1);
}

bool Tone::begin() {
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

bool Tone::available() {
    return _available;
}

void Tone::end() {
    _available = false;
    if (!stream) return;
    (*stream)->close();
}

void Tone::setStream(BufferedStream ** stream) {
    if (_available) end();
    this->stream = stream;
}

int Tone::provide(int n) {
    for(int i = 0; i < n; i++) {
        update();
    }
    return n;
}

void Tone::update() {
    if (!_available) return;
    //if ((*stream)->ready() < 2) return;

    int16_t * ptr = (int16_t*)((*stream)->buffer.getCurWritePointer());

    for (int i = 0; i < _block_size / sizeof(int16_t); i ++) {
        ptr[i] = MAX_INT16 * (_tone.amplitude * sinf(_delta * _t));
        _t += _tone.frequency;
        _t = _t >= _sample_rate ? _t - _sample_rate : _t;
        if (_modulation && ++t_call >= call_every) {
            _tone = _modulation();
            t_call -= call_every;
        }
    }

    (*stream)->provide(1);
}

int Tone::get_max_frequency() {
    return _frequency_high;
}

int Tone::get_min_frequency() {
    return _frequency_low;
}

WAVConfigurationPacket Tone::get_config() {
    String _name = String(_tone.frequency) + "Hz";

    WAVConfigurationPacket wav_packet;
    wav_packet.state = 0;
    wav_packet.size = _name.length();

    for (int i=0; i<_name.length(); i++) {
        wav_packet.name[i] = _name[i];
    }
    return wav_packet;
}
