CXXFLAGS=-Wall -std=c++11
EXE=$(patsubst %.cpp,%,$(wildcard *.cpp))
IN=$(sort $(wildcard *.in))
OUT=$(patsubst %.in,%.out,$(IN))
REF=$(patsubst %.in,%.ref,$(IN))

all: diff

clean:
	-$(RM) $(OUT)

realclean: clean
	-$(RM) city

ref: $(REF) city.cpp
	zip ref_$$(date +%Y-%m-%d-%H%M).zip $^

diff: $(OUT)
	@./delta.sh $(patsubst %.out,%,$^)
#	-@for f in $^; do \
		echo "< $$(basename $$f .out).ref > $$f"; \
		diff -s $$(basename $$f .out).ref $$f; \
	done

$(EXE): %: %.cpp
	$(CXX) $(CXXFLAGS) -O2 -o $@ $<

$(EXE).d: %.d: %.cpp
	$(CXX) $(CXXFLAGS) -g -o $@ $<

$(OUT): %.out: %.in city
	time java -jar grandprix_city.jar -exec ./city -novis -map $< | tr -d '\r' >| $@

$(REF): %.ref: %.out
	cp -f $< $@

.PHONY: all clean realclean ref diff
