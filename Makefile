CXXFLAGS     = -g -O2 -Wall -fmessage-length=0 -I. -L.
# -g -fprofile-arcs -ftest-coverage
LSOURCES     := $(wildcard parse/*.cpp)
BSOURCES     := $(wildcard src/*.cpp)
TSOURCES     := $(wildcard test/*.cpp)
LOBJECTS     := $(LSOURCES:.cpp=.o)
BOBJECTS     := $(BSOURCES:.cpp=.o)
TOBJECTS     := $(TSOURCES:.cpp=.o)
LDEPS        := $(LSOURCES:.cpp=.d)
BDEPS        := $(BSOURCES:.cpp=.d)
TDEPS        := $(TSOURCES:.cpp=.d)
GTEST        := ../googletest
GTEST_I      := -I$(GTEST)/include
GTEST_L      := -L$(GTEST)
LTARGET      = libparse.a
BTARGET      = pgen
TTARGET      = test_parse

all: lib pgen

lib: $(LTARGET)

test: lib pgen $(TTARGET)

check: test
	./$(TTARGET)

$(LTARGET): $(LOBJECTS)
	ar rvs $(LTARGET) $(LOBJECTS)

parse/%.o: parse/%.cpp
	$(CXX) $(CXXFLAGS) -c -MMD -o $@ $<

$(TTARGET): $(TOBJECTS) test/gtest_main.o
	$(CXX) $(CXXFLAGS) $(GTEST_L) $^ -pthread -lparse -lstdcore -lgtest -o $(TTARGET)

test/%.o: test/%.cpp
	$(CXX) $(CXXFLAGS) $(GTEST_I) $< -c -MMD -o $@

$(BTARGET): $(BOBJECTS) $(LTARGET)
	$(CXX) $(CXXFLAGS) $(BOBJECTS) -o $(BTARGET) -lparse

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -MMD -o $@ $<

test/gtest_main.o: $(GTEST)/src/gtest_main.cc
	$(CXX) $(CXXFLAGS) $(GTEST_I) $< -c -o $@

-include $(LDEPS)
-include $(BDEPS)
-include $(TDEPS)

clean:
	rm -f parse/*.o test/*.o src/*.o
	rm -f parse/*.d test/*.d src/*.d
	rm -f parse/*.gcda test/*.gcda src/*.gcda
	rm -f parse/*.gcno test/*.gcno src/*.gcno
	rm -f $(LTARGET) $(BTARGET) $(TTARGET)
