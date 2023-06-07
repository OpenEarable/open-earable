#ifndef OPEN_EARABLE_FILEWRITER_H
#define OPEN_EARABLE_FILEWRITER_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Arduino.h"

#include "SdFat.h"

#define SPI_SPEED SD_SCK_MHZ(50)
#define SPI_CC 27

class FileWriter {
public:
    FileWriter();
    ~FileWriter();
    bool begin();
    void end();

    void cleanFile();
    void setName(String name);

    void openFile();
    void closeFile();

    bool write_block(uint8_t *block, int size);
    void contained_write_block(uint8_t *block, int size);
private:
    SdExFat * sd;
    ExFatFile file;

    bool _is_open = false;

    String _name = "Logging.csv";
};


#endif //OPEN_EARABLE_FILEWRITER_H
