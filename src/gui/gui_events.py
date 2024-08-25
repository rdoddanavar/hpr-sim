import pathlib
import functools
from PyQt5.QtWidgets import(
    QFileDialog
)

def get_file_action(parent, target):
    return functools.partial(get_file, parent, target)

def get_file(parent, target):
    
    fileName = QFileDialog.getOpenFileName(parent, "Select Input File", pathlib.Path(".").resolve().as_posix(), "Input Files (*.yml *.yaml)")
    target.setText(fileName[0])

def get_directory_action(parent, target):
    return functools.partial(get_directory, parent, target)

def get_directory(parent, target):
    
    directory = QFileDialog.getExistingDirectory(parent, "Select Output Directory", pathlib.Path(".").resolve().as_posix())
    target.setText(directory)

if __name__ == "__main__":
    pass
