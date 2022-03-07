import subprocess
import os
import time
import sys
import keyboard

OPTIONS = ["-p", "-a"]

class Simulator :
    '''
    Simulator object that opens a new window and runs the simulation
    '''
    def __init__(self):
        # Buffer select can either be 0 or 1
        self.buffer_select = 0

        # Load child process and set up communication pipes
        if os.name == 'nt':
            path = ["./../vector-simulator/target/release/vector-simulator.exe"]
            path.extend(OPTIONS)
            self.child_process = subprocess.Popen(path, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        elif os.name == 'posix':
            path = ['./../vector-simulator/target/release/vector-simulator']
            path.extend(OPTIONS)
            self.child_process = subprocess.Popen(path, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        else:
            path = ["../vector-simulator/target/release/vector-simulator"]
            path.extend(OPTIONS)
            self.child_process = subprocess.Popen(path, stdin=subprocess.PIPE, stdout=subprocess.PIPE)

    def send_command(self, command) :
        self.child_process.stdin.write(f"{command}\n".encode())
        self.child_process.stdin.flush()

    def create_vec(x, y, brightness) :
        '''
        '''

        x_bin = Simulator.num_to_bin(x, 10)
        y_bin = Simulator.num_to_bin(y, 10)
        brightness_bin = Simulator.num_to_bin(brightness, 10)

        print(f"{x}:{x_bin} {y}:{y_bin} {brightness}:{brightness_bin}")
        return (x_bin, y_bin, brightness_bin)

    def absolute_vec(self, x, y, brightness) :
        '''
        '''
        vec = Simulator.create_vec(x, y, brightness)

        self.send_command(f"10{vec[0]}{vec[1]}{vec[2]}")

    def relative_vec(self, x_delta, y_delta, brightness) :
        '''
        '''
        vec = Simulator.create_vec(x_delta, y_delta, brightness)

        self.send_command(f"11{vec[0]}{vec[1]}{vec[2]}")

    def draw_current_buffer(self) :
        '''
        '''
        self.send_command(f"0110")
    
    def draw_buffer_A(self) :
        '''
        '''
        self.send_command(f"0100")

    def draw_buffer_B(self) :
        '''
        '''
        self.send_command(f"0101")
    
    def draw_buffer_switch(self) :
        '''
        '''
        self.send_command(f"0111")

    def halt(self) :
        '''
        '''
        self.send_command(f"0")
    
    def num_to_bin(value: int, width: int=None) -> str:
        if width is None:
            width = 8
        if value < 0:
            value = 2 ** width + value
        template = "{:0" + str(width + 2) + "b}"
        return template.format(value)[2:]                                                     

if __name__ == "__main__" :
    print("Staring simulator demo...")

    # Create simulator object
    sim = Simulator()
    
    box_x = 0
    box_y = 0

    while True :
        # Draw box
        sim.absolute_vec(box_x, box_y, 0)
        sim.absolute_vec(box_x + 100, box_y, 1023)
        sim.absolute_vec(box_x + 100, box_y + 100, 1023)
        sim.absolute_vec(box_x, box_y + 100, 1023)
        sim.absolute_vec(box_x, box_y, 1023)
        sim.halt()

        # Wait for the next event.
        event = keyboard.read_event()
        
        # Get input
        if event.event_type == keyboard.KEY_DOWN and event.name == 'up':
            box_y += 10
        elif event.event_type == keyboard.KEY_DOWN and event.name == 'down':
            box_y -= 10
        elif event.event_type == keyboard.KEY_DOWN and event.name == 'left':
            box_x -= 10
        elif event.event_type == keyboard.KEY_DOWN and event.name == 'right':
            box_x += 10
        
        
