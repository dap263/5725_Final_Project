import spidev
import time


bus = 0

device = 1

spi = spidev.SpiDev()
spi.open(bus, device)

spi.max_speed_hz = 500000

spi.mode = 0

