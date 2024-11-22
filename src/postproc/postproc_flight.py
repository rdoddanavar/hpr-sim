# System modules
import numpy as np
import pathlib
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

# Project modules
import util_yaml

#------------------------------------------------------------------------------#

def plot_pdf(outputPath: pathlib.Path) -> None:

    telem  = load_dir(outputPath)
    fields = telem[0]["fields"]

    pdfOut = PdfPages(outputPath / "plot.pdf")

    for field in fields:

        if field == "time":
            continue

        fig, ax = plt.subplots()

        for run in telem:
            x = run["data"]["time"]
            y = run["data"][field]
            ax.plot(x, y, color='b')

        idx  = fields.index(field)
        unit = telem[0]["units"][idx]

        ax.set_xlabel("time [s]")
        ax.set_ylabel(f"{field} [{unit}]")
        ax.set_title(f"{outputPath.stem}: {field}")

        pdfOut.savefig(fig)

    # Cleanup
    pdfOut.close()
    plt.close('all')

#------------------------------------------------------------------------------#

def load_dir(outputPath: pathlib.Path) -> list[dict]:

    subdirs = [subdir for subdir in outputPath.iterdir() if subdir.is_dir()]
    telem   = [None]*len(subdirs)

    for iRun, subdir in enumerate(subdirs):

        for item in subdir.iterdir():

            if item.stem == "telem":

                if item.suffix == ".csv":
                    telem[iRun] = load_csv(item)
                elif item.suffix == ".npy":
                    telem[iRun] = load_npy(item)

    return telem

#------------------------------------------------------------------------------#

def load_csv(filePath: pathlib.Path) -> dict:

    with open(filePath, 'r') as file:
        lines = file.read().splitlines()

    # Remove comment lines in header
    meta = []

    while lines[0][0] == '#':
        meta.append(lines.pop(0).strip("# "))

    fields = lines[0].split(',')
    units  = lines[1].split(',')
    data   = {}

    nField = len(fields)

    for field in fields:
        data[field] = []

    for line in lines[2:]:

        lineData = line.split(',')

        for iField in range(nField):
            data[fields[iField]].append(float(lineData[iField]))

    for field in fields:
        data[field] = np.array(data[field])

    # Pack telemetry dict for single run
    telem = {
        "meta"   : meta  ,
        "fields" : fields,
        "units"  : units ,
        "data"   : data  ,
    }

    return telem

#------------------------------------------------------------------------------#

def load_npy(npyPath: pathlib.Path) -> dict:

    statsPath = npyPath.parent / "stats.yml"
    
    # Load stats dict to get field names and units
    stats  = util_yaml.load(statsPath)
    fields = list(stats.keys())
    units  = []

    for field in fields:
        units.append(stats[field]["unit"])

    # Load binary *.npy data (2D float array)
    npyArr = np.load(npyPath)
    data   = {}

    for iCol, field in enumerate(fields):
        data[field] = npyArr[:, iCol]

    # Get metadata from stats file
    meta = []

    with open(statsPath, 'r') as statsFile:
        lines = statsFile.read().splitlines()

    while lines[0][0] == '#':
        meta.append(lines.pop(0).strip("# "))

    # Pack telemetry dict for single run
    telem = {
        "meta"   : meta  ,
        "fields" : fields,
        "units"  : units ,
        "data"   : data  ,
    }

    return telem
