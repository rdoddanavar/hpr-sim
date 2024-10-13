import sys
import os
import difflib
import filecmp
import pathlib
import shutil

def regression_test(dir1: pathlib.Path, dir2: pathlib.Path) -> None:

    dcmp = filecmp.dircmp(dir1, dir2)

    outputPath = pathlib.Path("output/diff")

    if os.path.exists(outputPath):
        shutil.rmtree(outputPath)

    os.mkdir(outputPath)

    # Compare files

    for file in dcmp.diff_files:
        diff_files(dir1 / file, dir2 / file, outputPath / file)

    # Compare subdirectories; assume one (1) nested level

    for subdir in dcmp.subdirs:

        os.mkdir(outputPath / subdir)

        for file in dcmp.subdirs[subdir].diff_files:
            diff_files(dir1 / subdir / file, dir2 / subdir / file, outputPath / subdir / file)

def diff_files(filePath1, filePath2, outputPath):

    file1 = open(filePath1, 'r')
    file2 = open(filePath2, 'r')

    # Burn first line to skip time stamp (always expected to differ)
    lines1 = file1.read().splitlines()[1:]
    lines2 = file2.read().splitlines()[1:]

    diff = difflib.HtmlDiff().make_file(lines1, lines2, "Before", "After")

    fileDiff = open(outputPath.as_posix() + ".html", 'w')
    fileDiff.write(diff)
    fileDiff.close()

if __name__ == "__main__":

    dir1 = pathlib.Path("tools/unit_test")
    dir2 = pathlib.Path("output/unit_test")

    regression_test(dir1, dir2)
