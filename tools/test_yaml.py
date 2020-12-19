import sys

sys.path.insert(0, "../src/util/")

import util_yaml

testDict = util_yaml.load('../input/input.yaml')

print(util_yaml.process(testDict))