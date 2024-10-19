import sys
import os
import pathlib
from datetime import datetime

# Module variables
timestamp = None

#------------------------------------------------------------------------------#

def is_bundled():

    # https://pyinstaller.org/en/stable/runtime-information.html#run-time-information

    if getattr(sys, 'frozen', False) and hasattr(sys, '_MEIPASS'):
        return True
    else:
        return False

#------------------------------------------------------------------------------#

def get_cmake_cache(field):

    subdir   = "_internal" if is_bundled() else "build"
    filePath = pathlib.Path(subdir) / "CMakeCache.txt"

    with open(filePath, 'r') as cacheFile:

        lines = cacheFile.read().split("\n")

        for line in lines:
            if field in line:
                value = line.split('=')[1]
                return value

#------------------------------------------------------------------------------#

def pybind11_setup():

    if os.name == "nt":

        if is_bundled():

            # Add path to pybind11 module(s)
            sys.path.append(pathlib.Path("_internal").resolve().as_posix())

        else:

            # Add path to libstdc++
            compilerPath = get_cmake_cache("CMAKE_CXX_COMPILER")
            os.add_dll_directory(pathlib.Path(compilerPath).parent)

#------------------------------------------------------------------------------#

def qt_setup():

    if os.name == "posix":

        # Fix for interaction between Qt5 & Wayland
        os.environ["QT_QPA_PLATFORM"] = "xcb"

    elif os.name == "nt":
        pass

#------------------------------------------------------------------------------#

def get_version():
    return get_cmake_cache("CMAKE_PROJECT_VERSION")

#------------------------------------------------------------------------------#

def set_timestamp():

    global timestamp
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

#------------------------------------------------------------------------------#

def get_timestamp():

    global timestamp

    if timestamp is None:
        set_timestamp()

    return timestamp

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    # Standalone execution
    pass
