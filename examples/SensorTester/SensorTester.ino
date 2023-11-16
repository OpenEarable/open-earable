// This skript requires the File 440Hz.wav provided under resources to be on the SD card.
// Additionaly the arduinoFFT library has to be installed.

#define NO_JINGLES

#include <Arduino.h>
//#include <Thread.h>

#include <arduinoFFT.h>

#include "OpenEarable.h"

String file_name = "play_file_2.wav";

String record_file;

long start;

bool sensor_started = true;
bool recording_finished = false;

const RGBColor red = {255,0,0};
const RGBColor green = {0,255,0};
const RGBColor off = {0,0,0};

//void (*task)();

#define NUM_TESTS 4

void test_button();
void test_imu();
void test_baro();
void test_mic();

void (*tasks[NUM_TESTS])() = {test_button, test_mic, test_baro, test_imu};

int task_num = 0;

uint8_t sensor_data[2][64];

long last_timestamp = 0;

void setup() {
    while(!Serial) {};
    open_earable.begin();

    edge_ml_generic.set_data_callback(on_sensor_data);

    Serial.println("------- Start Test -------");

    start = millis();
}

void test_button() {
  static bool init = true;
  static bool was_pressed = false;

  if (init) {
    earable_led.set_color(off);
    Serial.println("Test Button and RGB led: ");
    Serial.println("Please press and release the button. Make sure the RGB led turn white while the button is pressed.");
    init = false;
  }

  bool state = earable_btn.getState();

  if (!was_pressed && state == PRESSED) {
      was_pressed = true;
      const RGBColor white = {255,255,255};
      earable_led.set_color(white);
      Serial.println("Button pressed.");
      return;
  }

  if (was_pressed && state == RELEASED) {
      const RGBColor off = {0,0,0};
      earable_led.set_color(off);
      Serial.println("Button released.");
      Serial.println("Button test passed. \u2705");
      Serial.println("-------------------------");
      earable_led.set_color(green);
      task_num++;
      return;
  }
}

