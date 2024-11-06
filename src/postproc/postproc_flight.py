# System modules
import sys
import numpy as np
import pathlib
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

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

        idx   = fields.index(field)
        units = telem[0]["units"][idx]

        ax.set_xlabel("time [s]")
        ax.set_ylabel(f"{field} [{units}]")
        ax.set_title(f"{outputPath.stem}: {field}")

        pdfOut.savefig(fig)

    pdfOut.close()

#------------------------------------------------------------------------------#

def load_dir(outputPath: pathlib.Path) -> list[dict]:

    subdirs = [subdir for subdir in outputPath.iterdir() if subdir.is_dir()]
    telem   = [None]*len(subdirs)

    for iRun, subdir in enumerate(subdirs):

        filePath    = subdir / "telem.csv"
        telem[iRun] = load_csv(filePath)

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

    telem = {
        "meta"   : meta  ,
        "fields" : fields,
        "units"  : units ,
        "data"   : data  ,
    }

    return telem
