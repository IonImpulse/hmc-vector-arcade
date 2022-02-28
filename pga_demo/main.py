import os
os.path.append('../vector-simulator')

from vsim import *
from time import sleep

sim = Simulator()

while True:
    sleep(1)
    sim.toggle_buffer_select()

    print(f'Drawing frame {sim.buffer_select}')
