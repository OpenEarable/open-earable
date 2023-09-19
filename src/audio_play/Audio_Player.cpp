#include "Audio_Player.h"
//#include "WavPlayer.h"

#include <utility>

uint8_t AUDIO_BUFFER[audio_b_size * audio_b_count] __attribute__((aligned (16)));

bool Audio_Player::_paused = false;

Audio_Player::Audio_Player() {
    //source = new WavPlayer(&(i2s_player.stream));
}

Audio_Player::~Audio_Player() {
    delete[] source;
}

bool Audio_Player::begin() {
    i2s_player.setBuffer(AUDIO_BUFFER, audio_b_size, audio_b_count);
    if (!source) return false;
    source->begin();
    return source->available();
}

void Audio_Player::setSource(AudioSource * source) {
    this->source = source;
    source->setStream(&(i2s_player.stream));
}

void Audio_Player::start() {
    if (_running) {
        return;
    }

    if (!source->available()) {
        //i2s_player.clear_buffer();
        i2s_player.reset_buffer();

        source->begin();
        if (!source->available()) return;
    }

     _running = true;

    i2s_player.begin();
    play();
}

void Audio_Player::end() {
    if (!_running) {
        return;
    }
    _running = false;

    stop();
    i2s_player.end();
}

void Audio_Player::play() {
    i2s_player.start();
}

void Audio_Player::pause() {
    i2s_player.stop();
}

void Audio_Player::stop() {
    i2s_player.stop();
    i2s_player.end();
    source->end();
}

/*bool Audio_Player::check_completed() {
    if (!i2s_player.stream->available() && !i2s_player.is_running()) {
        end();
        return true;
    }
    return false;
}*/

void Audio_Player::_check_ready() {
    // This function avoids starting player during interrupts
    if (!_ready) return;
    start();
    _ready = false;
}

void Audio_Player::set_ready() {
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

    audio_player.set_ready();

    // tone <= max_file > => Play file else play tone
}

void Audio_Player::ble_configuration(WAVConfigurationPacket &configuration) {
    int state = configuration.state;

    if (state == 2) {
        if (_paused) return;
        if (!_running) return;
        _paused = true;
        audio_player.pause();
        return;
    } else if (state == 3) {
        if (!_paused) return;
        if (!_running) return;
        _paused = false;
        audio_player.play();
        return;
    }

    _paused = false;
    end();

    //TODO: move to WavPlayer

    /*if (configuration.size) {
        String name = String(configuration.name, configuration.size);

        set_name(name);
        i2s_player.set_mode_file(true);
    }*/

    if (configuration.state == 1) {
        set_ready();
        // start();
    }
}

WAVConfigurationPacket Audio_Player::make_wav_config() {
    return source->get_config();
}

Audio_Player audio_player;
