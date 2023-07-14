#ifndef OPEN_EARABLE_SD_LOGGER_H
#define OPEN_EARABLE_SD_LOGGER_H

#include "utils/FileWriter.h"
#include "EdgeML_Custom.h"

// #define LOGGER_BUFFER_SIZE 1024
#define LOGGER_BUFFER_SIZE 2048

class SD_Logger {
public:
    static bool begin();
    static void end();

    static void set_name(String name);

    static void data_callback(int, unsigned int, const String&);
private:
    static FileWriter * writer;
    static int _index;
    static char _buffer[LOGGER_BUFFER_SIZE];

    static String _name;

    static void dump_to_sd();
    static void write_header();
};

#endif //OPEN_EARABLE_SD_LOGGER_H
