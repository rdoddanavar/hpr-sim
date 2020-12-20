import sys
import pdb

sys.path.insert(0, "../src/util/")

import util_unit

util_unit.config()
pdb.set_trace()
x = util_unit.convert(0,"temperature","ft","mi")
print(x)