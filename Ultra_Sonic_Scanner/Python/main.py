import asyncio
from utils.single_series import single_measurement
from utils.utils import Timer, csv_writer
import os

measurements = 8
plot = False

data_sets = []
data_path = "data"

timer = Timer()

async def main():
    print("Starting series")
    timer.start("Total series")
    for i in range(1, measurements+1):
        print(f"\n\nMeasurement: {i}/{measurements}")
        data = await single_measurement(plot=plot)
        data_sets.append(data)
        await asyncio.sleep(8)
        path = f"{data_path}/Measurement_{i-1}.csv"
        csv_writer(path, ["Audio"], data_sets[i-1])

    timer.show("Total series")
    print("\n\nSeries over")


if __name__ == "__main__":
    if not os.path.exists(data_path):
        os.makedirs(data_path)
    asyncio.run(main())
