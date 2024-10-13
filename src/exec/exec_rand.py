# System modules
import numpy as np

# Path modifications

# Project modules
import preproc_input
import util_unit

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

distValid = {"none"    : 0,
             "normal"  : 2,
             "uniform" : 2}

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

                if distName not in distValid.keys():
                    raise ValueError("Random distribution choice not valid", distName)

                if len(distParam) != distValid[distName]:
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

                # Execute random draw
                rngFun  = getattr(rng, distName)
                rngEval = rngFun(*distParam)

                # Convert random draw value if necessary

                if "unit" in props:
                    
                    quantity = preproc_input.configInput[group][param]["quantity"]
                    unit     = inputDictRun[group][param]["unit"]

                    if quantity:
                        rngEval = util_unit.convert(rngEval, quantity, unit)

                inputDictRun[group][param]["value"] = rngEval

#------------------------------------------------------------------------------#