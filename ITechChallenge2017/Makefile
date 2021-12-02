CXXFLAGS=-O2 -Wall -std=c++11
CXX_EXE=$(patsubst %.cpp,%,$(wildcard *.cpp))

all: time

$(CXX_EXE): %: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

time: *.in

%.in: $(CXX_EXE) force
	@for exe in $(CXX_EXE); do \
	    echo ./$$exe \<$@; \
	    ./$$exe <$@ | diff -qs $(patsubst %.in,%.out,$@) -; \
	    echo; \
	done

force:
.PHONY: all time
