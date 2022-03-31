import vsim
from time import sleep

if __name__ == "__main__" :
    print("Staring simulator demo...")

    with open("buff0", 'w') as f :
        f.write("vec 0 0 0\nhalt")

    # Create simulator object
    sim = vsim.Simulator()

    chars = ['A', 'B', 'C']

    for char in chars :
        print(char)

        # Write char to buff and execute
        # Fonts are only in relative coordinates
        with open(f"fonts/simple/{char}", 'r') as f :
            text = f.read()

            print(text)

            sim.write_alt_buff(text)

            sim.toggle_buffer_select()

        sleep(5)
        # Now, shift vector over by 50 pixels
        # and go back to top

        commands = [
            "rvec 50 -100 0",
            "halt"
        ]

        sim.write_alt_buff("\n".join(commands))

        sim.toggle_buffer_select()