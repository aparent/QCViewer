CC = g++
CFLAGS = -g -O0 `pkg-config --define-variable=prefix=/home/aparent/local --cflags cairo`
LDFLAGS = `pkg-config --define-variable=prefix=/home/aparent/local --libs cairo` -Wl,-rpath,/home/aparent/local/lib
#FLAGS = -O3

LIB_FILES = circuitParser.cpp gate.cpp circuit.cpp utility.cpp draw.cpp TFCLexer.o



# All source files have associated object files
LIBOFILES		= $(LIB_FILES:%.cpp=%.o)

# all is the default rule
all	: libQC.a

# remove the old tapestry library and remake the new one

libQC.a:	$(LIBOFILES)
	rm -f $@
	ar cq $@ $(LIBOFILES)

testParser: testParser.o circuitParser.o gate.o circuit.o utility.o TFCLexer.o 
	${CC} ${CFLAGS} $^ -o  $@
testParser.o: testParser.cpp
	${CC} ${CFLAGS} -c  $<
circuitParser.o: circuitParser.cpp
	${CC} ${CFLAGS} -c  $<
gate.o: gate.cpp
	${CC} ${CFLAGS} -c  $<
circuit.o: circuit.cpp
	${CC} ${CFLAGS} -c  $<
utility.o: utility.cpp
	${CC} ${CFLAGS} -c  $<
draw.o: draw.cpp
	${CC} ${CFLAGS} -c  $<
TFCLexer.o: TFCLexer.cpp
	${CC} ${CFLAGS} -c  $<
TFCLexer.cpp: TFCLexer.l
	flex  $<


clean:
	rm *.o *.a
