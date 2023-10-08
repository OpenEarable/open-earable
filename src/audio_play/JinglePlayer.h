#ifndef JINGLE_PLAYER_H
#define JINGLE_PLAYER_H

#include "AudioSource.h"
#include "Play_Service.h"

enum Jingle {
    NOTIFICATION,
    SUCCESS,
    ERROR,
    ALARM,
    PING,
    OPEN,
    CLOSE,
    CLICK
};

class JinglePlayer : public AudioSource {
public:
    JinglePlayer(const uint16_t * jingle, int length);
    ~JinglePlayer();

    static JinglePlayer * getJingle(Jingle jingle);

    int provide(int n) override;
    bool available() override;
    bool begin() override;
    void end() override;
    void setStream(BufferedStream ** stream) override;

    WAVConfigurationPacket get_config() override;

    unsigned int get_sample_rate();
    unsigned int get_size();
private:
    bool _available = false;

    const int _preload_blocks = 6; // 12

    void preload_buffer();

    const uint16_t * jingle;

    int index = 0;
    const int length;
};

#endif