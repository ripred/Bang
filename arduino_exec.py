#
# arduino_exec.py
#
# Python Agent for the ArduinoCLI platform
# https://github.com/ripred/ArduinoCLI
#
#  v1.0 written 2023-12-10 ++trent m wyatt
#
import subprocess
import signal
import serial
import json
import sys

# A list a abbreviated commands that the Arduino
# can send to run a pre-registered command:
macros = {}


# function to get the serial port to use from the command line
def get_args():
    if len(sys.argv) <= 0:
        print("No COM port argument provided.")
        exit(-1)

    return sys.argv[1]


# The signal handler that is called when the user hits ctrl-c
def sigint_handler(signum, frame):
    print(" User hit ctrl-c, exiting.")
    save_macros(macros)
    sys.exit(0)


# function to set the signal handler for SIGINT
# (for when the user hits ctrl-c)
def set_signal_handler():
    signal.signal(signal.SIGINT, sigint_handler)


# function to open the specified serial port
def open_serial_port(port):
    cmd_serial = serial.Serial(port, 9600, timeout=1)

    if not cmd_serial:
        print(f"Could not open the serial port {port}")
        exit(0)

    print(f"Succesfully opened serial port {port}")
    return cmd_serial


# function to execute a command and capture any output
def execute_command(command):
    print(f"Executing: {command}")  # Output for the user
    try:
        result = subprocess.check_output(command, shell=True,
                                         stderr=subprocess.STDOUT)
        return result.decode('utf-8')  # Decode bytes to string
    except subprocess.CalledProcessError as e:
        return str(e.output)


# ================================================================================
def load_macros(filename='macros.txt'):
    try:
        with open(filename, 'r') as file:
            return json.load(file)
    except FileNotFoundError:
        return {}


def save_macros(macros, filename='macros.txt'):
    with open(filename, 'w') as file:
        json.dump(macros, file, indent=4, sort_keys=True)


def create_macro(name, command, macros):
    macros[name] = command
    save_macros(macros)


def read_macro(name, macros):
    return macros.get(name, "Macro not found")


def execute_macro(name, macros):
    if name in macros:
        return execute_command(macros[name])
    else:
        return f"Macro '{name}' not found"


def delete_macro(name, macros):
    if name in macros:
        del macros[name]
        save_macros(macros)
        return f"Macro '{name}' deleted"
    else:
        return f"Macro '{name}' not found"


# ================================================================================


def run():
    global macros

    port = get_args()
    cmd_serial = open_serial_port(port)
    set_signal_handler()
    macros = load_macros()

    prompted = False
    while True:
        if not prompted:
            print("Waiting for a command from the Arduino...")
        prompted = True

        arduino_command = cmd_serial.readline().decode('utf-8').strip()
        arduino_command = arduino_command.strip()

        if not arduino_command:
            continue

        print(f"Received command from Arduino: '{arduino_command}'")

        cmd_id = arduino_command[0]     # Extract the first character
        command = arduino_command[1:]   # Extract the remainder of the command
        result = ""

        # Check if the command is a macro related command:
        if cmd_id == '@':
            if command in macros:
                result = execute_command(macros[command])
            elif command == "list_macros":
                macro_list = [f'    "{macro}": "{macros[macro]}"'
                              for macro in macros]
                result = "Registered Macros:\n" + "\n".join(macro_list)
            elif command.startswith("add_macro:"):
                _, name, command = command.split(":")
                create_macro(name, command, macros)
                result = f"Macro '{name}' created with command '{command}'"
            elif command.startswith("delete_macro:"):
                _, name = command.split(":")
                result = delete_macro(name, macros)
            else:
                result = f"unrecognized macro command: {command}"
        elif cmd_id == '!':
            # Dispatch the command to handle built-in commands
            result = execute_command(command)
        else:
            result = f"unrecognized cmd_id: {cmd_id}"

        for line in result.split('\n'):
            print(line + '\n')
            cmd_serial.write(line.encode('utf-8') + b'\n')

        prompted = False


if __name__ == '__main__':
    run()
