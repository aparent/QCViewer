cmake_minimum_required (VERSION 2.6)
project (QCLib)

ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

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
ENDIF(UNIX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

include_directories(${CMAKE_SOURCE_DIR})
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR})

SOURCE_GROUP("Header Files" FILES ${QCLIB_HEADER_FILES})
SOURCE_GROUP("Source Files" FILES ${QCLIB_SRC_FILES})


ADD_LIBRARY(QC STATIC ${QCLIB_HEADER_FILES} ${QCLIB_SRC_FILES})
install (TARGETS QC DESTINATION lib)

add_executable (diracTest  diracTest.cpp )
target_link_libraries(diracTest QC )
#add_executable (testParser testParser.cpp)
#target_link_libraries(testParser QCLib)
