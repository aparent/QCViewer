set(CMAKE_CXX_FLAGS "-Wall -pedantic -ansi")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -pedantic -ansi")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -pedantic -ansi")

IF(WIN32)
	set(CMAKE_CXX_FLAGS "-Wall -pedantic")
	set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -pedantic")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -pedantic")
ENDIF(WIN32)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET(QCLIB_SRC_FILES
	circuit.cpp
	utility.cpp
	dirac.cpp
	diracLexer.cpp
	diracParser.cpp
	circuitWriter.cpp
	QCParser.cpp
	QCLexer.cpp
	QCParserUtils.cpp
	circuit.h
	circuitParser.h
	GateParser.cpp
	GateLexer.cpp
	gate.cpp
  state.cpp
  state.cpp
  simulate.cpp
	subcircuit.cpp
	gates/UGate.cpp
	gates/RGate.cpp
	gates/UGateLookup.cpp
)

SET(QCLIB_HEADER_FILES
	subcircuit.h
	gate.h
	dirac.h
	circuit.h
	circuitParser.h
	QCParserUtils.h
	diracParser.h
  parseNode.h
	QCLexer.h
	utility.h
  state.h
	gates/UGateLookup.h
)

IF(NOT WIN32)
add_custom_command (
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/QCParser.cpp ${CMAKE_CURRENT_BINARY_DIR}/QCParser.h
	COMMAND bison ${CMAKE_CURRENT_SOURCE_DIR}/QCParser.y
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/QCParser.y
)
add_custom_command (
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/QCLexer.cpp
	COMMAND flex ${CMAKE_CURRENT_SOURCE_DIR}/QCLexer.l
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/QCLexer.l
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
ENDIF(NOT WIN32)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

ADD_LIBRARY(QC STATIC ${QCLIB_HEADER_FILES} ${QCLIB_SRC_FILES})



