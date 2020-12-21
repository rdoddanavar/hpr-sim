import sys
import pdb

sys.path.insert(0, "../src/util/")

import util_unit

util_unit.config()

x = util_unit.convert(0,"temperature","C","F")
print(x)

pdb.set_trace()