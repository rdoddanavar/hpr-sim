# System modules
import numpy as np

# Path modifications

# Project modules

#------------------------------------------------------------------------------#

# Module variables

# distArg = {"beta": (2, []),
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

#------------------------------------------------------------------------------#

# def check_dist(inputDict):
# Validate dist definition in master inputDict

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
                rngFun    = getattr(rng, distName)
                rngEval   = rngFun(*distParam)

                # Assign value from random draw
                inputDictRun[group][param]["value"] = rngEval

#------------------------------------------------------------------------------#