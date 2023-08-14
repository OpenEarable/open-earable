#include "SD_Logger.h"

#include <utility>

FileWriter * SD_Logger::writer;
char SD_Logger::_buffer[LOGGER_BUFFER_SIZE];
int SD_Logger::_index = 0;
String SD_Logger::_name = "Log.csv";

bool SD_Logger::begin() {
    writer = new FileWriter();
    _index = 0;
    bool status = writer->begin();
    set_name(_name);
    writer->cleanFile();
    write_header();
    return status;
}

void SD_Logger::end() {
    writer->end();
    delete writer;
}

void SD_Logger::set_name(String name) {
    writer->setName(std::move(name));
}

void SD_Logger::data_callback(int id, unsigned int timestamp, const String & data_string) {
    if (id == -1) return dump_to_sd();

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
    if (_index == 0) return;
    writer->write_block((uint8_t*)_buffer, _index);
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






