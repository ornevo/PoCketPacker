#!/usr/bin/python
"""
Created by Or Nevo Michrowski.
This script serves as a GUI for the PoCketPacker program.
"""

from GUI.GUIHelpers import *
from GUI.PackerLauncher import launch

# Global widgets
# A label holding the path of the selected file to compress
chosen_file_label = None
log_box = None  # The box containing the PoCketPacker output
status_label = None  # A label holding the run status


def main():
    window = Tk()
    window.title(PROG_NAME)
    window.geometry(WINDOW_SIZE)
    window.resizable(0, 0)

    build_form(window, launch)

    window.mainloop()


if __name__ == '__main__':
    main()
