# Settings & flags
CXX=g++
CXXFLAGS=-O3 -Wall -shared -std=c++11 -fPIC
PYINCLUDE=`python3 -m pybind11 --includes`
PYSUFFIX=`python3-config --extension-suffix`
SRCDIR=../src/
BINDIR=../bin/

SRC=example.cpp

# Build process
all: input

input: 
	$(CXX) $(CXXFLAGS) $(PYINCLUDE) $(SRCDIR)input.cpp -o $(BINDIR)$@$(PYSUFFIX)

clean:
	cd ../bin/ && rm *