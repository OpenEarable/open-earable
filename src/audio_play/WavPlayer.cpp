#include "WavPlayer.h"

WavPlayer::WavPlayer(String name) {
    this->_name = name;
}

WavPlayer::~WavPlayer() {
    //delete _file;
    end();
}

bool WavPlayer::begin() {
    if (!stream || !(*stream)) return false;
    
    if (_available) return true;
    _available = sd_setup();

    if (!_available) return false;

    _available = check_format();

    if (!_available) return false;

    preload_buffer();

    (*stream)->open();
    _available = (*stream)->available();
    return _available;
}

void WavPlayer::end() {
    sd_manager.closeFile(&_file);
    (*stream)->close();
}

bool WavPlayer::available() {
    return _available;
}

void WavPlayer::setStream(BufferedStream ** stream) {
    if (_available) end();
    this->stream = stream;
}

int WavPlayer::provide(int n) {
    if (!_available) return 0;

    int blocks = (*stream)->get_contiguous_blocks();
    int cont = min(blocks, n);

    if (!cont) {
        (*stream)->close();
        return 0;
    }

    unsigned int read = sd_to_buffer(cont);

    if (read < cont) {
        (*stream)->close();
    }
    return cont;
}

bool WavPlayer::sd_setup() {
    if(!sd_manager.begin()) return false;
    return open_file();
}

bool WavPlayer::check_format() {
    if(!sd_manager.begin()) return false;
    if (!_file.isOpen() || !open_file()) return false;
    
    // read info
    _cur_read_sd = sizeof(info);
    sd_manager.read_block(&_file, (uint8_t*)&info, sizeof(info));

    for (int i = 0; i < 4; i++) {
        if (info.chunkID[i] != PLAYER_FORMAT.chunkID[i]
        || info.format[i] != PLAYER_FORMAT.format[i]
        || info.subchunk1ID[i] != PLAYER_FORMAT.subchunk1ID[i]) {
            Serial.println("Error: The Audio Player requires the file format to be WAV!");
            return false;
        }
    }

    if (info.numChannels != PLAYER_FORMAT.numChannels) {
        Serial.print("Error: The Audio Player requires the number of channels to be ");
        Serial.print(PLAYER_FORMAT.numChannels);
        Serial.println("!");
        return false;
    }

    if (info.sampleRate != PLAYER_FORMAT.sampleRate) {
        Serial.print("Error: The Audio Player requires the sample rate to be ");
        Serial.print(PLAYER_FORMAT.sampleRate);
        Serial.println("!");
        return false;
    }

    if (info.formatTag != PLAYER_FORMAT.formatTag) {
        Serial.print("Error: The Audio Player requires the format tag to be ");
        Serial.print(PLAYER_FORMAT.formatTag);
        Serial.println(" (PCM)!");
        return false;
    }

    return true;
}

void WavPlayer::preload_buffer() {
    int cont = provide(_preload_blocks);

    Serial.print("preload blocks: ");
    Serial.println(cont);
}

unsigned int WavPlayer::sd_to_buffer(int multi) {
    uint8_t * ptr = (*stream)->buffer.getWritePointer();

    unsigned int read_total = 0;
    unsigned int read = sd_manager.read_block(&_file, ptr, audio_b_size * multi);

    _cur_read_sd += read;
    read_total += read;

    const int blocks_read = read / (*stream)->buffer.getBlockSize();

    if (read != 0) {
        (*stream)->provide(blocks_read);
    }

    return blocks_read;
}

float WavPlayer::getSampleRate() {
    if (!_available) return -1;
    return info.sampleRate;
}

unsigned int WavPlayer::get_size() {
    if (!open_file()) return 0;
    return _file.fileSize();
}

bool WavPlayer::open_file() {
    if (_opened) return true;
    sd_manager.closeFile(&_file);
    _file = sd_manager.openFile(_name, false);
    _opened = _file.isOpen();
    return _opened;
}

WAVConfigurationPacket WavPlayer::get_config() {
    WAVConfigurationPacket wav_packet;

    wav_packet.size = _name.length();
    memcpy(wav_packet.name, _name.c_str(), _name.length());

    /*for (int i=0; i<_name.length(); i++) {
        wav_packet.name[i] = _name[i];
    }*/
    return wav_packet;
}