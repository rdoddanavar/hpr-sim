import sys
from pathlib import Path

# Path modifications
paths = ["../build/src", "../src/preproc", "../src/util"]

for item in paths:
    addPath = Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

#-----------------------------------------------------------------------------#

import util_yaml

yamlDict = util_yaml.load(sys.argv[1])
yamlDict = util_yaml.process(yamlDict)

breakpoint()