testParser: testParser.o circuitParser.o
	gcc $^ -o $@
test.o: testParser.cpp
	gcc -c $<
circuitParser.o: circuitParser.cpp
	gcc -c $<
