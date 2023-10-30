#include "I2S_Player.h"
#include "Arduino.h"
#include "Audio_Player.h"

void i2s_irq_handler(void);

I2S_Player::I2S_Player(bool use_eq) {
    this->use_eq = use_eq;

    eq = new Equalizer();
    stream = new BufferedInputStream();
}

I2S_Player::~I2S_Player() {
    delete eq;
    delete stream;
}

void I2S_Player::setBlockBufferSizes(int blockSize, int blockCount) {
    stream->buffer.setSizes(blockSize, blockCount);
}

/**
 * TODO: check at and of FILE
*/
bool I2S_Player::consume() {
    stream->consume(true);
    return stream->available();
}

bool I2S_Player::begin() {
    if (_available) return true;
    if (!sample_rate) return false;

    //initializing i2s pins
    nrf_i2s_pins_set(NRF_I2S,
                     digitalPinToPinName(_sckPin),
                     digitalPinToPinName(_lrckPin),
                     NC,
                     digitalPinToPinName(_sdoutPin),
                     NC);

    //if (!play_mode_file) mode = &const_freq;


    /* I2S MASTER CONFIGURATION */
    bool _i2s_config_status = nrf_i2s_configure(NRF_I2S,                            //I2S instance
                                          NRF_I2S_MODE_MASTER,                //I2S configured as master
                                          NRF_I2S_FORMAT_I2S,                 //I2S frame format
                                          NRF_I2S_ALIGN_LEFT,                 //Alignment of sample within a frame
                                          NRF_I2S_SWIDTH_16BIT,               //Sample width
                                          NRF_I2S_CHANNELS_LEFT,            //Enabled channels
                                          clock.nrf_i2s_mck,                //Master clock generator setup
                                          clock.nrf_i2s_ratio                  //MCK/LRCK ratio
    );

    //setting up the I2S transfer
    nrf_i2s_transfer_set(NRF_I2S, stream->buffer.getBlockSize()/WORD_SIZE, NULL, (uint32_t const *)stream->buffer.getReadPointer());

    if (use_eq) eq->update((int16_t *)stream->buffer.getReadPointer(), stream->buffer.getBlockSize() / sizeof(int16_t));

    //enable i2s peripheral
    nrf_i2s_enable(NRF_I2S);

    //enable i2s tx event interrupt
    nrf_i2s_int_enable(NRF_I2S, NRF_I2S_INT_TXPTRUPD_MASK);

    //assigning i2s_irq_handler as i2s interrupt handler
    __NVIC_SetVector(I2S_IRQn, (uint32_t)i2s_irq_handler);

    //setting I2S interrupt priority
    NRFX_IRQ_PRIORITY_SET(I2S_IRQn, NRFX_I2S_CONFIG_IRQ_PRIORITY);

    //enabling I2S interrupt in NVIC
    NRFX_IRQ_ENABLE(I2S_IRQn);

    _available = _i2s_config_status;

    return _available;
}

void I2S_Player::end() {
    NRFX_IRQ_DISABLE(I2S_IRQn);
    nrf_i2s_int_disable(NRF_I2S, NRF_I2S_INT_TXPTRUPD_MASK);
    nrf_i2s_disable(NRF_I2S);

    _running = false;
    _available = false;

    stream->buffer.clear();
}

void I2S_Player::start() {
    if (!_available || _running) return;
    nrf_i2s_tx_buffer_set(NRF_I2S, (uint32_t const *)stream->buffer.getReadPointer());
    if (use_eq) eq->update((int16_t *)stream->buffer.getReadPointer(), stream->buffer.getBlockSize() / sizeof(int16_t));

    nrf_i2s_task_trigger(NRF_I2S, NRF_I2S_TASK_START);
    _running = true;
}

void I2S_Player::stop() {
    if (!_available || !_running) return;
    nrf_i2s_task_trigger(NRF_I2S, NRF_I2S_TASK_STOP);
    _running = false;
}

bool I2S_Player::available() {
    return _available;
}

void I2S_Player::i2s_interrupt() {
    //Checking TXPTRUPD event
    if (nrf_i2s_event_check(NRF_I2S, NRF_I2S_EVENT_TXPTRUPD)) {
        //clear TXPTRUPD event
        nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_TXPTRUPD);

        bool stream_available = consume();

        if (stream->remaining() > 0) {
            nrf_i2s_tx_buffer_set(NRF_I2S, (uint32_t const *)stream->buffer.getReadPointer());
            if (use_eq) eq->update((int16_t *)stream->buffer.getReadPointer(), stream->buffer.getBlockSize() / sizeof(int16_t));
        } else if (!stream_available) {
            stop();
            audio_player.completed();
        }
    }
}

CircularBlockBuffer *I2S_Player::get_buffer() {
    return &(stream->buffer);
}

void I2S_Player::reset_buffer() {
    // reset the buffer
    stream->buffer.reset();
    eq->reset();
}

void i2s_irq_handler(void) {
    i2s_player.i2s_interrupt();
}

bool I2S_Player::is_running() {
    return _running;
}

int I2S_Player::setSampleRate(int _sampleRate) {
    sample_rate = _sampleRate;
    switch (_sampleRate)
    {
    case 16000: case (int)1e6/63:
        clock = {NRF_I2S_MCK_32MDIV63, NRF_I2S_RATIO_32X};
        break;
    case 32000: case (int)1e6/31:
        clock = {NRF_I2S_MCK_32MDIV31, NRF_I2S_RATIO_32X};
        break;
    case 41667:
        clock = {NRF_I2S_MCK_32MDIV3, NRF_I2S_RATIO_256X};
        break;
    case 44100: case (int)1e6/23:
        clock = {NRF_I2S_MCK_32MDIV23, NRF_I2S_RATIO_32X};
        //clock = {NRF_I2S_MCK_32MDIV15, NRF_I2S_RATIO_48X};
        break;
    case 50000:
        clock = {NRF_I2S_MCK_32MDIV10, NRF_I2S_RATIO_64X};
        break;
    case (int)1e6/18: //55555
        //clock = {NRF_I2S_MCK_32MDIV10, NRF_I2S_RATIO_64X};
        clock = {NRF_I2S_MCK_32MDIV6, NRF_I2S_RATIO_96X};
        break;
    // not working for some reason
    /*case 62500:
        //clock = {NRF_I2S_MCK_32MDIV10, NRF_I2S_RATIO_64X};
        clock = {NRF_I2S_MCK_32MDIV4, NRF_I2S_RATIO_128X};
        break;*/
    case (int)1e6/15:
        clock = {NRF_I2S_MCK_32MDIV5, NRF_I2S_RATIO_96X};
        break;
    case (int)1e6/12:
        clock = {NRF_I2S_MCK_32MDIV3, NRF_I2S_RATIO_128X};
        break;
    default:
        Serial.print("Error: Unsupported I2S sampling rate: ");
        Serial.println(_sampleRate);
        sample_rate = 0;
        break;
    }

    return sample_rate;
}

int I2S_Player::getSampleRate() {
    return sample_rate;
}

I2S_Player i2s_player(false);