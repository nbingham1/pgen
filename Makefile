CXXFLAGS     = -g -O2 -Wall -fmessage-length=0 -I. -L.
# -g -fprofile-arcs -ftest-coverage
BSOURCES     := $(wildcard src/*.cpp)
LSOURCES     := $(wildcard pgen/*.cpp)
LOBJECTS     := $(LSOURCES:.cpp=.o)
BOBJECTS     := $(BSOURCES:.cpp=.o)
LDEPS        := $(LSOURCES:.cpp=.d)
BDEPS        := $(BSOURCES:.cpp=.d)
LTARGET      = libpgen.a
BTARGET      = pgen-linux

all: lib $(BTARGET)

lib: $(LTARGET)

test: lib $(BTARGET) $(TTARGET)

check: test
	./$(TTARGET)

$(LTARGET): $(LOBJECTS)
	ar rvs $(LTARGET) $(LOBJECTS)

$(BTARGET): $(BOBJECTS) $(LTARGET)
	$(CXX) $(CXXFLAGS) $(BOBJECTS) -l:$(LTARGET) -o $(BTARGET)

pgen/%.o: pgen/%.cpp
	$(CXX) $(CXXFLAGS) -c -MMD -o $@ $<

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -MMD -o $@ $<

test/gtest_main.o: $(GTEST)/src/gtest_main.cc
	$(CXX) $(CXXFLAGS) $(GTEST_I) $< -c -o $@

-include $(LDEPS)
-include $(BDEPS)
-include $(TDEPS)

clean:
	rm -f src/*.o pgen/*.o
	rm -f src/*.d pgen/*.d
	rm -f $(LTARGET) $(BTARGET)
