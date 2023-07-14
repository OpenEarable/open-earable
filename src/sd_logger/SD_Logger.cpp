#include "SD_Logger.h"

#include <utility>

FileWriter * SD_Logger::writer;
char SD_Logger::_buffer[LOGGER_BUFFER_SIZE];
int SD_Logger::_index = 0;

bool SD_Logger::begin() {
    writer = new FileWriter();
    _index = 0;
    bool status = writer->begin();
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

void SD_Logger::data_callback(int id, unsigned int timestamp, uint8_t * data, int size) {
    if (id == -1) return dump_to_sd();

    return;

    String text = String(id);

    text += ", " + String(timestamp);

    if (false) {
        text += convert_float((float*)data);
    } else if (false) {
        text += convert_int((int*)data);
    }
    text += "\r\n";

    if (text.length() + _index > LOGGER_BUFFER_SIZE) {
        dump_to_sd();
    }

    text.toCharArray(&(_buffer[_index]), text.length());
    _index += text.length() - 1; // -1 to remove null terminator
}

String SD_Logger::convert_float(float *data) {
    String text = "";
    int count = (int)data[0];
    for (int i=0; i<count; i++) {
        text +=  ", " + String(data[i+1]);
    }
    return text;
}

String SD_Logger::convert_int(int *data) {
    String text = "";
    int count = data[0];
    for (int i=0; i<count; i++) {
        text +=  ", " + String(data[i+1]);
    }
    return text;
}

void SD_Logger::dump_to_sd() {
    if (_index == 0) return;
    writer->contained_write_block((uint8_t*)_buffer, _index);
    memset(_buffer, 0, LOGGER_BUFFER_SIZE);
    _index = 0;
}

void SD_Logger::write_header() {
    _index = 0;
    String header = "ID, TIMESTAMP, Data1, Data2, Data3\n\r";
    header.toCharArray(&(_buffer[_index]), header.length());
    _index += header.length() - 1; // -1 to remove null terminator

    dump_to_sd();
}