void test_imu() {
  static bool init = true;
  static bool was_pressed = false;
  static int state = 0;
  static float last_values[9] = {9};

  static long timestamp = 0;
  static float values_ema[9] = {0};

  static long time_pressed = 0;
  static bool passed = true;

  const float * values = (float *) (sensor_data[ACC_GYRO_MAG]);

  if (init) {
    earable_led.set_color(off);
    Serial.println("IMU Test: ");
    SensorConfigurationPacket config;

    config.sensorId = ACC_GYRO_MAG;
    config.sampleRate = 20; //1; //5
    config.latency = 0;
    open_earable.configure_sensor(config);

    init = false;

    Serial.println("Orient the earable horizontally.\nThen keep the button pressed and meanwhile rotate the earable 90° clockwise\nwithin in at most 2 seconds and immediately release the button.");
  }

  bool button_state = earable_btn.getState();

  const float alpha = 0.05;

  if (!was_pressed && button_state) {
    was_pressed = true;

    //float pressure = *(sensor_data[BARO_TEMP][sizeof(float)]);

    for (int i = 0; i < 9; i++) {
      /*Serial.print(values[i]);
      Serial.print(", ");*/
      last_values[i] = values[i];
    }
    //Serial.println();

    for (int i = 0; i < 9; i++) {
      values_ema[i] = values[i];
    }

    time_pressed = millis();
  } else if (was_pressed && !button_state) {
    was_pressed = false;
    /*for (int i = 0; i < 9; i++) {
      Serial.print(values_ema[i]);
      Serial.print(", ");
    }
    Serial.println();*/

    float duration = millis() - time_pressed;

    //check acc
    float acc = sqrt(values[0] * values[0] + values[1] * values[1] + values[2] * values[2]);
    
    switch(state) {
      case 0:
        if (abs(acc - 9.81) > 2 || values[1] < max(abs(values[0]), abs(values[2]))) {
          passed = false;
          Serial.println("Accelerometer test failed. \u274c");
          earable_led.set_color(red);
        }
        if (-values_ema[4] * duration / 1000 < 5 || -values_ema[4] < max(abs(values_ema[3]), abs(values_ema[5]))) {
          passed = false;
          Serial.println("Gyro test failed. \u274c");
          earable_led.set_color(red);
        }
        Serial.println("Keep the button pressed and meanwhile flip the side with the earpice upright (90°) \nwithin in at most 2 seconds and immediately release the button.");
        break;
      case 1:
        if (abs(acc - 9.81) > 2 || values[2] < max(abs(values[0]), abs(values[1]))) {
          passed = false;
          Serial.println("Accelerometer test failed. \u274c");
          earable_led.set_color(red);
        }
        if (-values_ema[3] * duration / 1000 < 5 || -values_ema[3] < max(abs(values_ema[4]), abs(values_ema[5]))) {
          passed = false;
          Serial.println("Gyro test failed. \u274c");
          earable_led.set_color(red);
        }
        Serial.println("Keep the button pressed and meanwhile rotate the earable by 90° clockwise \nwithin in at most 2 seconds and immediately release the button.");
        break;
      case 2:
        if (abs(acc - 9.81) > 2 || values[2] < max(abs(values[0]), abs(values[1]))) {
          passed = false;
          Serial.println("Accelerometer test failed. \u274c");
          earable_led.set_color(red);
        }
        if (-values_ema[5] * duration / 1000 < 5 || -values_ema[5] < max(abs(values_ema[3]), abs(values_ema[4]))) {
          passed = false;
          Serial.println("Gyro test failed. \u274c");
          earable_led.set_color(red);
        }
        break;
      /*case 3:
        if (abs(acc - 9.81) > 2 || values[2] < max(abs(values[0]), abs(values[1]))) Serial.println("Accelerometer test failed. \u274c");
        if (values_ema[5] * duration / 1000 < 5 || values_ema[5] < max(abs(values_ema[3]), abs(values_ema[4]))) Serial.println("Gyro test failed. \u274c");
        break;
      case 4:
        if (abs(acc - 9.81) > 2 || values[2] < max(abs(values[0]), abs(values[1]))) Serial.println("Accelerometer test failed. \u274c");
        if (values_ema[3] * duration / 1000 < 5 || values_ema[3] < max(abs(values_ema[4]), abs(values_ema[5]))) Serial.println("Gyro test failed. \u274c");
        break;
      case 5:
        if (abs(acc - 9.81) > 2 || values[1] < max(abs(values[0]), abs(values[2]))) Serial.println("Accelerometer test failed. \u274c");
        if (values_ema[4] * duration / 1000 < 5 || values_ema[4] < max(abs(values_ema[3]), abs(values_ema[5]))) Serial.println("Gyro test failed. \u274c");
        break;*/
    }

    state++;

    if (state == 3) {
      if (passed) {
        earable_led.set_color(green);
        Serial.println("IMU test passed. \u2705");
      } else {
        earable_led.set_color(red);
        Serial.println("IMU test failed. \u274c");
      }
      Serial.println("-------------------------");
      open_earable.stop_all_sensors();
      task_num++;
      return;
    }
  }

  if (was_pressed && button_state) {
    if (timestamp != last_timestamp) {
      timestamp = last_timestamp;
      for (int i = 0; i < 9; i++) {
        values_ema[i] = alpha * values[i] + (1 - alpha) * values_ema[i];
      }
    }
  }
}

