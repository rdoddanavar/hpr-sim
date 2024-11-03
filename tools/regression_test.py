import os
import difflib
import filecmp
import pathlib
import shutil
import argparse

def regression_test(dirCmp1: pathlib.Path, dirCmp2: pathlib.Path, dirOut: pathlib.Path) -> bool:

    if os.path.exists(dirOut):
        shutil.rmtree(dirOut)

    os.mkdir(dirOut)

    # Compare files
    dcmp   = filecmp.dircmp(dirCmp1, dirCmp2)
    isDiff = False

    for file in dcmp.diff_files:
        isDiff += diff_files(dirCmp1 / file, dirCmp2 / file, dirOut)

    # Compare subdirectories; assume one (1) nested level

    for subdir in dcmp.subdirs:

        os.mkdir(dirOut / subdir)

        for file in dcmp.subdirs[subdir].diff_files:
            isDiff += diff_files(dirCmp1 / subdir / file, dirCmp2 / subdir / file, dirOut / subdir)

    # Pass test if no file differences
    return not(isDiff)

def diff_files(filePath1, filePath2, dirOut) -> bool:

    # Burn first line to skip time stamp (always expected to differ)

    with open(filePath1, 'r') as file1:
        lines1 = file1.read().splitlines()[1:]

    with open(filePath2, 'r') as file2:
        lines2 = file2.read().splitlines()[1:]

    # Compare file contents

    if lines1 == lines2:

        isDiff = False

    elif filePath1.suffix == ".csv":

        # Files are too large for difflib to process in a reasonable time
        print(f"Regression: {filePath1.as_posix()} --> {filePath2.as_posix()}")
        print(f"Diff: <diff too large>")
        print()
        isDiff = True

    else:

        # Generate HTML diff report
        diff = difflib.HtmlDiff().make_file(lines1, lines2, "Before", "After")

        diffPath = (dirOut / filePath1.stem).as_posix() + ".html"

        with open(diffPath, 'w') as diffFile:
            diffFile.write(diff)

        print(f"Regression: {filePath1.as_posix()} --> {filePath2.as_posix()}")
        print(f"Diff: {diffPath}")
        print()

        isDiff = True

    return isDiff

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("dirCmp1", type=str, help="Compare directory 1")
    parser.add_argument("dirCmp2", type=str, help="Compare directory 2")
    parser.add_argument("dirOut" , type=str, help="Output directory"   )

    args = parser.parse_args()

    dirCmp1 = pathlib.Path(args.dirCmp1)
    dirCmp2 = pathlib.Path(args.dirCmp2)
    dirOut  = pathlib.Path(args.dirOut )

    passTest = regression_test(dirCmp1, dirCmp2, dirOut)
    assert passTest, f"Regression test failed! See artifacts at: {dirOut.resolve().as_posix()}"
