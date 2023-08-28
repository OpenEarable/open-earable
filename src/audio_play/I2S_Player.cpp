#include "I2S_Player.h"

void i2s_irq_handler(void);

sampling_mode file_mode = {NRF_I2S_MCK_32MDIV23, NRF_I2S_RATIO_32X};
sampling_mode const_freq = {NRF_I2S_MCK_32MDIV3, NRF_I2S_RATIO_256X};


I2S_Player::I2S_Player() {

}

I2S_Player::~I2S_Player() {

}

void I2S_Player::setBlockBufferSizes(int blockSize, int blockCount) {
    _blockBuffer.setSizes(blockSize, blockCount);
}

void I2S_Player::start() {
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
    _blockBuffer.incrementReadPointer(); // Requires buffer to be filled with at least 2 blocks

    //setting up the I2S transfer
    nrf_i2s_transfer_set(NRF_I2S, _blockBuffer.getBlockSize()/WORD_SIZE, NULL, (uint32_t const *)_blockBuffer.getReadPointer());

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

    _end_flag = true;
    _turn_off_flag = true;

    clear_buffer();
}

void I2S_Player::set_mode_file(bool play_file) {
    play_mode_file = play_file;
}

bool I2S_Player::get_mode_file() {
    return play_mode_file;
}

void I2S_Player::play() {
    _end_flag = false;
    _turn_off_flag = false;
    nrf_i2s_task_trigger(NRF_I2S, NRF_I2S_TASK_START);
}

void I2S_Player::stop() {
    nrf_i2s_task_trigger(NRF_I2S, NRF_I2S_TASK_STOP);
    _end_flag = true;
}

void I2S_Player::pause() {
    nrf_i2s_task_trigger(NRF_I2S, NRF_I2S_TASK_STOP);
}

void I2S_Player::completed() {
    _completed_flag = true;
    _turn_off_flag = true;
}

int I2S_Player::available() {
    //return !_blockBuffer.check_collision_r_next();
    return _blockBuffer.available_write();
}

uint8_t *I2S_Player::getWritePointer() {
    return _blockBuffer.getCurWritePointer(); // Use cur block
}

void I2S_Player::incrementWritePointer() {
    _blockBuffer.incrementWritePointer();
}

void I2S_Player::clear_buffer() {
    _blockBuffer.clear();
}

bool I2S_Player::check_config_status() {
    return _i2s_config_status;
}

void I2S_Player::i2s_interrupt() {
//Checking TXPTRUPD event
    if (nrf_i2s_event_check(NRF_I2S, NRF_I2S_EVENT_TXPTRUPD))
    {
        //clear TXPTRUPD event
        nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_TXPTRUPD);


        if (_end_flag) {
            return;
        }

        if (_blockBuffer.available_read()) {
            nrf_i2s_tx_buffer_set(NRF_I2S, (uint32_t const *)_blockBuffer.getReadPointer());
            _blockBuffer.incrementReadPointer();
        } else if (_turn_off_flag) {
            stop();
        }
    }
}

bool I2S_Player::get_turn_off() {
    return _turn_off_flag;
}

bool I2S_Player::get_end() {
    return _end_flag;
}

bool I2S_Player::get_completed() {
    return _completed_flag;
}

void I2S_Player::setBuffer(uint8_t *buffer, int blockSize, int blockCount) {
    _blockBuffer.set_buffer(buffer, blockSize, blockCount);
}

CircularBlockBuffer *I2S_Player::get_buffer() {
    return &_blockBuffer;
}

int I2S_Player::get_contiguous_blocks() const {
    return _blockBuffer.get_contiguous_write_blocks_cur(); // Use cur block
}

void I2S_Player::reset_buffer() {
    // reset the buffer
    _blockBuffer.reset();
}

void i2s_irq_handler(void) {
    i2s_player.i2s_interrupt();
}

I2S_Player i2s_player;