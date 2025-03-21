import sys
import pathlib
import argparse
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

# Path modifications
paths = ["build/src", "src/exec", "src/gui", "src/preproc", "src/postproc", "src/util"]

for item in paths:
    addPath = pathlib.Path(__file__).parent.parent / item
    sys.path.append(addPath.resolve().as_posix())

import postproc_flight

def regression_plot(dirCmp1: pathlib.Path, dirCmp2: pathlib.Path, dirOut: pathlib.Path) -> None:

    telem1 = postproc_flight.load_dir(dirCmp1)
    telem2 = postproc_flight.load_dir(dirCmp2)

    pdfOut = PdfPages((dirOut / "plot_reg.pdf").resolve())
    fields = telem1[0]["data"].keys()

    for field in fields:

        if field == "time":
            continue

        fig, ax = plt.subplots()

        for run in telem1:
            x = run["data"]["time"]
            y = run["data"][field]
            lines1 = ax.plot(x, y, color='b')

        lines1[0].set_label("before")

        for run in telem2:
            x = run["data"]["time"]
            y = run["data"][field]
            lines2 = ax.plot(x, y, color='r')

        lines2[0].set_label("after")
        unit = telem1[0]["units"][field]
        ax.set_xlabel("time [s]")
        ax.set_ylabel(f"{field} [{unit}]")
        ax.set_title(f"{dirCmp1} -> {dirCmp2}: {field}")
        ax.legend()

        pdfOut.savefig(fig)

    pdfOut.close()

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("dirCmp1", type=str, help="Compare directory 1")
    parser.add_argument("dirCmp2", type=str, help="Compare directory 2")
    parser.add_argument("dirOut" , type=str, help="Output directory"   )

    args = parser.parse_args()

    dirCmp1 = pathlib.Path(args.dirCmp1)
    dirCmp2 = pathlib.Path(args.dirCmp2)
    dirOut  = pathlib.Path(args.dirOut )

    regression_plot(dirCmp1, dirCmp2, dirOut)
