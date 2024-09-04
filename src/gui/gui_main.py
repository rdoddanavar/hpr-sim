# System modules
from PyQt5.QtWidgets import QApplication
from PyQt5.QtGui import QFont

# Project modules
import util_misc
import gui_elements

#------------------------------------------------------------------------------#

def exec():

    util_misc.qt_setup()

    app = QApplication([])
    font = QFont()
    font.setPointSize(14)
    app.setFont(font)
    window = gui_elements.MainWindow()
    window.show()
    app.exec()

#------------------------------------------------------------------------------#

if __name__ == "__main__":
    pass
