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

    ExFatFile openFile(const String& name, bool write=true);
    void closeFile(ExFatFile * file);

    void remove(const String& name);

    void mkdir(const String& name);

    bool exists(const String& name);

    unsigned int write_block_at(ExFatFile * file, int offset, uint8_t *block, int size);
    unsigned int write_block(ExFatFile * file, uint8_t *block, int size);

    unsigned int read_block_at(ExFatFile *file, int offset, uint8_t *block, int size);
    unsigned int read_block(ExFatFile *file, uint8_t *block, int size);

    SdExFat * sd;
private:
    bool _available = false;

    ExFatFile * _lastFile;

    bool sd_inserted = false;

    static void _read_sd_state();
};

extern SDManager sd_manager;

#endif //OPEN_EARABLE_SDMANAGER_H
