# System modules
import pathlib
import functools
from PyQt5.QtWidgets import(
    QFileDialog
)

# Project modules
import exec

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

def run_exec_action(lineInput, lineOutput):
    return functools.partial(run_exec,lineInput,lineOutput)

def run_exec(lineInput, lineOutput):
    
    inputPath  = pathlib.Path(lineInput.text())
    outputPath = pathlib.Path(lineOutput.text())
    configPath = pathlib.Path(__file__).parent.parent.parent / "config"
    exec.run(inputPath, outputPath, configPath)

if __name__ == "__main__":
    pass
