CC = g++

testParser: testParser.o circuitParser.o
	${CC} $^ -o $@
testParser.o: testParser.cpp
	${CC} -c $<
circuitParser.o: circuitParser.cpp
	${CC} -c $<

clean:
	rm *.o
