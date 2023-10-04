#include "Audio_Player.h"
#include "WavPlayer.h"
#include "JinglePlayer.h"

#include <utility>

uint8_t AUDIO_BUFFER[audio_b_size * audio_b_count] __attribute__((aligned (16)));

//bool Audio_Player::_paused = false;

Audio_Player::Audio_Player() {
    //source = new WavPlayer(&(i2s_player.stream));
}

Audio_Player::~Audio_Player() {
    //delete[] source;
}

bool Audio_Player::available() {
    return _available;
}

bool Audio_Player::begin() {
    if (_available) return true;
    i2s_player.setBuffer(AUDIO_BUFFER, audio_b_size, audio_b_count);
    if (!source) return false;
    source->begin();
    if (!source->available()) return false;
    i2s_player.begin();
    _available = i2s_player.available();
    if (!_available) source->end();
    return _available;
}

void Audio_Player::setSource(AudioSource * source) {
    this->source = source;
    if (source) source->setStream(&(i2s_player.stream));
}

void Audio_Player::play() {
    if (!source || _running) return;

    if (!_available) begin();

    if (!source->available()) {
        i2s_player.reset_buffer();

        source->begin();
        if (!source->available()) return;
    }

    _running = true;

    i2s_player.start();
}

void Audio_Player::end() {
    if (!_available) return;

    stop();
    i2s_player.end();

    _available = false;
}

void Audio_Player::pause() {
    if (!_available || !_running) return;
    _running = false;
    i2s_player.stop();
}

void Audio_Player::stop() {
    if (!_available || !_running) return;
    _running = false;
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

void Audio_Player::ble_configuration(WAVConfigurationPacket &configuration) {
    Tone * tone;
    switch(configuration.state) {
    case 1:
        if (configuration.size) {
            end();

            switch (configuration.mode) {
            case 0:
                setSource(NULL);
                return;
            case 1:
                setSource(new WavPlayer(String(configuration.name, configuration.size)));
                break;
            case 2:
                tone = (Tone*)(configuration.name);
                setSource(new ToneGenerator(tone->frequency,tone->amplitude,(Waveform)(configuration.size-1)));
                break;
            case 3:
                setSource(JinglePlayer::getJingle((Jingle) (configuration.size - 1)));
                break;
            }
        }
        play();
        break;
    case 2:
        pause();
        break;
    case 3:
        stop();
        break;
    }
}

WAVConfigurationPacket Audio_Player::make_wav_config() {
    return source ? source->get_config() : WAVConfigurationPacket();
}

Audio_Player audio_player;