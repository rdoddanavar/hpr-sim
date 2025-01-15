# System modules
import pathlib
import functools
from PySide6.QtWidgets import(
    QLabel,
    QPushButton,
    QSpinBox,
    QFileDialog,
    QSizePolicy
)

from PySide6.QtCore import Qt
import pyqtgraph as pg

#------------------------------------------------------------------------------#

def pyqtgraph_setup():

    pg.setConfigOptions(background='w')
    pg.setConfigOptions(foreground='k')
    pg.setConfigOptions(antialias=True)

#------------------------------------------------------------------------------#

class Label(QLabel):

    # QLabel w/ desired default settings

    def __init__(self, text):

        super().__init__(text)

        # Center text
        self.setAlignment(Qt.AlignCenter)

class PushButton(QPushButton):

    # QPushButton w/ desired default settings

    def __init__(self, text):

        super().__init__(text)

        # Automatic rescaling in both dimensions
        self.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)

class SpinBox(QSpinBox):

    # QSpinBox w/ desired default settings

    def __init__(self, min, max):

        super().__init__()

        self.setMinimum(min)
        self.setMaximum(max)

    spinBoxMax = 2147483647 # Enforced by QSpinBox.setMaximum

#------------------------------------------------------------------------------#

def action_get_file(parent, target):
    return functools.partial(get_file, parent, target)

def get_file(parent, target):

    fileName = QFileDialog.getOpenFileName(parent, "Select Input File", pathlib.Path(".").resolve().as_posix(), "Input Files (*.yml *.yaml)")
    target.setText(fileName[0])

#------------------------------------------------------------------------------#

def action_get_directory(parent, target):
    return functools.partial(get_directory, parent, target)

def get_directory(parent, target):

    directory = QFileDialog.getExistingDirectory(parent, "Select Output Directory", pathlib.Path(".").resolve().as_posix())
    target.setText(directory)

#------------------------------------------------------------------------------#

if __name__ == "__main__":
    pass
