#ifndef OPEN_EARABLE_SDMANAGER_H
#define OPEN_EARABLE_SDMANAGER_H

#include "SdFat.h"

#define SPI_SPEED SD_SCK_MHZ(50)
#define SPI_CC 27

class SDManager {
public:
    SDManager();
    ~SDManager();

    bool begin();
    void end();

    ExFatFile openFile(const String& name);
    void closeFile(ExFatFile * file);

    void remove(const String& name);

    unsigned int write_block_at(ExFatFile * file, int offset, uint8_t *block, int size);
    unsigned int write_block(ExFatFile * file, uint8_t *block, int size);

    unsigned int read_block_at(ExFatFile *file, int offset, uint8_t *block, int size);
    unsigned int read_block(ExFatFile *file, uint8_t *block, int size);

private:
    bool ready = false;

    int usage_count = 0;

    SdExFat * sd;
    ExFatFile * _lastFile;
};

extern SDManager sd_manager;

#endif //OPEN_EARABLE_SDMANAGER_H
