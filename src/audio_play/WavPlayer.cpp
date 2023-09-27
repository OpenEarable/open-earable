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

    // read info
    _cur_read_sd = sizeof(info);
    sd_manager.read_block(&_file, (uint8_t*)&info, sizeof(info));

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

    if (!cont) return 0;

    unsigned int read = sd_to_buffer(cont);

    if (read < cont) {
        (*stream)->close();
    }
    return cont;
}

bool WavPlayer::sd_setup() {
    if(!sd_manager.begin()) return false;
    open_file();
    return _file.isOpen();
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

    if (read != 0) {
        (*stream)->provide(read / audio_b_size);
    }

    return read_total / audio_b_size;
}

unsigned int WavPlayer::get_sample_rate() {
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

    wav_packet.state = 0;
    wav_packet.size = _name.length();

    memcpy(wav_packet.name, _name.c_str(), _name.length());

    /*for (int i=0; i<_name.length(); i++) {
        wav_packet.name[i] = _name[i];
    }*/
    return wav_packet;
}