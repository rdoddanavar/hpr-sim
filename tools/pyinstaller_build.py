import os
import shutil
import pathlib
import PyInstaller.__main__

# PyInstaller setup
name       = "hpr-sim"
workPath   = pathlib.Path("build/pyinstaller")
distPath   = workPath / "dist"
outputPath = distPath / "hpr-sim/output"

# Set import search paths
paths = ["src/exec", "src/gui", "src/preproc", "src/postproc", "src/util"]

# Set binary files and bundled locations: ("filePath", "location")

if os.name == "posix":
    binaries = [("build/src/model.cpython-310-x86_64-linux-gnu.so", "build/src")]
elif os.name == "nt":
    binaries = [("build/src/model.cp310-win_amd64.pyd", "build/src")]

# Set data files and bundled locations: ("filePath", "location")
datas = [("build/CMakeCache.txt"    , "build" ), 
         ("config/config_input.yml" , "config"), 
         ("config/config_output.yml", "config"), 
         ("config/config_unit.yml"  , "config")]

# Excluded modules from bundle
excludes = ["PySide2", "PySide6", "PyQt6"] # Using PyQt5; Qt bindings conflict with each other

# Execute PyInstaller commands
PyInstaller.__main__.run([
    f"{name}.py",
    "--workpath",
    workPath.resolve().as_posix(),
    "--distpath",
    distPath.resolve().as_posix(),
    "--noconfirm",
    "--paths",
    paths[0],
    "--paths",
    paths[1],
    "--paths",
    paths[2],
    "--paths",
    paths[3],
    "--paths",
    paths[4],
    "--add-binary",
    f"{binaries[0][0]}:{binaries[0][1]}",
    "--add-data",
    f"{datas[0][0]}:{datas[0][1]}",
    "--add-data",
    f"{datas[1][0]}:{datas[1][1]}",
    "--add-data",
    f"{datas[2][0]}:{datas[2][1]}",
    "--add-data",
    f"{datas[3][0]}:{datas[3][1]}",
    "--exclude-module",
    f"{excludes[0]}",
    "--exclude-module",
    f"{excludes[1]}",
    "--exclude-module",
    f"{excludes[2]}",
    "--hidden-import",
    "scipy.special._special_ufuncs",
    "--hidden-import",
    "scipy._lib.array_api_compat.numpy.fft"
])

outputPath.mkdir(parents=True, exist_ok=True)
shutil.copytree("input", distPath / "hpr-sim/input")
