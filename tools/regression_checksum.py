import argparse
import pathlib

def regression_checksum(dirIn: pathlib.Path, dirOut: pathlib.Path) -> None:
    
    subdirs = [subdir for subdir in dirIn.iterdir() if subdir.is_dir()]
    telem   = [None]*len(subdirs)

    for iRun, subdir in enumerate(subdirs):
        for item in subdir.iterdir():
            if item.name == "telem.npy":
                print(item)

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("dirIn" , type=str, help="Input directory" )
    parser.add_argument("dirOut", type=str, help="Output directory")

    args = parser.parse_args()

    dirIn  = pathlib.Path(args.dirCmp1)
    dirOut = pathlib.Path(args.dirOut )

    regression_checksum(dirIn, dirOut)
