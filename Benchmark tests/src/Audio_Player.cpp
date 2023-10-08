#include "Audio_Player.h"

#include <utility>

uint8_t AUDIO_BUFFER[audio_b_size * audio_b_count] __attribute__((aligned (16)));

Audio_Player::Audio_Player() {

}

Audio_Player::~Audio_Player() {

}

bool Audio_Player::init() {
    i2s_setup();
    return sd_setup();
}

void Audio_Player::i2s_setup() {
    // i2s_player.setBlockBufferSizes(_blockSize, _blockCount);
    i2s_player.setBuffer(AUDIO_BUFFER, audio_b_size, audio_b_count);
}

bool Audio_Player::sd_setup() {
    _fileWriter = new FileWriter();
    _fileWriter->setName(_name);
    _fileWriter->setWriting(false);
    return _fileWriter->begin();
}

void Audio_Player::update() {
    if (!_stream) return;
    if (check_completed()) return;
    if (!i2s_player.available()) return;

    unsigned int read = sd_to_buffer();
    if (read == 0) {
        i2s_player.completed();
    }
}

int Audio_Player::update_contiguous(int max_cont) {
    if (!_stream) return 0;
    if (check_completed()) return 0;

    int cont = min(i2s_player.get_contiguous_blocks(), max_cont);

    if (!cont) return 0;

    unsigned int read = sd_to_buffer(cont);

    if (read == 0) {
        i2s_player.completed();
    }
    return cont;
}

void Audio_Player::start() {
    if (_stream) {
        return;
    }
    _stream = true;

    i2s_player.clear_buffer();

    _fileWriter->openFile();
    _cur_read_sd = _default_offset;

    i2s_player.start();

    preload_buffer();

    play();
}

void Audio_Player::end() {
    if (!_stream) {
        return;
    }
    _stream = false;
    stop();
    i2s_player.end();
}

void Audio_Player::set_name(String name) {
    _name = std::move(name);
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
    // Load 3 blocks
    for (int i = 0; i < _preload_blocks; ++i) {
        sd_to_buffer();
    }
    return;
    while (i2s_player.available()) {
        Serial.println(String(i2s_player.available()));
        sd_to_buffer();
    }
    sd_to_buffer();
    sd_to_buffer();
    sd_to_buffer();
    return;
    while (i2s_player.available()) {
        Serial.println(String(i2s_player.available()));
        sd_to_buffer();
    }
    return;
    sd_to_buffer();
    sd_to_buffer();
    sd_to_buffer();
    sd_to_buffer();
    return;

}

unsigned int Audio_Player::sd_to_buffer() {
    uint8_t * ptr = i2s_player.getWritePointer();

    unsigned int read = _fileWriter->read_block_at(_cur_read_sd, ptr, _blockSize);
    _cur_read_sd += read;

    if (read != 0) {
        i2s_player.incrementWritePointer();
    }
    return read;
}

unsigned int Audio_Player::sd_to_buffer(int multi) {
    uint8_t * ptr = i2s_player.getWritePointer();

    unsigned int read = _fileWriter->read_block_at(_cur_read_sd, ptr, _blockSize * multi);
    _cur_read_sd += read;

    if (read != 0) {
        for (int i = 0; i < multi; ++i) {
            i2s_player.incrementWritePointer();
        }
    }
    return read;
}

bool Audio_Player::check_completed() {
    if (i2s_player.get_completed() && i2s_player.get_end()) {
        end();
        return true;
    }
    return false;
}


unsigned int Audio_Player::get_sample_rate() {
    _fileWriter->openFile();
    if (!_fileWriter->isOpen()) return 0;
    unsigned int rate;
    _fileWriter->read_block_at(24, (uint8_t *) &rate, 4);
    return rate;
}

unsigned int Audio_Player::get_size() {
    _fileWriter->openFile();
    if (!_fileWriter->isOpen()) return 0;
    return _fileWriter->get_size();
}

void Audio_Player::pre_open_file() {
    _fileWriter->openFile();
}

int Audio_Player::ready_blocks() {
    return i2s_player.available();
}

/*
void Audio_Player::config_callback(SensorConfigurationPacket *config) {
    // Check for PLAYER ID

    return;
    //if (config->sensorId != PLAYER) return;

    // Get tone frequency
    int tone = int(config->sampleRate);

    audio_player.end();

    // End playback if sample tone is 0
    if (tone == 0) {
        return;
    }

    // tone == 1 => Play file
    if (tone == 1) {
        audio_player.start();
    } else {
        // Logic for tone
        // TO DO!
    }
}
*/

Audio_Player audio_player;
