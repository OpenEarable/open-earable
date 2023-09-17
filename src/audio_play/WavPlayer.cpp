#include "WavPlayer.h"

WavPlayer::WavPlayer(String name) {
    //_file = *file;
    //this->stream = stream;
    this->_name = name;
}

WavPlayer::~WavPlayer() {
    //delete _file;
    end();
}

void WavPlayer::begin() {
    if (_available || !stream || !(*stream)) return;
    _available = sd_setup();

    if (!_available) return;

    _cur_read_sd = _default_offset;
    _file.seekSet(_default_offset);

    preload_buffer();

    (*stream)->open();
    _available = (*stream)->available();
}

void WavPlayer::end() {
    sd_manager.closeFile(&_file);
    (*stream)->close();
}

bool WavPlayer::available() {
    return _available;
}

bool WavPlayer::setStream(BufferedStream ** stream) {
    if (_available) {
        end();
    }
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

/*void WavPlayer::set_name(String name) {
    _name = std::move(name);
    _opened = false;
}*/

void WavPlayer::preload_buffer() {
    int cont = provide(_preload_blocks);

    Serial.print("preload blocks: ");
    Serial.println(cont);
}

unsigned int WavPlayer::sd_to_buffer(int multi) {
    uint8_t * ptr = (*stream)->buffer.getCurWritePointer();

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
    if (!open_file()) return 0;
    uint64_t pos = _file.curPosition();
    unsigned int rate;
    sd_manager.read_block_at(&_file, 24, (uint8_t *) &rate, 4);
    _file.seekSet(pos);
    return rate;
}

unsigned int WavPlayer::get_size() {
    if (!open_file()) return 0;
    // uint64_t pos = _file.curPosition(); // old
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

    for (int i=0; i<_name.length(); i++) {
        wav_packet.name[i] = _name[i];
    }
    return wav_packet;
}