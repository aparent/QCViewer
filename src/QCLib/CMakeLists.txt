set(CMAKE_CXX_FLAGS "-Wall -pedantic -ansi")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -pedantic -ansi")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -pedantic -ansi")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET(QCLIB_SRC_FILES
	circuit.cpp
	diracLexer.cpp
	TFCLexer.cpp
	utility.cpp
	circuitParser.cpp
	dirac.cpp
	diracParser.cpp
	GateParser.cpp
	GateLexer.cpp
	gate.cpp
  state.cpp
  state.cpp
  simulate.cpp
	gates/UGate.cpp
	gates/RGate.cpp
	gates/UGateLookup.cpp
)

SET(QCLIB_HEADER_FILES
	gate.h
	dirac.h
	circuit.h
	circuitParser.h
	diracParser.h
  parseNode.h
	TFCLexer.h
	utility.h
  state.h
	gates/UGateLookup.h
)

IF(UNIX)
	add_custom_command (
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/TFCLexer.cpp
		COMMAND flex ${CMAKE_CURRENT_SOURCE_DIR}/TFCLexer.l
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/TFCLexer.l
	)
	add_custom_command (
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/diracLexer.cpp
		COMMAND flex ${CMAKE_CURRENT_SOURCE_DIR}/diracLexer.l
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/diracLexer.l
	)
	add_custom_command (
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/diracParser.cpp ${CMAKE_CURRENT_BINARY_DIR}/diracParser.h
		COMMAND bison -d -v -o diracParser.cpp ${CMAKE_CURRENT_SOURCE_DIR}/diracParser.y
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/diracParser.y
	)
	add_custom_command (
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/QCLLexer.cpp
		COMMAND flex ${CMAKE_CURRENT_SOURCE_DIR}/QCLang/QCLLexer.l
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/QCLang/QCLLexer.l
	)
	add_custom_command (
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/QCLParser.cpp ${CMAKE_CURRENT_BINARY_DIR}/QCLParser.h
		COMMAND bison ${CMAKE_CURRENT_SOURCE_DIR}/QCLang/QCLParser.y
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/QCLang/QCLParser.y
	)
	add_custom_command (
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/GateLexer.cpp
		COMMAND flex ${CMAKE_CURRENT_SOURCE_DIR}/gates/GateLexer.l
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gates/GateLexer.l
	)
	add_custom_command (
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/GateParser.cpp ${CMAKE_CURRENT_BINARY_DIR}/GateParser.h
		COMMAND bison ${CMAKE_CURRENT_SOURCE_DIR}/gates/GateParser.y
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gates/GateParser.y
	)
ENDIF(UNIX)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

ADD_LIBRARY(QC STATIC ${QCLIB_HEADER_FILES} ${QCLIB_SRC_FILES})



#Some testing stuff, uncomment to use
#add_executable (diracTest  diracTest.cpp )
#target_link_libraries(diracTest QC )
#add_executable (simTest  simTest.cpp )
#target_link_libraries(simTest QC )
#add_executable (testParser  testParser.cpp )
#target_link_libraries(testParser QC )
#add_executable (QCLang/QCLTest  QCLang/QCLTest.cpp )
#target_link_libraries(QCLang/QCLTest QC )
#add_executable (testParser testParser.cpp)
#target_link_libraries(testParser QCLib)
