set(CMAKE_CXX_FLAGS "-Wall -pedantic -ansi")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -pedantic -ansi")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -pedantic -ansi")

IF(WIN32)
	set(CMAKE_CXX_FLAGS "-Wall -pedantic")
	set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -pedantic")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -pedantic")
ENDIF(WIN32)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB QCVIEWER_SRC_FILES "*.cpp" "QCLib/*.cpp" "QCLib/gates/*.cpp")
file(GLOB QCVIEWER_HEADER_FILES "*.h" "QCLib/*.h" "QCLib/gates/*.cpp")

LIST(APPEND QCVIEWER_SRC_FILES 
	diracLexer.cpp
	diracParser.cpp
	QCParser.cpp
	QCLexer.cpp
	GateParser.cpp
	GateLexer.cpp
)

IF(NOT WIN32)
add_custom_command (
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/QCParser.cpp ${CMAKE_CURRENT_BINARY_DIR}/QCParser.h
	COMMAND bison ${CMAKE_CURRENT_SOURCE_DIR}/QCLib/QCParser.y
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/QCLib/QCParser.y
)
add_custom_command (
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/QCLexer.cpp
	COMMAND flex ${CMAKE_CURRENT_SOURCE_DIR}/QCLib/QCLexer.l
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/QCLib/QCLexer.l
)
add_custom_command (
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/diracLexer.cpp
	COMMAND flex ${CMAKE_CURRENT_SOURCE_DIR}/QCLib/diracLexer.l
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/QCLib/diracLexer.l
)
add_custom_command (
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/diracParser.cpp ${CMAKE_CURRENT_BINARY_DIR}/diracParser.h
	COMMAND bison -d -v -o diracParser.cpp ${CMAKE_CURRENT_SOURCE_DIR}/QCLib/diracParser.y
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/QCLib/diracParser.y
)
add_custom_command (
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/GateLexer.cpp
	COMMAND flex ${CMAKE_CURRENT_SOURCE_DIR}/QCLib/gates/GateLexer.l
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/QCLib/gates/GateLexer.l
)
add_custom_command (
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/GateParser.cpp ${CMAKE_CURRENT_BINARY_DIR}/GateParser.h
	COMMAND bison ${CMAKE_CURRENT_SOURCE_DIR}/QCLib/gates/GateParser.y
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/QCLib/gates/GateParser.y
)
ENDIF(NOT WIN32)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

add_executable (viewer WIN32  ${QCVIEWER_SRC_FILES})
target_link_libraries(viewer ${GTKMM_LIBRARIES})
install(TARGETS viewer DESTINATION ${QCViewer_SOURCE_DIR}/../bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/../QCLib )
include_directories( ${QCViewer_SOURCE_DIR}/QCLib)


include_directories(${GTKMM_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTKMM_LIBRARY_DIRS})
include_directories(${Freetype_INCLUDE_DIRS})
LINK_DIRECTORIES(${Freetype_LIBRARY_DIRS})


