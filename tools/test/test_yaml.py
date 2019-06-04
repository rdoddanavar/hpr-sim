# util_yaml.py testing

import sys
import pdb

sys.path.insert(0, "../../src/util")

import util_yaml

name     = "./test.yaml"
yamlDict = util_yaml.load(name)

yamlDict = util_yaml.processInput(yamlDict)

print(yamlDict)

pdb.set_trace()