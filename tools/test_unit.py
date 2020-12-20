import sys

sys.path.insert(0, "../src/util/")

import util_unit

unitDict = util_unit.config()
x = util_unit.convert(5280/2,"length","ft","mi")
print(x)