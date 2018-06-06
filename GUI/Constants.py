"""
Created by Or Nevo Michrowski
This file contains constants for the GUI script
"""

# General constants
EXIT_CODE_LOG = "Exit status code: "

# Log box constants
# The amount of characters the selected path
#   can contain without overflow the window
SELECTED_PATH_MAX_CHARS = 57
LOG_BOX_HEIGHT = 10
LOG_BOX_WIDTH = 87

# Location constants
WINDOW_W = 650
WINDOW_H = 375
WINDOW_SIZE = str(WINDOW_W) + "x" + str(WINDOW_H)
PADD_LEFT = 10
CHOOSE_FILE_BUTTON_X = PADD_LEFT + 275
TITLE_Y = 30
CHOOSE_FILE_Y = 60
CHOSEN_FILE_LABEL_Y = 90
LOG_TITLE_Y = 120
LOG_BOX_Y = 150
STATUS_Y = 300
START_BUTTON_Y = 345

# Label constants
PROG_NAME = "PoCketPacker"
STATUS_PREFIX = "Status: "
INIT_STATUS = "Waiting for execution."
CHOOSE_FILE_TITLE = "Choose an executable to pack:"
START_BUTTON_TEXT = "Pack"
CHOOSE_FILE_BUTTON_TEXT = "choose file..."
LOG_TITLE = "Log:"
SELECTED_PREFIX = "Selected: "
DEFAULT_SELECTED_FILE = "None"
TITLE_FONT = ("Arial Bold", 30)
REGULAR_FONT = ("Arial", 14)

# Launching
EXPECTED_PACKER_PATH = "./PoCketPacker"

# Statuses
STAT_RUNNING = "Running..."
STAT_FINISH_SUCCESS = "Finished successfully"
STAT_FINISH_ABORT = "Aborted with error"

# Errors
PACKER_NOT_FOUND_ERR = "ERROR: The PoCketPacker executable was not found in " \
                       + EXPECTED_PACKER_PATH + ".\n" + \
                       "Are you running in the right working directory, " \
                       "\"{LOCAL_PACKER_DIRECTORY}/\"?\n" + \
                       "Current working directory: "
DIDNT_CHOOSE_FILE_ERR = "Please select a file to pack."
