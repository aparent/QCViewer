CC = g++
CFLAGS = -g -O0 `pkg-config --define-variable=prefix=/users/j3parker/local --cflags cairo`
LDFLAGS = `pkg-config --define-variable=prefix=/users/j3parker/local --libs cairo` -Wl,-rpath,/users/j3parker/local/lib
#FLAGS = -O3

testParser: testParser.o circuitParser.o gate.o circuit.o draw.o
	${CC} ${LDFLAGS} $^ -o $@
testParser.o: testParser.cpp
	${CC} ${CFLAGS} -c $<
circuitParser.o: circuitParser.cpp
	${CC} ${CFLAGS} -c $<
gate.o: gate.cpp
	${CC} ${CFLAGS} -c $<
circuit.o: circuit.cpp
	${CC} ${CFLAGS} -c $<
draw.o: draw.cpp
	${CC} ${CFLAGS} -c $<

clean:
	rm *.o
