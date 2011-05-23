CC = g++
CFLAGS = -g -O0 `pkg-config --cflags cairo`
LDFLAGS = `pkg-config --libs cairo`
#FLAGS = -O3

HEADERS := dirac/parseNode.h
OBJDIR := obj
OBJS := $(addprefix $(OBJDIR)/,circuitParser.o gate.o circuit.o utility.o draw.o TFCLexer.o dirac.o diracParser.o diracLexer.o)
LIBOFILES	:= $(addprefix $(OBJDIR)/,circuitParser.o gate.o circuit.o utility.o draw.o TFCLexer.o dirac.o)

all	:$ $(OBJDIR) libQC.a
libQC.a:	$(LIBOFILES)
	rm -f $@
	ar cq $@ $(LIBOFILES)

$(OBJDIR)/%.o: %.cpp
	${CC} -c ${CFLAGS}  $< -o $@
TFCLexer.cpp: TFCLexer.l
	flex  $<
diracLexer.cpp: diracLexer.l
	flex  -o $@ $<
diracParser.cpp: diracParser.y
	bison -d -o $@ $<
diracParser.h: diracParser.y
	bison -d -o $@ $<

$(OBJDIR):
	mkdir $(OBJDIR)

testParser: draw.o testParser.o circuitParser.o gate.o circuit.o utility.o TFCLexer.o
	${CC} ${LDFLAGS} $^ -o  $@

testDirac: diracTest.o dirac.o diracParser.o diracLexer.o
	${CC} ${LDFLAGS} $^ -o  $@

clean:
	rm obj/*.o *.a TFCLexer.cpp diracLexer.cpp diracParser.cpp diracParser.h
