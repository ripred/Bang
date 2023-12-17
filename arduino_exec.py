import subprocess
import signal
import serial
import sys

prompted = False

def sigint_handler(signum, frame):
    print("\nuser hit ctrl-c, exiting.")
    sys.exit(0)


# Check if there are command-line arguments
if len(sys.argv) > 1:
    # Retrieve and print command-line arguments
    command_port = sys.argv[1]
else:
    print("No COM port argument provided.")
    exit(-1)

#command_port = '/dev/cu.usbserial-A4016Z9Q'

cmd_serial = serial.Serial(command_port, 9600, timeout=1)

if cmd_serial:
    print(f"Succesfully opened serial port {command_port}")
else:
    print(f"Could not open the serial port {command_port}")
    exit(0)

def execute_command(command):
    print(f"Executing: {command}")  # Output for the user
    try:
        # Execute the command and capture the output
        result = subprocess.check_output(command, shell=True, stderr=subprocess.STDOUT)
        return result.decode('utf-8')  # Decode bytes to string
    except subprocess.CalledProcessError as e:
        # If the command execution fails, return the error message
        return str(e.output)

# Set the SIGINT handler
signal.signal(signal.SIGINT, sigint_handler)

while True:
    if not prompted:
        print("Waiting for a command from the Arduino...")
    prompted = True

    # Read a line from the Arduino
    arduino_command = cmd_serial.readline().decode('utf-8').strip()

    # Strip whitespace from the beginning and end of the line
    arduino_command = arduino_command.strip()

    if arduino_command:
        print(f"Received command from Arduino: '{arduino_command}'")

        # Execute the command
        result_output = execute_command(arduino_command)

        # Split the result into lines and send each line individually
        for line in result_output.split('\n'):
            print(line + '\n')
            if cmd_serial:
                cmd_serial.write(line.encode('utf-8') + b'\n')

        prompted = False

