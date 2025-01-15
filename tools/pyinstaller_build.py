import sys
import os
import shutil
import pathlib
import PyInstaller.__main__

utilPath = pathlib.Path(__file__).parent.parent / "src" / "util"
sys.path.append(utilPath.resolve().as_posix())

import util_misc

#------------------------------------------------------------------------------#

# PyInstaller setup
name       = "hpr-sim"
workPath   = pathlib.Path("build/pyinstaller")
distPath   = workPath / "dist"
outputPath = distPath / "hpr-sim" / "output"

# Set import search paths
paths = ["src/exec", "src/gui", "src/preproc", "src/postproc", "src/util"]

# Set binary files and bundled locations: ("filePath", "location")

if os.name == "posix":

    # Bundle pybind11 module
    binaries = [("build/src/model.cpython-312-x86_64-linux-gnu.so", ".")]

elif os.name == "nt":

    # Bundle pybind11 module
    binaries = [("build/src/model.cp312-win_amd64.pyd", ".")]

    # Bundle libstdc++ from mingw64
    compilerPath = util_misc.get_cmake_cache("CMAKE_CXX_COMPILER")
    libPath      = pathlib.Path(compilerPath).parent / "libstdc++-6.dll"
    binaries    += [(libPath.resolve().as_posix(), ".")]

# Set data files and bundled locations: ("filePath", "location")
data = [("build/CMakeCache.txt"        , "."), 
        ("src/preproc/config_input.yml", "."), 
        ("src/util/config_unit.yml"    , ".")]

# Excluded modules from bundle
excludes = ["PySide2", "PyQt5", "PyQt6"] # Using PySide6; Qt bindings conflict with each other

# Hidden imports to resolve python environment inconsistencies
if os.name == "posix":
    hiddenImports = ["scipy.special._special_ufuncs"        ,
                     "scipy._lib.array_api_compat.numpy.fft",
                     "scipy.io"                             ]
elif os.name == "nt":
    hiddenImports = ["scipy.io"]

#------------------------------------------------------------------------------#

# Gather PyInstaller arguments
args = []

args += [f"{name}.py"]
args += ["--noconfirm"]

args += ["--workpath"]
args += [workPath.resolve().as_posix()]
args += ["--distpath"]
args += [distPath.resolve().as_posix()]

for path in paths:
    args += ["--paths"]
    args += [path]

for binInfo in binaries:
    args += ["--add-binary"]
    args += [f"{binInfo[0]}:{binInfo[1]}"]

for dataInfo in data:
    args += ["--add-data"]
    args += [f"{dataInfo[0]}:{dataInfo[1]}"]

for exclude in excludes:
    args += ["--exclude-module"]
    args += [exclude]

for hidden in hiddenImports:
    args += ["--hidden-import"]
    args += [hidden]

# Execute PyInstaller
PyInstaller.__main__.run(args)

#------------------------------------------------------------------------------#

# Copy necessary directories
outputPath.mkdir(parents=True, exist_ok=True)
shutil.copytree("input", distPath / "hpr-sim" / "input")
