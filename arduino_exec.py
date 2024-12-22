#!/usr/bin/env python3
"""
arduino_exec.py

@brief Python Agent for the ArduinoCLI platform. This script allows
communication with Arduino boards, enabling the execution of built-in
commands, macros, and compilation/upload of Arduino code.

See the project repository for full details, installation, and use:
https://github.com/ripred/ArduinoCLI

@author Trent M. Wyatt
@date 2023-12-10
@version 1.4

Release Notes:
1.4 - Modified to properly close/toggle DTR before uploading on an ATmega328-based Nano.

1.3 - added support for serial output

1.2 - added support for compiling, uploading, and replacing the
      functionality in the current Arduino program flash memory.

1.1 - added support for macro functionality.

1.0 - implemented the basic 'execute and capture output' functionality.

IMPORTANT NOTE:
The '&' (compile and upload) operations require the Arduino CLI tool to
be installed on your system. Arduino CLI is a command-line interface that
simplifies interactions with Arduino boards. If you don't have Arduino CLI
installed, you can download it from the official Arduino website:
https://arduino.cc/en/software

Follow the installation instructions for your operating system provided
on the Arduino website. Once installed, make sure the 'arduino-cli'
executable is in your system's PATH. The '&' operations use
'arduino-cli compile' and 'arduino-cli upload' commands to compile and
upload Arduino code. Ensure the Arduino CLI commands are accessible
before using the compile and upload functionality.

======================================================================
"""

import subprocess
import argparse
import logging
import signal
import serial
import json
import sys
import os
import time

# A list of abbreviated commands that the Arduino can send to run a pre-registered command:
macros = {}

# The logger
logger = None

# The serial port
cmd_serial = None

# Store command-line args globally so other functions can reference them
ARGS = None


def parse_args():
    """
    @brief Parse command line arguments.

    Parses the command line arguments using argparse.

    @return argparse.Namespace: The parsed arguments.
    """
    parser = argparse.ArgumentParser(description='ArduinoCLI Python Agent')
    parser.add_argument('-p', '--port', required=True, help='Serial port name')
    parser.add_argument('-b', '--baud', type=int, default=38400, help='Baud rate')
    return parser.parse_args()


def setup_logger():
    """
    @brief Set up the logger for error logging.

    Configures a logger to log errors to both the console and a file.
    """
    global logger

    # Set up logging configuration
    logging.basicConfig(level=logging.ERROR)  # Set the logging level to ERROR

    file_name = 'bang.log'
    file_handler = logging.FileHandler(
        os.path.join(os.path.abspath(os.path.dirname(__file__)),
                     file_name))
    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
    file_handler.setFormatter(formatter)

    logger = logging.getLogger(__name__)
    logger.setLevel(logging.ERROR)  # Set the logging level to ERROR
    logger.addHandler(file_handler)


def sigint_handler(signum, frame):
    """
    @brief Signal handler for SIGINT (Ctrl+C).

    Handles the SIGINT signal (Ctrl+C) to save macros and exit gracefully.
    """
    print(" User hit ctrl-c, exiting.")
    save_macros(macros)
    sys.exit(0)


def set_signal_handler():
    """
    @brief Set the signal handler for SIGINT.
    """
    signal.signal(signal.SIGINT, sigint_handler)


def open_serial_port(port, baud):
    """
    @brief Open the specified serial port with a timeout of 30ms.

    @return serial.Serial: The opened serial port.
    """
    global cmd_serial

    try:
        cmd_serial = serial.Serial(port, baud, timeout=0.030)
    except Exception as e:
        result = f"Failed to open the serial port: '{port}'. {str(e)}"
        print(result)
        sys.exit(-1)

    if not cmd_serial:
        print(f"Could not open the serial port: '{port}'")
        sys.exit(-1)

    print(f"Successfully opened serial port: '{port}'")
    return cmd_serial


def execute_command(command):
    """
    @brief Execute a command and capture the output.

    Executes a command using subprocess and captures the output.
    If an error occurs, logs the error and returns an error message.
    """
    try:
        result = subprocess.check_output(command, shell=True,
                                         stderr=subprocess.STDOUT, text=True)
        return result.strip()
    except subprocess.CalledProcessError as e:
        errtxt = f"Error executing command: {e}"
        logger.error(errtxt)
        return errtxt
    except Exception as e:
        errtxt = f"An unexpected error occurred: {e}"
        logger.error(errtxt)
        return errtxt


def load_macros(filename='macros.txt'):
    """
    @brief Load macros from a file.
    """
    try:
        with open(filename, 'r') as file:
            return json.load(file)
    except FileNotFoundError:
        return {}


def save_macros(macros_dict, filename='macros.txt'):
    """
    @brief Save macros to a file.
    """
    with open(filename, 'w') as file:
        json.dump(macros_dict, file, indent=4, sort_keys=True)


def create_macro(name, command, macros_dict):
    """
    @brief Create a new macro.
    """
    macros_dict[name] = command
    save_macros(macros_dict)


def read_macro(name, macros_dict):
    """
    @brief Read the command associated with a macro.
    """
    return macros_dict.get(name, "Macro not found")


def execute_macro(name, macros_dict):
    """
    @brief Execute a macro.
    """
    if name in macros_dict:
        return execute_command(macros_dict[name])
    else:
        return f"Macro '{name}' not found"


