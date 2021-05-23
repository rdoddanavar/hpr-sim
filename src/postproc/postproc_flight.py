# System modules
import sys
import numpy as np
import matplotlib.pyplot as plt

#-----------------------------------------------------------------------------#

def postproc(filePath):

    ## READ OUTPUT CSV

    with open(filePath, 'r') as file:
        lines = file.read().splitlines()
    
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

    #-------------------------------------------------------------------------#

    ## BUILD PLOTS

    fig, ax = plt.subplots(2, 2)
    ax = [ax[0][0], ax[0][1], ax[1][0], ax[1][1]]

    fields.remove("time")
    units.remove("s")

    # Set reasonable xlim
    iPos = np.where(data["linPosZ"] <= 0)
    iVel = np.where(data["linVelZ"] <  0)
    iGnd = np.intersect1d(iPos, iVel)[0]

    iGnd = iVel[0][-1]

    for iAx in range(len(ax)):

        ax[iAx].plot(data["time"][:iGnd+1], data[fields[iAx]][:iGnd+1])

        ax[iAx].set_title(fields[iAx])
        ax[iAx].set_xlabel("time [s]")
        ax[iAx].set_ylabel(fields[iAx] + " [" + units[iAx] + ']')

        ax[iAx].xaxis.grid()
        ax[iAx].yaxis.grid()

        ax[iAx].set_xlim(data["time"][0], data["time"][iGnd])

    print("apogee = " + str(data["linPosZ"].max()))

    plt.show()

if __name__ == "__main__":

    filePath  = sys.argv[1]
    postproc(filePath)