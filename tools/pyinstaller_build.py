
#NAME="hpr-sim"
#WORKPATH="build/pyinstaller"
#DISTPATH="build/pyinstaller/dist"
#OPTS="--workpath $WORKPATH --distpath $DISTPATH --noconfirm"
#SPECFILE="$NAME.spec"

#pyinstaller $OPTS $SPECFILE
#cp -rfv input $DISTPATH/$NAME
#mkdir -p $DISTPATH/$NAME/output

import os
import pathlib
import PyInstaller.__main__

if os.name == "posix":
    binaries = ["model.cpython-310-x86_64-linux-gnu.so"]
elif os.name == "nt":
    binaries = ["model.cp310-win_amd64.pyd"]

name     = "hpr-sim"
specFile = name + ".spec"
workPath = pathlib.Path("build/pyinstaller")
distPath = workPath / "dist"
output   = distPath / "output"

PyInstaller.__main__.run([
    f"{name}.py",
    "--no-conirm",
    f"--workpath {workPath.resolve().as_posix()}",
    f"--distpath {distPath.resolve().as_posix()}"
])

os.mkdir(output.resolve().as_posix())
