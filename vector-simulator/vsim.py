import subprocess
import os
import time
import sys

OPTIONS = ["-p"]

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
            self.child_process = subprocess.Popen(path + ['-p'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
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
    
    def num_to_bin(num, wordsize):
        if num < 0:
            num = 2**wordsize+num
        base = bin(num)[2:]
        padding_size = wordsize - len(base)
        return '0' * padding_size + base                                                       

if __name__ == "__main__" :
    print("Staring simulator demo...")

    # Create simulator object
    sim = Simulator()

    while True :
        # Draw absolute vec to 300, 300, with max brightness
        print(Simulator.create_vec(300, 300, 1023))
        sim.absolute_vec(0, 0, 1023)
        sim.absolute_vec(300, 300, 1023)

        sim.halt()

        sim.draw_current_buffer()

        time.sleep(1)
