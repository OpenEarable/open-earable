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
    if (!source || !device) return false;
    source->setStream(&(device->stream));
    device->setBuffer(AUDIO_BUFFER, audio_b_size, audio_b_count);
    source->begin();
    _sampleRate = source->getSampleRate();
    //Serial.print("sampleRate: ");
    //Serial.println(_sampleRate);
    if (!source->available()) return false;
    _sampleRate = device->setSampleRate(_sampleRate);
    device->begin();
    _available = device->available();
    if (!_available) source->end();
    return _available;
}

void Audio_Player::setSource(AudioSource * source) {
    if (_available) {
        Serial.println("The audio player needs to be ended to change the source.");
        return;
    }
    this->source = source;
    /*if (!source || !device) return;
    _sampleRate = source->getSampleRate();
    device->setSampleRate(_sampleRate);
    source->setStream(&(device->stream));*/
    play_service.writeSource(get_config());
}

void Audio_Player::setDevice(OutputDevice * device) {
    if (_available) {
        Serial.println("The audio player needs to be ended to change the device.");
        return;
    }
    this->device = device;
    /*if (!device || !source) return;
    _sampleRate = source->getSampleRate();
    _sampleRate = device->setSampleRate(_sampleRate);
    source->setStream(&(device->stream));*/
}

void Audio_Player::play() {
    if (!_available || !source || _running) return;

    if (!source->available()) {
        source->begin();
        if (!source->available()) return;
    }

    _running = true;

    device->start();

    play_service.writePlayerState(PLAY);
}

void Audio_Player::end() {
    if (!_available) return;

    stop();
    device->end();

    _available = false;
}

void Audio_Player::pause() {
    if (!_available || !_running) return;
    _running = false;
    device->stop();

    play_service.writePlayerState(PAUSE);
}

void Audio_Player::stop() {
    if (!_available) return;
    if (_running) device->stop();
    _running = false;
    
    source->end();
    //source->begin();

    play_service.writePlayerState(STOP);
}

/*bool Audio_Player::check_completed() {
    if (!i2s_player.stream->available() && !i2s_player.is_running()) {
        end();
        return true;
    }
    return false;
}*/

void Audio_Player::completed() {
    _running = false;
    source->end();
    play_service.writePlayerState(STOP);
}

void Audio_Player::ble_configuration(WAVConfigurationPacket &configuration) {
    Tone * tone;
    //if (!configuration.size) return;
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
        setSource(new ToneGenerator(tone->frequency,tone->amplitude,(Waveform)(configuration.size-1), 1e6/12));
        break;
    case 3:
        setSource(JinglePlayer::getJingle((Jingle) (configuration.size - 1)));
        break;
    }
    if (!_available) begin();
    play();
}

void Audio_Player::set_state(int state) {
    switch(state) {
    case PLAY:
        play();
        break;
    case PAUSE:
        pause();
        break;
    case STOP:
        stop();
        break;
    }
}

WAVConfigurationPacket Audio_Player::get_config() {
    return source ? source->get_config() : WAVConfigurationPacket();
}

Audio_Player audio_player;