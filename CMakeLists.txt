cmake_minimum_required (VERSION 2.6)
project (QCLib)

ADD_DEFINITIONS( "-ggdb3 -Wall -Wextra -pedantic -ansi  -Wno-unused-function " )
#ADD_DEFINITIONS( "-O2 -Wall -Wextra -pedantic -ansi" )
#ADD_DEFINITIONS( "-O3 -Wall -pedantic -ansi" )

SET(QCLIB_SRC_FILES
	circuit.cpp
	diracLexer.cpp
	TFCLexer.cpp
	utility.cpp
	circuitParser.cpp
	dirac.cpp
	diracParser.cpp
	gate.cpp
  state.cpp
  state.cpp
  simulate.cpp
	gates/UGate.cpp
	gates/RGate.cpp
	gates/UGateLookup.cpp
	QCLang/QCLParser.cpp
	QCLang/QCLLexer.cpp
	QCLang/QCLParserUtils.cpp
	QCLang/REPLInterperater.cpp
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
		OUTPUT ${CMAKE_SOURCE_DIR}/TFCLexer.cpp
		COMMAND flex ${CMAKE_SOURCE_DIR}/TFCLexer.l
		DEPENDS ${CMAKE_SOURCE_DIR}/TFCLexer.l
	)
	add_custom_command (
		OUTPUT ${CMAKE_SOURCE_DIR}/diracLexer.cpp
		COMMAND flex ${CMAKE_SOURCE_DIR}/diracLexer.l
		DEPENDS ${CMAKE_SOURCE_DIR}/diracLexer.l
	)
	add_custom_command (
		OUTPUT ${CMAKE_SOURCE_DIR}/diracParser.cpp ${CMAKE_SOURCE_DIR}/diracParser.h
		COMMAND bison -d -v -o diracParser.cpp ${CMAKE_SOURCE_DIR}/diracParser.y
		DEPENDS ${CMAKE_SOURCE_DIR}/diracParser.y
	)
	add_custom_command (
		OUTPUT ${CMAKE_SOURCE_DIR}/QCLang/QCLLexer.cpp
		COMMAND flex ${CMAKE_SOURCE_DIR}/QCLang/QCLLexer.l
		DEPENDS ${CMAKE_SOURCE_DIR}/QCLang/QCLLexer.l
	)
	add_custom_command (
		OUTPUT ${CMAKE_SOURCE_DIR}/QCLang/QCLParser.cpp
		COMMAND bison ${CMAKE_SOURCE_DIR}/QCLang/QCLParser.y
		DEPENDS ${CMAKE_SOURCE_DIR}/QCLang/QCLParser.y
	)
ENDIF(UNIX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

include_directories(${CMAKE_SOURCE_DIR})
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR})

SOURCE_GROUP("Header Files" FILES ${QCLIB_HEADER_FILES})
SOURCE_GROUP("Source Files" FILES ${QCLIB_SRC_FILES})


ADD_LIBRARY(QC STATIC ${QCLIB_HEADER_FILES} ${QCLIB_SRC_FILES})
install (TARGETS QC DESTINATION lib)

#Some testing stuff, uncomment to use
#add_executable (diracTest  diracTest.cpp )
#target_link_libraries(diracTest QC )
#add_executable (simTest  simTest.cpp )
#target_link_libraries(simTest QC )
#add_executable (testParser  testParser.cpp )
#target_link_libraries(testParser QC )
add_executable (QCLang/QCLTest  QCLang/QCLTest.cpp )
target_link_libraries(QCLang/QCLTest QC )
#add_executable (testParser testParser.cpp)
#target_link_libraries(testParser QCLib)
