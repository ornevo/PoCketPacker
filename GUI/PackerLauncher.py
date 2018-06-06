"""
Created by Or Nevo Michrowski.
This file handles the launching of- and bridging the UI and
    the packer CLI output
"""

import os
import subprocess
from Constants import *
from GUIHelpers import *


def launch():
    """
    Starts the packer process
    :param chosen_file_label: the label presenting the selected file
    :return: None
    """
    # Check for PoCketPacker's existence
    if not os.path.isfile(EXPECTED_PACKER_PATH):
        # Alert not found and exit
        working_dir = os.path.dirname(os.path.realpath(__file__))
        alert_err(PACKER_NOT_FOUND_ERR + working_dir)
        exit(1)

    # Get the file to pack, by reading the
    #   "Selected: ..." label from after the "Selected: ".
    file_to_pack = get_selected_file()
    # If didn't choose a file
    if file_to_pack == DEFAULT_SELECTED_FILE:
        alert_err(DIDNT_CHOOSE_FILE_ERR)
        return

    # Launch the packer
    set_status(STAT_RUNNING)
    prc = subprocess.Popen(["./PoCketPacker", file_to_pack],
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                           universal_newlines=True)
    # Read output while process is still running
    for line in iter(prc.stdout.readline, ""):
        log(line)
    for line in iter(prc.stderr.readline, ""):
        log(line)
    # Then read stderr
    prc.stdout.close()
    prc.stderr.close()

    # And finally, print the exit code
    return_code = prc.wait()
    # Since PoCketPacker prints the
    #   code himself if != 0, print only if 0
    if not return_code:
        set_status(STAT_FINISH_SUCCESS)
        log(EXIT_CODE_LOG + str(return_code))
    else:
        set_status(STAT_FINISH_ABORT)
