#!/usr/bin/python
"""
Created by Or Nevo Michrowski.
Handles the interaction with the window.
"""

from Tkinter import *
import tkMessageBox
from tkFileDialog import askopenfilename
from ScrolledText import ScrolledText
from Constants import *


# Global
window = None
selected_path = DEFAULT_SELECTED_FILE


def get_selected_file():
    """
    :return: the selected file path
    """
    return selected_path


def set_status(new_status):
    """
    Sets the status label
    :param new_status: the new status
    :return: None
    """
    global status_label
    status_label['text'] = STATUS_PREFIX + new_status


def alert_err(message):
    """
    Pops a message box with message. Returns
    :param message: The message to alert
    :return: None
    """
    tkMessageBox.showerror(PROG_NAME, message)


def log(log_msg):
    """
    Adds a log line to the log box
    :param log_msg: the message to append
    :return: None
    """
    global log_box
    log_box.config(state=NORMAL)
    log_box.insert(INSERT, log_msg)
    log_box.config(state=DISABLED)


def handle_choose_file_click():
    """
    Prompts a file selection dialog and saves the selected file's path
    :return: None
    """
    global chosen_file_label, selected_path
    chosen = selected_path = askopenfilename(title=CHOOSE_FILE_TITLE[:-1])

    # Shorten it to match space by replacing dirs with '...'
    while len(chosen) > SELECTED_PATH_MAX_CHARS:
        # Replace the next directory in the path (after '/home') with '...'
        splt = chosen.split("/")
        if len(splt) <= 2:
            break
        if splt[2] == '...':
            # Needs to have a next directory to replace...
            if len(splt) >= 5:
                chosen = "/".join(splt[:2] + ['...'] + splt[4:])
            else:
                break
        else:
            # Needs to have a next directory to replace...
            if len(splt) >= 4:
                chosen = "/".join(splt[:2] + ['...'] + splt[3:])
            else:
                break

    chosen_file_label['text'] = "Selected: " + chosen


def build_form(window_param, launch_callback):
    """
    This function builds the graphics of the TKinter window
    :param window_param: The window object
    :param launch_callback: a function to be called when clicking "start"
    :return: None
    """
    global chosen_file_label, log_box, status_label

    window = window_param
    # Title
    title = Label(window, text=PROG_NAME, font=TITLE_FONT)
    title.place(relx=0.5, y=TITLE_Y, anchor=CENTER)

    # Choose file title
    choose_file_title = Label(window, text=CHOOSE_FILE_TITLE,
                              font=REGULAR_FONT)
    choose_file_title.place(x=PADD_LEFT, y=CHOOSE_FILE_Y)

    # Choose file button
    choose_file_button = Button(window, text=CHOOSE_FILE_BUTTON_TEXT,
                                command=handle_choose_file_click)
    choose_file_button.place(x=CHOOSE_FILE_BUTTON_X, y=CHOOSE_FILE_Y)

    # Chosen file label
    chosen_file_label = Label(window,
                              text=SELECTED_PREFIX + DEFAULT_SELECTED_FILE,
                              font=REGULAR_FONT)
    chosen_file_label.place(x=PADD_LEFT, y=CHOSEN_FILE_LABEL_Y)

    # Log title
    log_title = Label(window, text=LOG_TITLE, font=REGULAR_FONT)
    log_title.place(x=PADD_LEFT, y=LOG_TITLE_Y)

    # Log text box
    log_box = ScrolledText(window, width=LOG_BOX_WIDTH, height=LOG_BOX_HEIGHT)
    log_box.place(x=PADD_LEFT, y=LOG_BOX_Y)

    # Status
    status_label = Label(window, text=STATUS_PREFIX + INIT_STATUS,
                         font=REGULAR_FONT)
    status_label.place(x=PADD_LEFT, y=STATUS_Y)

    # Start button
    start_button = Button(window, text=START_BUTTON_TEXT,
                          command=launch_callback,
                          font=REGULAR_FONT)
    start_button.place(relx=0.5, y=START_BUTTON_Y, anchor=CENTER)
