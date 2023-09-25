#include "Audio_Player.h"

#include <utility>

uint8_t AUDIO_BUFFER[audio_b_size * audio_b_count] __attribute__((aligned (16)));

bool Audio_Player::_paused = false;

Audio_Player::Audio_Player() {
    _tone_player = new Tone();
}

Audio_Player::~Audio_Player() {
    delete[] _tone_player;
}

bool Audio_Player::init() {
    i2s_setup();
    _tone_player->setup(); // Tone setup after i2s_setup!
    return sd_setup();
}

void Audio_Player::i2s_setup() {
    // i2s_player.setBlockBufferSizes(_blockSize, _blockCount);
    i2s_player.setBuffer(AUDIO_BUFFER, audio_b_size, audio_b_count);
}

bool Audio_Player::sd_setup() {
    if(!sd_manager.begin()) return false;
    open_file();
    return _file.isOpen();
}

void Audio_Player::update() {
    if (!_stream) return _check_ready();;

    if (_tone) {
        _tone_player->update();
        return;
    }

    if (check_completed()) return;
    if (i2s_player.available() < 2) return;


    unsigned int read = sd_to_buffer();
    if (read == 0) {
        i2s_player.completed();
    }
}

int Audio_Player::update_contiguous(int max_cont) {
    if (!_stream) {
        _check_ready();
        return 0;
    }


    if (_tone) {
        _tone_player->update();
        return 0;
    }

    if (check_completed()) return 0;

    int blocks = i2s_player.get_contiguous_blocks();

    int cont = min(blocks, max_cont);

    if (!cont) return 0;

    unsigned int read = sd_to_buffer(cont);

    if (read == 0) {
        i2s_player.completed();
    }
    return cont;
}

void Audio_Player::start_tone(int frequency) {
    if (_stream) {
        return;
    }
    _tone = true;
    _stream = true;

    i2s_player.clear_buffer();

    _tone_player->start(frequency);
    i2s_player.start();
    play();
}

void Audio_Player::start() {
    if (_stream) {
        return;
    }
    _tone = false;
    _stream = true;

    i2s_player.clear_buffer();

    if (!open_file()) {
        _stream = false;
        return;
    }

    _cur_read_sd = _default_offset;
    _file.seekSet(_default_offset);

    i2s_player.reset_buffer();
    preload_buffer();
    i2s_player.start();
    play();
}

void Audio_Player::end() {
    if (!_stream) {
        return;
    }
    _stream = false;

    stop();
    i2s_player.end();
    if (_tone) {
        _tone_player->end();
    }
}

void Audio_Player::set_name(String name) {
    _name = std::move(name);
    _opened = false;
}

void Audio_Player::play() {
    i2s_player.play();
}

void Audio_Player::stop() {
    i2s_player.stop();
}

void Audio_Player::pause() {
    i2s_player.pause();
}

void Audio_Player::preload_buffer() {
    if (!_tone) {
        update_contiguous(_preload_blocks);
        return;
    }

    // Preload tones
    for (int i = 0; i < _preload_blocks; ++i) {
        _tone_player->update();
    }
}

unsigned int Audio_Player::sd_to_buffer() {
    uint8_t * ptr = i2s_player.getWritePointer();

    unsigned int read =sd_manager.read_block(&_file, ptr, audio_b_size);
    _cur_read_sd += read;

    if (read != 0) {
        i2s_player.incrementWritePointer();
    }
    return read;
}

unsigned int Audio_Player::sd_to_buffer(int multi) {
    uint8_t * ptr = i2s_player.getWritePointer();

    unsigned int read_total = 0;
    unsigned int read = sd_manager.read_block(&_file, ptr, audio_b_size * multi);
    _cur_read_sd += read;
    read_total += read;

    if (read != 0) {
        for (int i = 0; i < multi; ++i) {
            i2s_player.incrementWritePointer();
        }
    }

    return read_total;
}

bool Audio_Player::check_completed() {
    if (i2s_player.get_completed() && i2s_player.get_end()) {
        end();
        return true;
    }
    return false;
}

int Audio_Player::get_max_frequency() {
    return _tone_player->get_max_frequency();
}

int Audio_Player::get_min_frequency() {
    return _tone_player->get_min_frequency();
}

bool Audio_Player::is_mode_tone() {
    return _tone;
}

unsigned int Audio_Player::get_sample_rate() {
    if (!open_file()) return 0;
    uint64_t pos = _file.curPosition();
    unsigned int rate;
    sd_manager.read_block_at(&_file, 24, (uint8_t *) &rate, 4);
    _file.seekSet(pos);
    return rate;
}

unsigned int Audio_Player::get_size() {
    if (!open_file()) return 0;
    // uint64_t pos = _file.curPosition(); // old
    return _file.fileSize();
}

int Audio_Player::ready_blocks() {
    return i2s_player.available();
}

bool Audio_Player::open_file() {
    if (_opened) return true;
    sd_manager.closeFile(&_file);
    _file = sd_manager.openFile(_name, false);
    _opened = _file.isOpen();
    return _opened;
}

WAVConfigurationPacket Audio_Player::make_wav_config() {
    WAVConfigurationPacket wav_packet;
    wav_packet.state = 0;
    wav_packet.size = _name.length();

    for (int i=0; i<_name.length(); i++) {
        wav_packet.name[i] = _name[i];
    }
    return wav_packet;
}

void Audio_Player::_check_ready() {
    // This function avoids starting player during interrupts
    if (!_ready) return;

    if (_last_frequency) {
        start_tone(_last_frequency);
    } else {
        start();
    }
    _ready = false;
}

void Audio_Player::set_ready() {
    _last_frequency = 0;
    _ready = true;
}

void Audio_Player::set_ready(int frequency) {
    _last_frequency = frequency;
    _ready = true;
}

void Audio_Player::config_callback(SensorConfigurationPacket *config) {
    // Check for PLAYER ID
    if (config->sensorId != PLAYER) return;

    // Get tone frequency
    int tone = int(config->sampleRate);

    if (tone == 2) {
        if (_paused) return;
        _paused = true;
        audio_player.pause();
        return;
    } else if (tone == 3) {
        if (!_paused) return;
        _paused = false;
        audio_player.play();
        return;
    }
    _paused = false;

    audio_player.end();

    // End playback if sample tone is 0
    if (tone == 0) {
        return;
    }

    // tone <= max_file > => Play file else play tone
    if (tone == 1) {
        i2s_player.set_mode_file(true);
        audio_player.set_ready();
        //audio_player.start(); // Started in ready
    }
    else {
        i2s_player.set_mode_file(false);
        audio_player.set_ready(tone);
        //audio_player.start_tone(tone); // Started in ready
    }
}

void Audio_Player::ble_configuration(WAVConfigurationPacket &configuration) {
    int state = configuration.state;

    if (state == 2) {
        if (_paused) return;
        if (!_stream) return;
        _paused = true;
        audio_player.pause();
        return;
    } else if (state == 3) {
        if (!_paused) return;
        if (!_stream) return;
        _paused = false;
        audio_player.play();
        return;
    }

    _paused = false;
    end();

    if (configuration.size) {
        String name = String(configuration.name, configuration.size);

        set_name(name);
        i2s_player.set_mode_file(true);
    }

    if (configuration.state == 1) {
        set_ready();
        // start();
    }
}

Audio_Player audio_player;
