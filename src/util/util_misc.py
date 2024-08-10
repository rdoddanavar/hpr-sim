import sys
import os
import pathlib

#------------------------------------------------------------------------------#

def is_bundled():

    # https://pyinstaller.org/en/stable/runtime-information.html#run-time-information

    if getattr(sys, 'frozen', False) and hasattr(sys, '_MEIPASS'):
        return True
    else:
        return False

#------------------------------------------------------------------------------#

def get_cmake_cache(field):

    subdir   = "_internal" if is_bundled() else "."
    filePath = pathlib.Path(subdir) / "build/CMakeCache.txt"

    with open(filePath, 'r') as cacheFile:

        lines = cacheFile.read().split("\n")

        for line in lines:
            if field in line:
                value = line.split('=')[1]
                return value

#------------------------------------------------------------------------------#

def pybind11_setup():

    if (os.name == "nt") and not(is_bundled()):

        # Explicitly add path to libstdc++
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

if __name__ == "__main__":

    # Standalone execution
    pass
