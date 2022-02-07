import subprocess
import os
import time

OPTIONS = ["-p", "-d", "0.3", "-f", "0.00000001", "-s", "100000000"]

class Simulator :
    '''
    Simulator object that opens a new window and runs the simulation
    '''
    def __init__(self):
        # Buffer select can either be 0 or 1
        self.buffer_select = 0
        
        # Load child process and set up communication pipes
        if os.name == 'nt':
            path = ["./../vector-simulator/vector-simulator.exe"]
            path.extend(OPTIONS)
            self.child_process = subprocess.Popen(path, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        else:
            path = ["../vector-simulator/vector-simulator"]
            path.extend(OPTIONS)
            self.child_process = subprocess.Popen(path, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        
    def send_command(self, command) :
        self.child_process.stdin.write(f"{command}\n".encode())
        self.child_process.stdin.flush()
    
    def toggle_buffer_select(self) :
        '''
        Toggles buffer select between 0 and 1
        '''
        self.buffer_select = (self.buffer_select + 1) % 2
        self.send_command(f"{self.buffer_select}")

    def set_buffer_select(self, buffer_select) :
        '''
        Sets the buffer select
        Throws error if buffer select is not 0 or 1
        '''
        if buffer_select not in [0, 1] :
            return Exception("Buffer select must be 0 or 1")

        self.buffer_select = buffer_select
        self.send_command(f"{self.buffer_select}")
    

if __name__ == "__main__" :
    print("Staring simulator demo...")

    # Create simulator object
    sim = Simulator()

    while True :
        sim.toggle_buffer_select()
        
        print(f"{sim.buffer_select}")
        # Wait for frame delay
        time.sleep(1)
