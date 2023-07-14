#ifndef OPEN_EARABLE_FILEWRITER_H
#define OPEN_EARABLE_FILEWRITER_H

#include "Arduino.h"

#include "utils/SDManager.h"

class FileWriter {
public:
    FileWriter();
    ~FileWriter();
    bool begin();
    void end();

    bool openFile();
    void closeFile();

    void cleanFile();
    void setName(String name);

    bool isOpen();

    unsigned int get_size();

    unsigned int write_block_at(unsigned int offset, uint8_t *block, int size);
    unsigned int write_block(uint8_t *block, int size);

    unsigned int readBlock_at(unsigned int offset, uint8_t *block, int size);
    unsigned int read_block(uint8_t *block, int size);
private:
    ExFatFile file;

    String _name = "file.txt";
};


#endif //OPEN_EARABLE_FILEWRITER_H
