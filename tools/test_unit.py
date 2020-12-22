import sys
import pdb
from pathlib import Path

srcPath = Path(__file__).parent / "../src/util"
srcPath = str(srcPath.resolve())

#sys.path.insert(0, "../src/util/")
sys.path.append(srcPath)

import util_unit

util_unit.config()

x = util_unit.convert(0,"temperature","C","F")
print(x)
