import os
import difflib
import filecmp
import pathlib
import shutil
import argparse

def regression_test(dirComp1: pathlib.Path, dirComp2: pathlib.Path, dirOut: pathlib.Path) -> None:

    dcmp = filecmp.dircmp(dirComp1, dirComp2)

    if os.path.exists(dirOut):
        shutil.rmtree(dirOut)

    os.mkdir(dirOut)

    # Compare files

    for file in dcmp.diff_files:
        diff_files(dirComp1 / file, dirComp2 / file, dirOut / file)

    # Compare subdirectories; assume one (1) nested level

    for subdir in dcmp.subdirs:

        os.mkdir(dirOut / subdir)

        for file in dcmp.subdirs[subdir].diff_files:
            diff_files(dirComp1 / subdir / file, dirComp2 / subdir / file, dirOut / subdir / file)

def diff_files(filePath1, filePath2, dirOut):

    file1 = open(filePath1, 'r')
    file2 = open(filePath2, 'r')

    # Burn first line to skip time stamp (always expected to differ)
    lines1 = file1.read().splitlines()[1:]
    lines2 = file2.read().splitlines()[1:]

    diff = difflib.HtmlDiff().make_file(lines1, lines2, "Before", "After")

    fileDiff = open(dirOut.as_posix() + ".html", 'w')
    fileDiff.write(diff)
    fileDiff.close()

    print()
    print(f"Regression: {filePath1.as_posix()} --> {filePath2.as_posix()}")
    print(f"Diff: {dirOut.as_posix()}.html")

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("dirComp1", type=str, help="Compare directory 1")
    parser.add_argument("dirComp2", type=str, help="Compare directory 2")
    parser.add_argument("dirOut"  , type=str, help="Output directory"   )

    args = parser.parse_args()

    dirComp1 = pathlib.Path(args.dirComp1)
    dirComp2 = pathlib.Path(args.dirComp2)
    dirOut   = pathlib.Path(args.dirOut  )

    regression_test(dirComp1, dirComp2, dirOut)
