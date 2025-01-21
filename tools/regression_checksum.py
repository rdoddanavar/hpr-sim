import argparse
import pathlib
import hashlib

def regression_checksum(dirIn: pathlib.Path) -> None:
    
    # Generate MD5 checksum for all binary telemetry files (*.npy)
    # Checksum is more efficient to version control for CI testing

    subdirs = [subdir for subdir in sorted(dirIn.iterdir()) if subdir.is_dir()]

    for subdir in subdirs:
        for item in sorted(subdir.iterdir()):

            if item.name == "telem.npy":

                # Get checksum

                with open(item, "rb") as fileIn:

                    hash = hashlib.md5()

                    while chunk := fileIn.read(8192): # Arbitrary chunk size
                        hash.update(chunk)

                # Get metadata

                with open(subdir / "input.yml", 'r') as metaIn:
                    metaData = metaIn.read().splitlines()[:3]
                    metaStr  = "\n".join(metaData)

                # Write metadata & hash string

                with open(subdir / "telem_md5.txt", 'w') as fileOut:
                    fileOut.write(f"{metaStr}\n")
                    fileOut.write(f"{hash.hexdigest()}\n")

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("dirIn" , type=str, help="Input directory" )

    args  = parser.parse_args()
    dirIn = pathlib.Path(args.dirIn)

    regression_checksum(dirIn)
