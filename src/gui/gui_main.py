# System modules
import sys
import os
from PySide6.QtWidgets import QApplication
from PySide6.QtGui import QFont

# Project modules
import util_misc
import gui_elements

#------------------------------------------------------------------------------#

def exec():

    util_misc.qt_setup()

    if os.name == "nt":
        sys.argv += ['-platform', 'windows:darkmode=0'] # Disable dark mode PySide6

    app = QApplication(sys.argv)
    font = QFont()
    font.setPointSize(14)
    app.setFont(font)
    window = gui_elements.MainWindow()
    window.show()
    app.exec()

#------------------------------------------------------------------------------#

if __name__ == "__main__":
    pass
