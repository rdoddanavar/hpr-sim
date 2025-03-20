# System modules
import pathlib
import numpy as np
import scipy
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

# Project modules
import util_yaml

#------------------------------------------------------------------------------#

def plot_pdf(outputPath: pathlib.Path) -> None:

    telem  = load_dir(outputPath)
    fields = telem[0]["data"].keys()
    pdfOut = PdfPages(outputPath / "plots.pdf")

    for field in fields:

        if field == "time":
            continue

        fig, ax = plt.subplots()

        for run in telem:
            x = run["data"]["time"]
            y = run["data"][field]
            ax.plot(x, y, color='b')

        unit = telem[0]["units"][field]

        ax.set_xlabel("time [s]")
        ax.set_ylabel(f"{field} [{unit}]")
        ax.set_title(f"{outputPath.stem}: {field}")

        pdfOut.savefig(fig)

    # Cleanup
    pdfOut.close()
    plt.close('all')

#------------------------------------------------------------------------------#

def load_dir(outputPath: pathlib.Path) -> list[dict]:

    subdirs = [subdir for subdir in sorted(outputPath.iterdir()) if subdir.is_dir()]
    telem   = [None]*len(subdirs)

    for iRun, subdir in enumerate(subdirs):
        for item in sorted(subdir.iterdir()):
            if item.name == "telem.npy":
                telem[iRun] = load_npy(item)

    return telem

#------------------------------------------------------------------------------#

def load_npy(npyPath: pathlib.Path) -> dict:

    statsPath = npyPath.parent / "stats.yml"

    # Load stats dict to get field names and units
    stats  = util_yaml.load(statsPath)
    fields = list(stats.keys())
    units  = {}

    for field in fields:
        unit = stats[field]["unit"]
        unit = '' if unit is None else unit
        units[field] = unit

    # Load binary *.npy data (2D float array)
    npyArr = np.load(npyPath)
    data   = {}

    for iCol, field in enumerate(fields):
        data[field] = npyArr[:, iCol]

    # Get metadata from stats file

    with open(statsPath, 'r') as statsFile:
        lines = statsFile.read().splitlines()

    meta = {
        "datetime" : lines[0].strip("# "),
        "version"  : lines[1].strip("# "),
        "run"      : lines[2].strip("# "),
    }

    # Pack telemetry dict for single run
    telem = {
        "meta"   : meta ,
        "data"   : data ,
        "units"  : units,
    }

    return telem

#------------------------------------------------------------------------------#

def export_csv(outputPath: pathlib.Path, telemPrec: int) -> None:

    subdirs = [subdir for subdir in sorted(outputPath.iterdir()) if subdir.is_dir()]
    telem   = load_dir(outputPath)

    for iRun in range(len(telem)):

        csvPath = subdirs[iRun] / "telem.csv"
        fields  = telem[iRun]["data"].keys()

        nRow   = len(telem[iRun]["data"]["time"])
        nCol   = len(fields)
        csvArr = np.zeros((nRow, nCol))

        for iCol, field in enumerate(fields):
            csvArr[:,iCol] = telem[iRun]["data"][field]

        headerStr = "# " + telem[iRun]["meta"]["datetime"] + "\n" \
                    "# " + telem[iRun]["meta"]["version"]  + "\n" \
                    "# " + telem[iRun]["meta"]["run"]      + "\n"

        headerStr += ','.join(fields) + "\n" + ','.join(telem[iRun]["units"].values())

        np.savetxt(csvPath, csvArr, fmt=f"%.{telemPrec}f", delimiter=',', header=headerStr, comments='')

#------------------------------------------------------------------------------#

def export_mat(outputPath: pathlib.Path) -> None:

    subdirs = [subdir for subdir in sorted(outputPath.iterdir()) if subdir.is_dir()]
    telem   = load_dir(outputPath)

    for iRun in range(len(telem)):
        matPath = subdirs[iRun] / "telem.mat"
        scipy.io.savemat(matPath, telem[iRun], oned_as="column")

#------------------------------------------------------------------------------#

def npy_to_mat(npyPath: pathlib.Path) -> None:

    telem   = load_npy(npyPath)
    matPath = npyPath.parent / "telem.mat"
    scipy.io.savemat(matPath, telem, oned_as="column")
