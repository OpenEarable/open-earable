import serial
import array
import time

import matplotlib.pyplot as plt
from scipy.signal import find_peaks
import numpy as np

recording_time = 2  # in seconds
peak_finder = True

port = 'COM9'
baudrate = 1000000
timeout = 1

sample_rate = 16000
package_size = 1024

ser = serial.Serial(port, baudrate=baudrate, timeout=timeout)
time.sleep(0.5)

START = b'1'
STOP = b'0'
CONFIRM = b'1'


def main():
    print("Starting")
    send_start()
    print("Recording")
    data = receive_data()
    print("Stopping")
    send_stop()
    end_serial()
    print("Evaluating")
    evaluate(data)


def send_start():
    ser.write(START)
    check_response()


def receive_data():
    max_samples = sample_rate * recording_time
    package_count = max_samples // package_size
    samples = [0] * (package_count + 1) * package_size

    for i in range(package_count + 1):
        index = i * package_size // 2
        samples[index:index + package_size] = receive_package()
    return samples


def send_stop():
    ser.write(STOP)
    ser.reset_input_buffer()


def end_serial():
    ser.close()


def evaluate(data):
    print("Show")
    data = np.array(data)
    plt.plot(data)

    if peak_finder:
        print("Find peaks")
        peaks, _ = find_peaks(data, prominence=5)
        plt.plot(peaks, data[peaks], "xr")
        diff = np.diff(peaks)
        mean = np.mean(diff)
        print(f"{mean=}")

    # label the axes
    plt.ylabel("Amplitude")
    plt.xlabel("Time")
    # set the title
    plt.title("Samples")
    # display the plot
    plt.show()


def check_response():
    if ser.read(1) == CONFIRM:
        return True
    raise Exception("No Response")


def receive_package():
    raw_data = ser.read(package_size)
    return array.array('h', raw_data)


if __name__ == "__main__":
    main()