def delete_macro(name, macros_dict):
    """
    @brief Delete a macro.
    """
    if name in macros_dict:
        del macros_dict[name]
        save_macros(macros_dict)
        return f"Macro '{name}' deleted"
    else:
        return f"Macro '{name}' not found"


def reset_nano_dtr(port_name):
    """
    @brief Toggle DTR on an ATmega328-based Nano to reset it into the bootloader.

    Opening the port sets DTR high by default; we lower it briefly, then raise it again.
    """
    print(f"Toggling DTR on port: {port_name} to reset the Nano...")
    try:
        with serial.Serial(port_name, 9600, timeout=1) as s:
            # Lower DTR
            s.dtr = False
            time.sleep(0.1)
            # Raise DTR
            s.dtr = True
        # Wait a bit so the bootloader can start
        time.sleep(0.5)
    except Exception as e:
        print(f"Warning: Could not toggle DTR for reset: {e}")


def compile_and_upload(folder):
    """
    @brief Compile and upload Arduino code from the specified folder.

    On an ATmega328-based Nano, we:
      1) Close the current Python serial connection (so avrdude can own the port).
      2) Manually toggle DTR to reset the board.
      3) Call 'arduino-cli compile' and 'arduino-cli upload'.
      4) Reopen the Python serial port so we can keep receiving commands.
    """
    global cmd_serial, ARGS

    # Check if the specified folder exists
    if not os.path.exists(folder):
        return f"Error: Folder '{folder}' does not exist."

    # Check if the folder contains a matching .ino file
    ino_file = os.path.join(folder, f"{os.path.basename(folder)}.ino")
    if not os.path.isfile(ino_file):
        return f"Error: Folder '{folder}' does not contain a matching .ino file."

    # Use the port the user gave us
    port_name = ARGS.port
    fqbn = "arduino:avr:nano:cpu=atmega328old"  # Adjust if needed
    compile_command = f"arduino-cli compile --fqbn {fqbn} {folder}"
    upload_command = f"arduino-cli upload -p {port_name} --fqbn {fqbn} {folder}"

    # 1) Close our current Python-held serial port
    if cmd_serial and cmd_serial.is_open:
        print("Closing Python serial port before compile/upload...")
        cmd_serial.close()
        time.sleep(0.5)  # Give OS time to actually release the port

    # 2) Manually toggle DTR to reset the Nano
    reset_nano_dtr(port_name)

    # 3) Compile and upload
    compile_result = execute_command(compile_command)
    print(f"executed: {compile_command}\nresult: {compile_result}")

    upload_result = execute_command(upload_command)
    print(f"executed: {upload_command}\nresult: {upload_result}")

    # 4) Reopen the port so we can continue reading commands
    print("Reopening Python serial port after upload...")
    cmd_serial = open_serial_port(port_name, ARGS.baud)

    # Summarize results
    return f"Compile Result:\n{compile_result}\nUpload Result:\n{upload_result}"


def run():
    """
    @brief Main execution function.

    Handles communication with Arduino, waits for commands, and executes them.
    """
    global macros
    global cmd_serial
    global ARGS

    ARGS = parse_args()
    cmd_serial = open_serial_port(ARGS.port, ARGS.baud)

    set_signal_handler()
    macros = load_macros()
    setup_logger()

    print("Waiting for commands from Arduino...")

    while True:
        try:
            arduino_command = cmd_serial.readline().decode('utf-8').strip()
        except Exception as e:
            logger.error(f"An unexpected error occurred: {e}")
            continue

        if not arduino_command:
            continue

        logtext = f"Received command from Arduino: '{arduino_command}'"
        logger.info(logtext)

        cmd_id = arduino_command[0]     # Extract first character
        command = arduino_command[1:]   # Extract remainder
        command = command.strip()
        result = ""

        # ! => raw shell command
        if cmd_id == '!':
            result = execute_command(command)

        # @ => macros
        elif cmd_id == '@':
            if command in macros:
                result = execute_command(macros[command])
            elif command == "list_macros":
                macro_list = [f'    "{m}": "{macros[m]}"' for m in macros]
                result = "Registered Macros:\n" + "\n".join(macro_list)
            elif command.startswith("add_macro:"):
                _, macro_name, macro_cmd = command.split(":")
                create_macro(macro_name, macro_cmd, macros)
                result = f"Macro '{macro_name}' created with command '{macro_cmd}'"
            elif command.startswith("delete_macro:"):
                _, macro_name = command.split(":")
                result = delete_macro(macro_name, macros)
            else:
                result = f"unrecognized macro command: @{command}"
                print(result + '\n')
                cmd_serial.write(result.encode('utf-8') + b'\n')
                continue

        # & => compile + upload
        elif cmd_id == '&':
            result = compile_and_upload(command)

        # # => just a text line
        elif cmd_id == '#':
            result = command

        else:
            result = f"unrecognized cmd_id: {cmd_id}"
            print(result + '\n')
            cmd_serial.write(result.encode('utf-8') + b'\n')
            continue

        # Output results to console and serial
        lines = result.split('\n')
        if lines and not lines[-1].strip():
            lines.pop()  # Remove any empty trailing line

        for line in lines:
            print(line)
            cmd_serial.write(line.encode('utf-8') + b'\n')


if __name__ == '__main__':
    run()

