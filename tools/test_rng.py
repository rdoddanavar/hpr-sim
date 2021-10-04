from numpy.random import Generator, Philox, SeedSequence


sg = SeedSequence(1234)
rg = [Generator(Philox(s)) for s in sg.spawn(10)]

global f

f = 10

breakpoint()