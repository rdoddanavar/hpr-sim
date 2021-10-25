# System modules
import numpy as np

# Path modifications

# Project modules

#------------------------------------------------------------------------------#

# Module variables

# distArgs = {"beta": (2, []),
# #"binomial": (,),
# "chisquare": (,),
# "dirichlet": (,),
# "exponential": (,),
# "f": (,),
# "gamma": (,),
# "geometric": (,),
# "gumbel": (,),
# "hypergeometric": (,),
# "laplace": (,),
# "logistic": (,),
# "lognormal": (,),
# "logseries": (,),
# "multinomial": (,),
# "multivariate_hypergeometric": (,),
# "multivariate_normal": (,),
# "negative_binomial": (,),
# "noncentral_chisquare": (,),
# "noncentral_f": (,),
# "normal": (,),
# "pareto": (,),
# "poisson": (,),
# "power": (,),
# "rayleigh": (,),
# "standard_cauchy": (,),
# "standard_exponential": (,),
# "standard_gamma": (,),
# "standard_normal": (,),
# "standard_t": (,),
# "triangular": (,),
# "uniform": (,),
# "vonmises": (,),
# "wald": (,),
# "weibull": (,),
# "zipf": (,),
# }

distAvailable = {"none"    :           [],
                 "normal"  : [True, True],
                 "uniform" : [True, True]}

#------------------------------------------------------------------------------#

# Validate dist definition in master inputDict

def check_dist(inputDict):

    for group in inputDict.keys():

        for param in inputDict[group].keys():

            props = inputDict[group][param].keys()

            if "dist" in props:

                distName  = inputDict[group][param]["dist"]["name"]
                distParam = inputDict[group][param]["dist"]["param"]

                if distName == "none":
                    continue

                if distName not in distAvailable.keys():
                    raise ValueError("Random distribution choice not valid", distName)

                if len(distParam) != len(distAvailable[distName]):
                    raise ValueError("Number of random distribution parameters incorrect", distName, len(distParam))

#------------------------------------------------------------------------------#

def mc_draw(inputDictRun):

    seedRun = inputDictRun["exec"]["seed"]["value"]

    # Initialize RNG
    philox = np.random.Philox(seedRun)
    rng    = np.random.Generator(philox)

    for group in inputDictRun.keys():

        for param in inputDictRun[group].keys():

            props = inputDictRun[group][param].keys()

            if "dist" in props:

                distName  = inputDictRun[group][param]["dist"]["name"]
                distParam = inputDictRun[group][param]["dist"]["param"]

                if distName == "none":
                    continue

                # # Convert units of distribution parameters, if necessary
                # quantity = 
                # unit     = 

                # for iParam in range(len(distParam)):
                #     if distAvailable[distName]:
                #         distParam[iParam] = util_unit.convert(distParam[iParam], quantity, unit)

                # Assign value from random draw
                rngFun  = getattr(rng, distName)
                rngEval = rngFun(*distParam)
                inputDictRun[group][param]["value"] = rngEval

#------------------------------------------------------------------------------#