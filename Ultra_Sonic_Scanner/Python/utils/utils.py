import time
from math import ceil
import csv

short_parser = lambda data: int.from_bytes(data, "little", signed=True)
SHORT_SIZE = 2


def csv_writer(path, headers, dataset):
    with open(path, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["Audio"])
        for value in dataset:
            writer.writerow([value])


class Timer:
    def __init__(self):
        self.times = {}

    def start(self, name):
        if name not in self.times:
            self.times[name] = [None, None]

        self.times[name][0] = time.perf_counter()

    def stop(self, name):
        if name not in self.times:
            return
        self.times[name][1] = time.perf_counter()

    def clear(self):
        self.times = {}

    def show(self, name, auto_stop=True):
        if name not in self.times:
            return

        if auto_stop:
            self.stop(name)
        s, e = self.times[name]
        delta = round(e - s, 2)
        print(f"{name} took {delta}s")


class Buffer:
    def __init__(self, size=0):
        self.buffer = []
        self.size = size
        self.offset = 0

        self.set_size(size)

    def set_size(self, size):
        self.size = size
        self.buffer = [0] * size
        self.offset = 0

    def add(self, values):
        length = len(values)
        self.buffer[self.offset: length + self.offset] = values
        self.offset += length

    def clear(self):
        self.set_size(self.size)

    def show(self):
        print(f"Elements: {len(self.buffer)}")


class Packet_Manager:
    def __init__(self):
        self.received = 0
        self.expected = 0

        self.state = 0
        self.has_expected = False

        self.progress = Progress()

    def set_expected(self, expected):
        self.expected = expected
        self.has_expected = True
        self.progress.set_total(self.expected)

    def set_received(self, received):
        self.received = received

    def increment(self):
        self.received += 1

    def clear(self):
        self.received = 0
        self.expected = 0
        self.state = 0
        self.has_expected = False
        self.progress.clear()

    def set_state(self, state):
        self.state = state

    def show(self):
        print(f"Packages: {self.received}/{self.expected}")

    def show_progress(self):
        self.progress.show(self.received)


class Progress:
    def __init__(self):
        self.amount = 20
        self.symbol = "#"
        self.blank = " "
        self.minimal = 0.02

        self.last = None
        self.total = None

    def set_total(self, total):
        self.total = total

    def clear(self):
        self.last = None

    def show(self, value, total=None):
        if total is None:
            total = self.total
        if total is None:
            return

        percent = value / total

        if self.last is not None:
            if percent - self.last < self.minimal:
                return

        self.last = percent

        bar = self.symbol * ceil(percent * self.amount)
        bar += self.blank * (self.amount - len(bar))

        percent *= 100
        print(f"Progress:  {bar}  |  {percent:.2f}%")
