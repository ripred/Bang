"""
arduino_exec.py

@brief Python Agent for the ArduinoCLI platform. This script allows
communication with Arduino boards, enabling the execution of built-in
commands, macros, and compilation/upload of Arduino code.

@author Trent M. Wyatt
@date 2023-12-10
@version 1.2

Release Notes:
1.2 - added support for compiling, uploading and replacing the
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
"""

import subprocess
import logging
import signal
import serial
import json
import sys
import os

# A list of abbreviated commands that the Arduino
# can send to run a pre-registered command:
macros = {}

# The logger
logger = None


def setup_logger():
    """
    @brief Set up the logger for error logging.

    Configures a logger to log errors to both the console and a file.

    @return None
    """
    global logger

    # Set up logging configuration
    logging.basicConfig(level=logging.ERROR)  # Set the logging level to ERROR

    file_handler = logging.FileHandler(
            os.path.join(os.path.abspath(os.path.dirname(__file__)),
                         'arduino_exec.log'))
    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
    file_handler.setFormatter(formatter)

    logger = logging.getLogger(__name__)
    logger.setLevel(logging.ERROR)  # Set the logging level to ERROR
    logger.addHandler(file_handler)


def get_args():
    """
    @brief Get the serial port from the command line.

    Checks if a serial port argument is provided in the command line.

    @return str: The serial port obtained from the command line.
    """
    if len(sys.argv) <= 1:
        print("Usage: python arduino_exec.py <COM_port>")
        exit(-1)

    return sys.argv[1]


def sigint_handler(signum, frame):
    """
    @brief Signal handler for SIGINT (Ctrl+C).

    Handles the SIGINT signal (Ctrl+C) to save macros and exit gracefully.

    @param signum: Signal number
    @param frame: Current stack frame

    @return None
    """
    print(" User hit ctrl-c, exiting.")
    save_macros(macros)
    sys.exit(0)


def set_signal_handler():
    """
    @brief Set the signal handler for SIGINT.

    Sets the signal handler for SIGINT (Ctrl+C) to sigint_handler.

    @return None
    """
    signal.signal(signal.SIGINT, sigint_handler)


def open_serial_port(port):
    """
    @brief Open the specified serial port.

    Attempts to open the specified serial port with a timeout of 1 second.

    @param port: The serial port to open.

    @return serial.Serial: The opened serial port.

    @exit If the serial port cannot be opened,
          the program exits with an error message.
    """
    cmd_serial = serial.Serial(port, 9600, timeout=1)

    if not cmd_serial:
        print(f"Could not open the serial port {port}")
        exit(0)

    print(f"Succesfully opened serial port {port}")
    return cmd_serial


def execute_command(command):
    """
    @brief Execute a command and capture the output.

    Executes a command using subprocess and captures the output.
    If an error occurs, logs the error and returns an error message.

    @param command: The command to execute.

    @return str: The output of the command or an error message.
    """
    print(f"Executing: {command}")  # Output for the user

    try:
        result = subprocess.check_output(command, shell=True,
                                         stderr=subprocess.STDOUT)
        return result.decode('utf-8')
    except subprocess.CalledProcessError as e:
        return f"Error executing command: {e}"
    except Exception as e:
        errtxt = f"An unexpected error occurred: {e}"
        logger.error(errtxt)
        return errtxt


def load_macros(filename='macros.txt'):
    """
    @brief Load macros from a file.

    Attempts to load macros from a specified file.
    If the file is not found, returns an empty dictionary.

    @param filename: The name of the file containing
                     macros (default: 'macros.txt').

    @return dict: The loaded macros.
    """
    try:
        with open(filename, 'r') as file:
            return json.load(file)
    except FileNotFoundError:
        return {}


def save_macros(macros, filename='macros.txt'):
    """
    @brief Save macros to a file.

    Saves the provided macros to a specified file.

    @param macros: The macros to save.
    @param filename: The name of the file to save macros
                     to (default: 'macros.txt').

    @return None
    """
    with open(filename, 'w') as file:
        json.dump(macros, file, indent=4, sort_keys=True)


