cmake_minimum_required (VERSION 2.6)
project (QCLib)

ADD_DEFINITIONS( "-W -Wall -ansi -pedantic" )

SET(QCLIB_SRC_FILES
	circuit.cpp
	diracLexer.cpp
	draw.cpp
	TFCLexer.cpp
	utility.cpp
	circuitParser.cpp
	dirac.cpp
	diracParser.cpp
	gate.cpp
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
)

IF(UNIX)
	add_custom_command (
		OUTPUT ${CMAKE_SOURCE_DIR}/TFCLexer.cpp
		COMMAND flex ${CMAKE_CURRENT_BINARY_DIR}/TFCLexer.l
	)
	add_custom_command (
		OUTPUT ${CMAKE_SOURCE_DIR}/diracLexer.cpp
		COMMAND flex ${CMAKE_CURRENT_BINARY_DIR}/diracLexer.l
	)
	add_custom_command (
		OUTPUT ${CMAKE_SOURCE_DIR}/diracParser.cpp ${CMAKE_SOURCE_DIR}/diracParser.h
		COMMAND bison -d -o diracParser.cpp ${CMAKE_CURRENT_BINARY_DIR}/diracParser.y
	)
ENDIF(UNIX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

FIND_PACKAGE(Cairo REQUIRED)

include_directories(${CAIRO_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
LINK_DIRECTORIES(${CAIRO_LIBRARY_DIRS} )

SOURCE_GROUP("Header Files" FILES ${QCLIB_HEADER_FILES})
SOURCE_GROUP("Source Files" FILES ${QCLIB_SRC_FILES})

ADD_LIBRARY(QCLib STATIC ${QCLIB_HEADER_FILES} ${QCLIB_SRC_FILES})
target_link_libraries(QCLib cairo)
install (TARGETS QCLib  DESTINATION lib)

add_executable (diracTest  diracTest.cpp )
target_link_libraries(diracTest QCLib )
#add_executable (testParser testParser.cpp)
#target_link_libraries(testParser QCLib)
