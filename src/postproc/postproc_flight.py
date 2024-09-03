# System modules
import sys
import numpy as np
import matplotlib.pyplot as plt
import pathlib

#-----------------------------------------------------------------------------#

def load_mc(outputPath):

    subdirs = [subdir for subdir in outputPath.iterdir() if subdir.is_dir()]
    telem   = [None]*len(subdirs)

    for iRun, subdir in enumerate(subdirs):

        filePath    = subdir / "telem.csv"
        telem[iRun] = load_csv(filePath)

    return telem

def load_csv(filePath):

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

# def postproc(filePath):

#     (data, fields, units) = process(filePath)

#     ## BUILD PLOTS

#     fields.remove("time")
#     units.remove("s")

#     # Set reasonable xlim
#     #iPos = np.where(data["linPosZ"] <= 0)
#     #iVel = np.where(data["linVelZ"] <  0)
#     #iGnd = np.intersect1d(iPos, iVel)[0]
#     #iGnd = iVel[0][-1]
#     iGnd = len(data["time"]) - 1

#     for field in fields:

#         fig, ax = plt.subplots()

#         ax.plot(data["time"][:iGnd+1], data[field][:iGnd+1])

#         ax.set_title(field)
#         ax.set_xlabel("Time [s]")
#         #ax.set_ylabel(field + " [" + unit + ']')

#         ax.xaxis.grid()
#         ax.yaxis.grid()

#         ax.set_xlim(data["time"][0], data["time"][iGnd])

#     print("apogee = " + str(data["linPosZ"].max()))

#     plt.show()
