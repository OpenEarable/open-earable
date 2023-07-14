#include "Audio_Player.h"

#include <utility>

Audio_Player::Audio_Player() {

}

Audio_Player::~Audio_Player() {

}

bool Audio_Player::init() {
    i2s_setup();
    return sd_setup();
}

void Audio_Player::i2s_setup() {
    i2s_player.setBlockBufferSizes(_blockSize, _blockCount);
}

bool Audio_Player::sd_setup() {
    _fileWriter = new FileWriter();
    _fileWriter->setName(_name);
    return _fileWriter->begin();
}

void Audio_Player::update() {
    if (i2s_player.get_turn_off()) return;
    if (!i2s_player.available()) return;

    unsigned int read = sd_to_buffer();
    if (read == 0) {
        i2s_player.completed();
    }
}

void Audio_Player::start() {
    if (_stream) {
        return;
    }
    _stream = true;

    i2s_player.clear_buffer();
    i2s_player.config();

    _fileWriter->openFile();
    _cur_read_sd = _default_offset;
    preload_buffer();

    play();
}

void Audio_Player::end() {
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
    // Load 2 blocks
    sd_to_buffer();
    sd_to_buffer();
}

unsigned int Audio_Player::sd_to_buffer() {
    uint8_t * ptr = i2s_player.getWritePointer();

    unsigned int read = _fileWriter->write_block_at(_cur_read_sd, ptr, _blockSize);
    _cur_read_sd += read;

    if (read != 0) {
        i2s_player.incrementWritePointer();
    }
    return read;
}

unsigned int Audio_Player::get_sample_rate() {
    _fileWriter->openFile();
    if (!_fileWriter->isOpen()) return 0;
    unsigned int rate;
    _fileWriter->readBlock_at(24, (uint8_t*)&rate, 4);
    return rate;
}

unsigned int Audio_Player::get_size() {
    _fileWriter->openFile();
    if (!_fileWriter->isOpen()) return 0;
    return _fileWriter->get_size();
}

Audio_Player audio_player;
