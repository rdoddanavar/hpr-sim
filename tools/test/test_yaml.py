# util_yaml.py testing

import sys
import pdb

sys.path.insert(0, "../../src/util")

import util_yaml

name     = "./test.yml"
yamlDict = util_yaml.load(name)

yamlDict = util_yaml.process(yamlDict, yamlDict)

pdb.set_trace()