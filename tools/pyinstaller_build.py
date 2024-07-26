import os
import shutil
import pathlib
import PyInstaller.__main__

if os.name == "posix":
    binaries = ["build/src/model.cpython-310-x86_64-linux-gnu.so:build/src"]
elif os.name == "nt":
    binaries = ["build/src/model.cp310-win_amd64.pyd:build/src"]

name       = "hpr-sim"
specFile   = name + ".spec"
workPath   = pathlib.Path("build/pyinstaller")
distPath   = workPath / "dist"
outputPath = distPath / "hpr-sim" / "output"

paths    = ["src/exec", "src/preproc", "src/postproc", "src/util"]
datas    = ["build/CMakeCache.txt:build", "config/config_input.yml:config", "config/config_output.yml:config", "config/config_unit.yml:config"]

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
    "--add-binary",
    binaries[0],
    "--add-data",
    datas[0],
    "--add-data",
    datas[1],
    "--add-data",
    datas[2],
    "--add-data",
    datas[3]
])

outputPath.mkdir(parents=True, exist_ok=True)
shutil.copytree("input", distPath / "hpr-sim" / "input")
