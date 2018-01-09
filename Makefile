CXXFLAGS	 =  -g -O2 -Wall -fmessage-length=0 -I. -I../stdcore
# -g -fprofile-arcs -ftest-coverage
SOURCES		 := $(wildcard parse/*.cpp)
TESTS        := $(wildcard test/*.cpp)
OBJECTS		 := $(SOURCES:%.cpp=%.o)
TEST_OBJECTS := $(TESTS:.cpp=.o)
DEPS         := $(OBJECTS:.o=.d)
TEST_DEPS    := $(TEST_OBJECTS:.o=.d)
GTEST        := ../googletest
GTEST_I      := -I$(GTEST)/include -I. -I../stdcore
GTEST_L      := -L$(GTEST) -L. -L../stdcore
TARGET		 = libparse.a
TEST_TARGET  = test_parse

-include $(DEPS)
-include $(TEST_DEPS)

all: lib

lib: $(TARGET)

test: lib $(TEST_TARGET)

check: test
	./$(TEST_TARGET)

$(TARGET): $(OBJECTS)
	ar rvs $(TARGET) $(OBJECTS)

parse/%.o: parse/%.cpp 
	$(CXX) $(CXXFLAGS) -MM -MF $(patsubst %.o,%.d,$@) -MT $@ -c $<
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TEST_TARGET): $(TEST_OBJECTS) test/gtest_main.o
	$(CXX) $(CXXFLAGS) $(GTEST_L) $^ -pthread -lparse -lstdcore -lgtest -o $(TEST_TARGET)

test/%.o: test/%.cpp
	$(CXX) $(CXXFLAGS) $(GTEST_I) -MM -MF $(patsubst %.o,%.d,$@) -MT $@ -c $<
	$(CXX) $(CXXFLAGS) $(GTEST_I) $< -c -o $@
	
test/gtest_main.o: $(GTEST)/src/gtest_main.cc
	$(CXX) $(CXXFLAGS) $(GTEST_I) $< -c -o $@

clean:
	rm -f parse/*.o test/*.o
	rm -f parse/*.d test/*.d
	rm -f parse/*.gcda test/*.gcda
	rm -f parse/*.gcno test/*.gcno
	rm -f $(TARGET) $(TEST_TARGET)
