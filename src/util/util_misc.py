import pathlib

#------------------------------------------------------------------------------#

def get_cmake_cache(cmakePath, field):

    filePath = pathlib.Path(__file__).parent / cmakePath

    with open(filePath, 'r') as cacheFile:

        lines = cacheFile.read().split("\n")

        for line in lines:
            if field in line:
                value = line.split('=')[1]
                return value

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    # Standalone execution
    pass
