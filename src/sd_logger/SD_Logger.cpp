#include "SD_Logger.h"

#include <utility>

ExFatFile SD_Logger::_file;
bool SD_Logger::_opened = false;
char SD_Logger::_buffer[LOGGER_BUFFER_SIZE];
int SD_Logger::_index = 0;
String SD_Logger::_name = "Log.csv";

bool SD_Logger::begin() {
    _index = 0;
    if(!sd_manager.begin()) return false;
    sd_manager.remove(_name);
    if (!open_file()) return false;
    write_header();
    return _file.isOpen();
}

void SD_Logger::end() {
    sd_manager.end();
}

void SD_Logger::set_name(String name) {
    _name = std::move(name);
    _opened = false;
}

void SD_Logger::data_callback(int id, unsigned int timestamp, const String & data_string) {
    if (id == -1) {
        dump_to_sd();
        _file.close();
        _opened = false;
        return;
    };

    String text = String(id);
    text += ", " + String(timestamp);
    text += ", " + data_string;
    text += "\r\n";

    if (text.length() + _index > LOGGER_BUFFER_SIZE) {
        dump_to_sd();
    }

    text.toCharArray(&(_buffer[_index]), text.length());
    _index += text.length() - 1; // -1 to remove null terminator
}

void SD_Logger::dump_to_sd() {
    if (!open_file()) return;
    if (_index == 0) return;
    sd_manager.write_block(&_file, (uint8_t*)_buffer, _index);
    memset(_buffer, 0, LOGGER_BUFFER_SIZE);
    _index = 0;
}

void SD_Logger::write_header() {
    _index = 0;
    String header = "ID, TIMESTAMP, Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data8, Data9\n\r";
    header.toCharArray(&(_buffer[_index]), header.length());
    _index += header.length() - 1; // -1 to remove null terminator
    dump_to_sd();
}

bool SD_Logger::open_file() {
    if (_opened) return true;
    _file = sd_manager.openFile(_name, true);
    _opened = _file.isOpen();
    return _opened;
}






