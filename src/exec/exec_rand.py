# System modules

# Path modifications

# Project modules

#------------------------------------------------------------------------------#

# Module variables

distArg = {"beta": (2, []),
#"binomial": (,),
"chisquare": (,),
"dirichlet": (,),
"exponential": (,),
"f": (,),
"gamma": (,),
"geometric": (,),
"gumbel": (,),
"hypergeometric": (,),
"laplace": (,),
"logistic": (,),
"lognormal": (,),
"logseries": (,),
"multinomial": (,),
"multivariate_hypergeometric": (,),
"multivariate_normal": (,),
"negative_binomial": (,),
"noncentral_chisquare": (,),
"noncentral_f": (,),
"normal": (,),
"pareto": (,),
"poisson": (,),
"power": (,),
"rayleigh": (,),
"standard_cauchy": (,),
"standard_exponential": (,),
"standard_gamma": (,),
"standard_normal": (,),
"standard_t": (,),
"triangular": (,),
"uniform": (,),
"vonmises": (,),
"wald": (,),
"weibull": (,),
"zipf": (,),
}

#------------------------------------------------------------------------------#

def mc_draw(rng):

    inputDictRun = copy.deepcopy(inputDict)

    for group in inputDictRun.keys():

        for param in inputDictRun[group].keys():

            props = inputDictRun[group][param].keys()

            if "dist" in props:

                distName  = inputDictRun[group][param]["dist"]["name"]
                distParam = inputDictRun[group][param]["dist"]["param"]
                rngFun    = getattr(rng, distName)
                rngEval   = rngFun(*distParam)

    return inputDictRun

#------------------------------------------------------------------------------#