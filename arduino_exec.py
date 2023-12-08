import serial
import subprocess

# Replace 'COM4' with the actual full path to the port your
# Arduino is connected to to transmit the response to.
#
# For example linux and Mac users would
# use something like this:
#
# command_port = '/dev/cu.usbserial-A4016Z9Q'
#

command_port = 'COM4'

cmd_serial = serial.Serial(command_port, 9600, timeout=1)

if cmd_serial:
    print(f"succesfully opened serial port {command_port}")
else:
    print(f"could not open the serial port {command_port}")
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

prompted = False
while True:
    if (not prompted):
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

        # Print the output for debugging (optional)
        print(f"Command Output:\n{result_output}")

        # send the captured output to the Arduino
        if cmd_serial:
            cmd_serial.write(result_output.encode('utf-8') + b'\n')

        prompted = False
