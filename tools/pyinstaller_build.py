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
outputPath = distPath / "hpr-sim/output"

# Set import search paths
paths = ["src/exec", "src/gui", "src/preproc", "src/postproc", "src/util"]

# Set binary files and bundled locations: ("filePath", "location")

if os.name == "posix":
    binaries = [("build/src/model.cpython-310-x86_64-linux-gnu.so", ".")]
elif os.name == "nt":
    binaries = [("build/src/model.cp310-win_amd64.pyd", ".")]
    compilerPath = util_misc.get_cmake_cache("CMAKE_CXX_COMPILER")
    libPath      = pathlib.Path(compilerPath).parent / "libstdc++-6.dll"
    binaries    += [(libPath.resolve().as_posix(), ".")]

print(libPath.resolve().as_posix())

# Set data files and bundled locations: ("filePath", "location")
data = [("build/CMakeCache.txt"        , "."), 
        ("src/preproc/config_input.yml", "."), 
        ("src/util/config_unit.yml"    , ".")]

# Excluded modules from bundle
excludes = ["PySide2", "PySide6", "PyQt6"] # Using PyQt5; Qt bindings conflict with each other

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

# Execute PyInstaller
PyInstaller.__main__.run(args)

# Copy necessary directories
outputPath.mkdir(parents=True, exist_ok=True)
shutil.copytree("input", distPath / "hpr-sim/input")
