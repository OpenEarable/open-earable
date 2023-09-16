#include "Tone.h"


Tone::Tone() {

}

Tone::~Tone() {

}

void Tone::setup() {
    //_circ = i2s_player.get_buffer();
    stream = &(i2s_player.stream);
}

short Tone::_sinewave(int tick) {
    return _short_max * sin((float)tick * _multiplier);
}

void Tone::_calc_multi() {
    _multiplier = 2.0 * PI * float(_current_frequency) / _sample_rate;
}

void Tone::start(int frequency) {
    if (!stream) return;

    int blocks = (*stream)->buffer.getBlockCount();
    _block_size = (*stream)->buffer.getBlockSize();

    if (!_original_blocks) {
        _original_blocks = blocks;
    }

    int offset = (blocks - _extra_blocks) * _block_size;

    (*stream)->buffer.setSizes(_block_size, blocks - _extra_blocks);

    _max_buffer = _extra_blocks * _block_size / 2;
    uint8_t * buf = &((*stream)->buffer.get_buffer()[offset]);
    _buffer = (short*)buf;

    // Limit frequency to boundary
    frequency = min(frequency, _frequency_high);
    frequency = max(frequency, _frequency_low);
    _current_frequency = frequency;

    // Calculate multiplier
    _calc_multi();

    // fill buffer with values
    for (int i = 0; i<_max_buffer; i++) {
        _buffer[i] = _sinewave(i);
    }

    short best_value = -_short_max;
    int best_index = 0;
    // find best 0
    for (int i = 1; i<_max_buffer-1; i++) {
        short cur = _buffer[i];
        short next = _buffer[i+1];

        if (next <= 0) continue;
        if (cur>0) continue;

        if (cur < best_value) continue;

        best_value = cur;
        best_index = i;
    }

    int counts = _max_buffer / best_index;
    _end_index = best_index;

    // fill rest buffer
    for (int i = 0; i < counts-1; ++i) {
        memcpy(&(_buffer[best_index*(i+1)]), _buffer, best_index*sizeof(short));
        _end_index += best_index;
    }
}

void Tone::end() {
    if (!stream) return;

    int size = (*stream)->buffer.getBlockSize();
    (*stream)->buffer.setSizes(size, _original_blocks);
    _original_blocks = 0;
}

int Tone::provide(int n) {
    for(int i = 0; i < n; i++) {
        update();
    }
    return n;
}

void Tone::update() {
    if ((*stream)->ready() < 2) return;

    uint8_t * ptr = (*stream)->buffer.getCurWritePointer();

    int first_block = _end_index - _cur_pos;

    int first_size = first_block*sizeof(short);
    if (first_size > _block_size) {
        memcpy(ptr, &(_buffer[_cur_pos]), _block_size);
        _cur_pos += _block_size/2;
        if (_cur_pos >= _end_index) _cur_pos -= _end_index;
        (*stream)->provide(1);
        return;
    }

    memcpy(ptr, &(_buffer[_cur_pos]), first_size);

    int second_block = _block_size/2 - first_block;

    if (!second_block) {
        (*stream)->provide(1);
        return;
    }
    int second_size = second_block*sizeof(short);
    memcpy(&(ptr[first_size]), _buffer, second_size);

    _cur_pos = second_block;

    if (_cur_pos >= _end_index) _cur_pos -= _end_index;
    (*stream)->provide(1);
}

int Tone::get_max_frequency() {
    return _frequency_high;
}

int Tone::get_min_frequency() {
    return _frequency_low;
}

/*void Audio_Player::start_tone(int frequency) {
    if (_stream) {
        return;
    }
    _tone = true;
    _stream = true;

    i2s_player.clear_buffer();

    _tone_player->start(frequency);
    i2s_player.begin();
    play();
}*/