def create_macro(name, command, macros):
    """
    @brief Create a new macro.

    Creates a new macro with the given name and command, and saves it.

    @param name: The name of the new macro.
    @param command: The command associated with the new macro.
    @param macros: The dictionary of existing macros.

    @return None
    """
    macros[name] = command
    save_macros(macros)


def read_macro(name, macros):
    """
    @brief Read the command associated with a macro.

    Retrieves the command associated with a given macro name.

    @param name: The name of the macro.
    @param macros: The dictionary of existing macros.

    @return str: The command associated with the macro or an error message.
    """
    return macros.get(name, "Macro not found")


def execute_macro(name, macros):
    """
    @brief Execute a macro.

    Executes the command associated with a given macro name.

    @param name: The name of the macro.
    @param macros: The dictionary of existing macros.

    @return str: The output of the macro command or an error message.
    """
    if name in macros:
        return execute_command(macros[name])
    else:
        return f"Macro '{name}' not found"


def delete_macro(name, macros):
    """
    @brief Delete a macro.

    Deletes the specified macro and saves the updated macro list.

    @param name: The name of the macro to delete.
    @param macros: The dictionary of existing macros.

    @return str: Confirmation message or an error message if the
                 macro is not found.
    """
    if name in macros:
        del macros[name]
        save_macros(macros)
        return f"Macro '{name}' deleted"
    else:
        return f"Macro '{name}' not found"


# Define constant part of the compile and upload commands
COMPILE_COMMAND_BASE = 'arduino-cli compile --fqbn arduino:avr:nano'
UPLOAD_COMMAND_BASE = 'arduino-cli upload -p /dev/cu.usbserial-41430 --fqbn arduino:avr:nano:cpu=atmega328old'


def compile_and_upload(folder):
    """
    @brief Compile and upload Arduino code.

    Compiles and uploads Arduino code from the specified folder.

    @param folder: The folder containing the Arduino project.

    @return str: Result of compilation and upload process.
    """
    # Check if the specified folder exists
    if not os.path.exists(folder):
        return f"Error: Folder '{folder}' does not exist."

    # Check if the folder contains a matching .ino file
    ino_file = os.path.join(folder, f"{os.path.basename(folder)}.ino")
    if not os.path.isfile(ino_file):
        return f"Error: Folder '{folder}' does not contain a matching .ino file."

    compile_command = f'{COMPILE_COMMAND_BASE} {folder}'
    upload_command = f'{UPLOAD_COMMAND_BASE} {folder}'

    compile_result = execute_command(compile_command)
    upload_result = execute_command(upload_command)

    result = f"Compile Result:\n{compile_result}\nUpload Result:\n{upload_result}"

    return result


def run():
    """
    @brief Main execution function.

    Handles communication with Arduino, waits for commands, and executes them.

    @return None
    """
    global macros

    port = get_args()
    cmd_serial = open_serial_port(port)
    set_signal_handler()
    macros = load_macros()
    setup_logger()

    prompted = False
    while True:
        if not prompted:
            print("Waiting for a command from the Arduino...")
        prompted = True

        arduino_command = cmd_serial.readline().decode('utf-8').strip()
        arduino_command = arduino_command.strip()

        if not arduino_command:
            continue

        logtext = f"Received command from Arduino: '{arduino_command}'"
        print(logtext)
        logger.info(logtext)

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
                result = f"unrecognized macro command: @{command}"
        elif cmd_id == '!':
            # Dispatch the command to handle built-in commands
            result = execute_command(command)
        elif cmd_id == '&':
            # Dispatch the compile and avrdude upload
            result = compile_and_upload(command)
        else:
            result = f"unrecognized cmd_id: {cmd_id}"

        for line in result.split('\n'):
            print(line + '\n')
            cmd_serial.write(line.encode('utf-8') + b'\n')

        prompted = False


if __name__ == '__main__':
    run()
