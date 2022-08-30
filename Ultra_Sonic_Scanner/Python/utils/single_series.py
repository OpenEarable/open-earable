import asyncio
from bleak import BleakScanner, BleakClient, BleakError
import matplotlib.pyplot as plt
import numpy as np
from utils.utils import *

from bleak.backends.device import BLEDevice


PREFIX = "Earable_Audio"

SERVICE = "00000002-0000-1000-8000-00805f9b34fb"
SERVICE_INFO = "00000003-0000-1000-8000-00805f9b34fb"

delay = 0.05
buffer_size = 122880 // 2

buffer = Buffer(buffer_size)
package_man = Packet_Manager()
timer = Timer()


async def clear_all():
    package_man.clear()
    buffer.clear()
    timer.clear()

async def get_aura_device() -> BLEDevice | None:
    print("Getting Ear device")
    device = await BleakScanner.find_device_by_filter(
        lambda d, ad: d.name and d.name.startswith(PREFIX),
        timeout=10
    )
    return device


async def single_measurement(plot=True):
    await clear_all()

    timer.start("Get Device")
    device = await get_aura_device()
    if not device:
        raise BleakError(f"No Aura with prefix {PREFIX} found.")
    print(f"Found EAR: {device}")
    timer.show("Get Device")

    timer.start("Connect")

    async with BleakClient(device) as client:
        print(f"Connected: {client.is_connected}")
        timer.show("Connect")
        timer.start("Receive")
        await client.start_notify(SERVICE, handler)
        await client.start_notify(SERVICE_INFO, handler_info)
        await waiter()
        await client.stop_notify(SERVICE)
    print("Sending done")
    timer.show("Receive")
    package_man.show()
    buffer.show()

    if plot:
        evaluate(buffer.buffer)
    print("End")

    return buffer.buffer


async def waiter():
    while not package_man.state:
        await asyncio.sleep(delay)


def handler(sender, data):
    package_man.increment()
    size = short_parser(data[:SHORT_SIZE])
    values = parser(data, size)
    buffer.add(values)
    package_man.show_progress()


def parser(data, size):
    offset = SHORT_SIZE
    split = [data[i:i + SHORT_SIZE] for i in range(offset, size + offset, SHORT_SIZE)]
    values = list(map(short_parser, split))
    return values


def values_to_buffer(values):
    global buffer, buffer_offset
    length = len(values)
    buffer[buffer_offset: length + buffer_offset] = values
    buffer_offset += length


def handler_info(sender, data):
    packages = short_parser(data[:SHORT_SIZE])
    state = short_parser(data[SHORT_SIZE:])

    package_man.set_expected(packages)
    package_man.set_state(state)

def evaluate(data):
    print("Show")
    data = np.array(data)
    plt.plot(data)

    # label the axes
    plt.ylabel("Amplitude")
    plt.xlabel("Time")
    # set the title
    plt.title("Samples")
    # display the plot
    plt.show()

if __name__ == "__main__":
    asyncio.run(single_measurement())
