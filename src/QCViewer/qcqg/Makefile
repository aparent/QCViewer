.PHONY: all clean
CFLAGS=-I. -std=c++11 -Wall -Wextra -g
CIRCUITKIT_OBJS=QCParser.o QCLexer.o circuit.o subcircuit.o gate.o QCParserUtils.o \
								gates/GateParser.o gates/GateLexer.o gates/RGate.o gates/UGate.o \
								gates/UGateLookup.o utility.o qcqg.o

all: qc2quigl

%.cpp: %.l
	flex $^
	mv `basename $@` `dirname $@` 2>/dev/null || true

%.cpp: %.y
	bison $^
	mv `basename $@` `dirname $@` 2>/dev/null || true

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

gates/GateLexer.o: gates/GateParser.cpp

qc2quigl: $(CIRCUITKIT_OBJS)
	$(CXX) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o gates/*.o QCParser.cpp QCLexer.cpp gates/GateParser.cpp gates/GateLexer.cpp qc2quigl

