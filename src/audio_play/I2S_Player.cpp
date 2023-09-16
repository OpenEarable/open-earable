#include "I2S_Player.h"
#include "Arduino.h"

void i2s_irq_handler(void);

sampling_mode file_mode = {NRF_I2S_MCK_32MDIV23, NRF_I2S_RATIO_32X};
sampling_mode const_freq = {NRF_I2S_MCK_32MDIV11, NRF_I2S_RATIO_64X};


I2S_Player::I2S_Player() {
    eq = new Equalizer();
    stream = new BufferedInputStream();
}

I2S_Player::~I2S_Player() {

}

void I2S_Player::setBlockBufferSizes(int blockSize, int blockCount) {
    stream->buffer.setSizes(blockSize, blockCount);
}

/**
 * TODO: check at and of FILE
*/
bool I2S_Player::consume(int n) {
    stream->consume(true);
    return !stream->closed();
}

void I2S_Player::begin() {
    //initializing i2s pins
    nrf_i2s_pins_set(NRF_I2S,
                     digitalPinToPinName(_sckPin),
                     digitalPinToPinName(_lrckPin),
                     NC,
                     digitalPinToPinName(_sdoutPin),
                     NC);

    sampling_mode * mode = &file_mode;
    if (!play_mode_file) mode = &const_freq;


    /* I2S MASTER CONFIGURATION */
    _i2s_config_status = nrf_i2s_configure(NRF_I2S,                            //I2S instance
                                          NRF_I2S_MODE_MASTER,                //I2S configured as master
                                          NRF_I2S_FORMAT_I2S,                 //I2S frame format
                                          NRF_I2S_ALIGN_LEFT,                 //Alignment of sample within a frame
                                          NRF_I2S_SWIDTH_16BIT,               //Sample width
                                          NRF_I2S_CHANNELS_LEFT,            //Enabled channels
                                          mode->nrf_i2s_mck,                //Master clock generator setup
                                          mode->nrf_i2s_ratio                  //MCK/LRCK ratio
    );

    // Original NRF_I2S_MCK_32MDIV3 and NRF_I2S_RATIO_256X  41667Hz
    // Alternate NRF_I2S_MCK_32MDIV23 and NRF_I2S_RATIO_32X

    // Do buffer reset externally!

    // Ignore first block in buffer
    //stream->consume(true);
    consume(1);
    //stream->buffer.incrementReadPointer(); // Requires buffer to be filled with at least 2 blocks

    //setting up the I2S transfer
    nrf_i2s_transfer_set(NRF_I2S, stream->buffer.getBlockSize()/WORD_SIZE, NULL, (uint32_t const *)stream->buffer.getReadPointer());

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
}

void I2S_Player::end() {
    NRFX_IRQ_DISABLE(I2S_IRQn);
    nrf_i2s_int_disable(NRF_I2S, NRF_I2S_INT_TXPTRUPD_MASK);
    nrf_i2s_disable(NRF_I2S);

    //_end_flag = true;
    _turn_off_flag = true;

    stream->buffer.clear();
    Serial.println("end");
}

void I2S_Player::start() {
    _turn_off_flag = false;
    nrf_i2s_task_trigger(NRF_I2S, NRF_I2S_TASK_START);
    Serial.println("start");
}

void I2S_Player::stop() {
    nrf_i2s_task_trigger(NRF_I2S, NRF_I2S_TASK_STOP);
    //_end_flag = true;
    Serial.println("stop");
}

void I2S_Player::set_mode_file(bool play_file) {
    play_mode_file = play_file;
}

bool I2S_Player::get_mode_file() {
    return play_mode_file;
}

/*void I2S_Player::play() {
    //_end_flag = false;
    _turn_off_flag = false;
    nrf_i2s_task_trigger(NRF_I2S, NRF_I2S_TASK_START);
}

void I2S_Player::stop() {
    nrf_i2s_task_trigger(NRF_I2S, NRF_I2S_TASK_STOP);
    //_end_flag = true;
}

void I2S_Player::pause() {
    nrf_i2s_task_trigger(NRF_I2S, NRF_I2S_TASK_STOP);
}

void I2S_Player::completed() {
    _completed_flag = true;
    _turn_off_flag = true;
}*/

/*int I2S_Player::available() {
    //return !stream->buffer.check_collision_r_next();
    return stream->buffer.available_write();
}

int I2S_Player::remaining() {
    return stream->buffer.available_read();
}

uint8_t *I2S_Player::getWritePointer() {
    return stream->buffer.getCurWritePointer(); // Use cur block
}

void I2S_Player::incrementWritePointer() {
    stream->buffer.incrementWritePointer();
}

void I2S_Player::clear_buffer() {
    stream->buffer.clear();
}*/

bool I2S_Player::check_config_status() {
    return _i2s_config_status;
}

void I2S_Player::i2s_interrupt() {
//Checking TXPTRUPD event
    if (nrf_i2s_event_check(NRF_I2S, NRF_I2S_EVENT_TXPTRUPD))
    {
        //clear TXPTRUPD event
        nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_TXPTRUPD);

        //stream->consume(true);
        bool stream_running = consume(1);

        /*
        _end_flag not needed, provider.end() will be true
        */

        /*if (_end_flag) {
            return;
        }*/

        if (stream->remaining()) {
            nrf_i2s_tx_buffer_set(NRF_I2S, (uint32_t const *)stream->buffer.getReadPointer());
            //stream->buffer.incrementReadPointer();
            eq->update((int16_t *)stream->buffer.getReadPointer(), stream->buffer.getBlockSize() / sizeof(int16_t));
        } else if (!stream_running) {
            stop();
        }
    }
}

/*bool I2S_Player::get_turn_off() {
    return _turn_off_flag;
}

bool I2S_Player::get_end() {
    return _end_flag;
}

bool I2S_Player::get_completed() {
    return _completed_flag;
}*/

void I2S_Player::setBuffer(uint8_t *buffer, int blockSize, int blockCount) {
    stream->buffer.set_buffer(buffer, blockSize, blockCount);
}

CircularBlockBuffer *I2S_Player::get_buffer() {
    return &(stream->buffer);
}

/*int I2S_Player::get_contiguous_blocks() const {
    return stream->buffer.get_contiguous_write_blocks(); // Use cur block
}*/

void I2S_Player::reset_buffer() {
    // reset the buffer
    stream->buffer.reset();
    eq->reset();
}

void i2s_irq_handler(void) {
    i2s_player.i2s_interrupt();
}

I2S_Player i2s_player;