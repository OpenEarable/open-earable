#include "Arduino.h"

//#include "ArduinoBLE.h"

#include "Memory.h"

#include "Audio_Player.h"
#include "PDM_MIC_Sensor.h"

String name = "play_file.wav";

int rate = 62500;

int _buffer_loops = 4;

// unsigned long duration = 3 * 60 * 1000;
unsigned long duration = 90 * 1000;
unsigned long start = 0;

unsigned long _cycle = 0;

unsigned long _edge_ml_last = 0;
unsigned long _overlap = 5;
unsigned long _overlap_prepopen = 4;

unsigned long _edge_ml_delay = (int)(1000.0/20);

unsigned long _buffer_interval_time = _edge_ml_delay - _overlap;
unsigned long _buffer_interval_time_prepopen = _edge_ml_delay - _overlap_prepopen;

int min_ready_pdm = 3;
int min_ready_play = 4;

int apx_buf_time_init = 14;
int apx_pdm = apx_buf_time_init;
int apx_play = apx_buf_time_init;


bool flag = false;

int counter = 0;

void update();
void update_lm();
bool update_pdm();
bool update_play();

bool check_overlap();
bool check_overlap_prepopen();

void setup() {
    Serial.begin(0);
    while (!Serial) {};

    Serial.println("READY");

    print_memory();
    print_preallocated();

    //BLE.begin();

    audio_player.set_name(name);
    audio_player.init();

    pdm_mic_sensor.setSampleRate(rate);
    pdm_mic_sensor.init();

    audio_player.end();
    pdm_mic_sensor.end();

    audio_player.start();
    pdm_mic_sensor.start();

    start = millis();

    print_memory();
}

void loop() {
    if (flag) return;

    if (millis() - start > duration) {
        audio_player.end();
        pdm_mic_sensor.end();
        flag = true;

        CircularBlockBuffer * pdm = PDM2.get_buffer();
        CircularBlockBuffer * i2s = i2s_player.get_buffer();

        Serial.println("HITS: " + String(PDM2.get_buffer_hits()));

        Serial.println("END");
        return;
    }

    update();
    //BLE.poll();
    return;

    while (PDM2.available()) {
        pdm_mic_sensor.update();
    }

    while (i2s_player.available()) {
        audio_player.update();
    }

    //update();
}

void update() {
    update_lm();

    if (check_overlap()) return;

    counter = 0;

    bool overlapped;
    if (_cycle++ % 2) {
        unsigned long t1 = millis();

        overlapped = update_pdm();

        if (counter != 0) {
            float avg = float(millis() - t1) / counter;
            apx_pdm = 0.5 * (avg + apx_pdm);
            if (avg > 0.5) Serial.println("F PDM: " + String(avg));
        }

        if (overlapped) {
            if (!check_overlap_prepopen()) {
                if (audio_player.ready_blocks() < min_ready_play) return;
                audio_player.pre_open_file();
            }
            //if (!check_overlap_prepopen())
            return;
        }
        counter = 0;
        t1 = millis();
        update_play();
        if (counter != 0) {
            float avg = float(millis() - t1) / counter;
            if (avg > 0.5) Serial.println("Play: " + String(avg));
        }
    } else {
        unsigned long t1 = millis();
        overlapped = update_play();
        if (counter != 0) {
            float avg = float(millis() - t1) / counter;
            apx_play = 0.5 * (avg + apx_play);
            if (avg > 0.5) Serial.println("F Play: " + String(avg));
        }
        if (overlapped) {
            //if (!check_overlap_prepopen()) pdm_mic_sensor.pre_open_file();
            return;
        }
        counter = 0;
        t1 = millis();
        update_pdm();
        if (counter != 0) {
            float avg = float(millis() - t1) / counter;
            if (avg > 0.5) Serial.println("PDM: " + String(avg));
        }
    }
}

unsigned long last = 0;

void update_lm() {
    unsigned int now = millis();
    if (now - _edge_ml_last > _edge_ml_delay) {
        _edge_ml_last = now;

        unsigned long T1 = millis();

        CircularBlockBuffer * pdm = PDM2.get_buffer();
        CircularBlockBuffer * i2s = i2s_player.get_buffer();


        // PDM: Write Big / Read Small
        // I2S: Write Small / Read Big
        Serial.println("PDM:  " + String(pdm->available_write()) + "  " + String(pdm->available_read()));
        Serial.println("I2S:  " + String(i2s->available_write()) + "  " + String(i2s->available_read()) + "\n");

        Serial.println("Time " + String(now-last));
        last = now;

        if (PDM2.get_buffer_hits() == 1) {
            Serial.println("HIT");
        }

        T1 = 10 - (millis() - T1);
        if (T1 <= 0) return;
        delay(T1);
        //delay(8);
        //Serial.println("Delay");
    }
}

bool update_pdm() {
    if (pdm_mic_sensor.ready_blocks() < min_ready_pdm) return check_overlap();

    int remaining = int(_edge_ml_delay - (millis() - _edge_ml_last));
    int max_buffers = remaining / apx_pdm;

    int cont = pdm_mic_sensor.update_contiguous(max_buffers);
    counter += cont;
    if (check_overlap()) return true;

    cont = max_buffers - cont;
    if (!cont || max_buffers == cont) return false;
    pdm_mic_sensor.update_contiguous(cont);
    counter += cont;

    return check_overlap();
    if (pdm_mic_sensor.ready_blocks() < min_ready_pdm) return check_overlap();

    for (int i=0; i<_buffer_loops; i++) {

        pdm_mic_sensor.update();
        counter++;
    }
    return false;
}

bool update_play() {
    if (audio_player.ready_blocks() < min_ready_play) return check_overlap();

    int remaining = int(_edge_ml_delay - (millis() - _edge_ml_last));
    int max_buffers = remaining / apx_play;

    int cont = audio_player.update_contiguous(max_buffers);
    counter += cont;
    if (check_overlap()) return true;

    cont = max_buffers - cont;
    if (!cont || max_buffers == cont) return false;
    audio_player.update_contiguous(cont);
    counter += cont;

    return check_overlap();
    if (audio_player.ready_blocks() < min_ready_play) return check_overlap();

    for (int i=0; i<_buffer_loops; i++) {
        if (check_overlap()) return true;
        audio_player.update();
        counter++;
    }
    return false;
}

bool check_overlap() {
    return (millis() - _edge_ml_last > _buffer_interval_time);
}

bool check_overlap_prepopen() {
    return (millis() - _edge_ml_last > _buffer_interval_time_prepopen);
}

