void test_baro() {
  static bool init = true;
  static long timestamp = 0;

  if (init) {
    earable_led.set_color(off);
    Serial.println("Baro Test: ");

    SensorConfigurationPacket config;

    config.sensorId = BARO_TEMP;
    config.sampleRate = 20; //1; //5
    config.latency = 0;
    open_earable.configure_sensor(config);

    Serial.println("Please blow firmly into the earpeace to the side with the pressure sensor.");
    init = false;
  }

  float temp = *((float *) sensor_data[BARO_TEMP]);
  float pressure = *reinterpret_cast<const float*>(sensor_data[BARO_TEMP] + sizeof(float));
  //float pressure = *(sensor_data[BARO_TEMP][sizeof(float)]);

  const float alpha = 0.01;

  static float temp_ema = temp;
  static float pressure_ema = pressure;

  if (temp_ema == 0) temp_ema = temp;
  if (pressure_ema == 0) pressure_ema = pressure;

  if (timestamp != last_timestamp) {
    timestamp = last_timestamp;

    temp_ema = alpha * temp + (1 - alpha) * temp_ema;
    pressure_ema = alpha * pressure + (1 - alpha) * pressure_ema;

    /*Serial.print(pressure);
    Serial.print(", ");
    Serial.println(pressure_ema);*/
  }

  if (temp - temp_ema > 0.5 && pressure - pressure_ema > 70) {
      Serial.println("BMP280 test passed. \u2705");
      Serial.println("-------------------------");
      open_earable.stop_all_sensors();
      earable_led.set_color(green);
      task_num++;
      return;
  }
}

void test_mic() {
  static bool init = true;
  static bool stopped = false;
  static long timestamp = 0;
  static bool was_pressed = false;

  if (init) {
    earable_led.set_color(off);
    Serial.println("Microphone and Speaker Test: ");

    Serial.println("Make sure you are in a quiet environment and the SD card is inserted. Then press the button.");

    init = false;
  }

  bool button_state = earable_btn.getState();

  if (!was_pressed && button_state) {
    was_pressed = true;

    SensorConfigurationPacket config;

    audio_player.setSource(new WavPlayer("440Hz.wav"));
    audio_player.begin();
    audio_player.play();

    config.sensorId = PDM_MIC;
    config.sampleRate = 62500; //1; //5
    config.latency = 0;
    open_earable.configure_sensor(config);

    timestamp = millis();
  }

  if (!stopped && was_pressed && !button_state && !i2s_player.is_running()) {
    stopped = true;
    recorder.stop();

    const int samples = 1024;

    int16_t signal[samples];

    double vReal[samples];
    double vImag[samples] = {0};

    const String recording_name = ((WavRecorder *)(recorder.target))->get_name();
    ExFatFile file = sd_manager.openFile(recording_name, false);

    sd_manager.read_block_at(&file, 30000 * 2 + sizeof(WaveInfo), (uint8_t*)signal, samples * 2);
    sd_manager.closeFile(&file);

    for (int i = 0; i < samples; i++) {
      vReal[i] = signal[i];
      //Serial.println(signal[i]);
    }

    arduinoFFT FFT = arduinoFFT(vReal, vImag, samples, 62500);
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    FFT.ComplexToMagnitude();

    /*for (int i = 0; i < 30; i++) {
      Serial.println(vReal[i]);
    }*/

    //Serial.println();

    float freq = FFT.MajorPeakParabola();

    if (abs(freq - 440) < 10) {
      Serial.println("PDM test passed. \u2705");
      earable_led.set_color(green);
    } else {
      Serial.println("PDM test failed. \u274c");
      earable_led.set_color(red);
    }

    Serial.print("Frequency: ");
    Serial.print(freq);
    Serial.println("Hz");
    Serial.println("-------------------------");
    
    open_earable.stop_all_sensors();

    task_num++;
  }
}

void loop() {
  open_earable.update();
  if (task_num < NUM_TESTS) tasks[task_num]();
  else {
    Serial.print("Test finished!");
    while (true) {};
  }
}


static void on_sensor_data(int id, unsigned int timestamp, uint8_t * data, int size) {
    memcpy(&(sensor_data[id][0]), data, size);
    last_timestamp = timestamp;
}