cmake_minimum_required (VERSION 2.6)
project (viewer)

#ADD_DEFINITIONS( "-O2 -Wall -Wextra -ansi -pedantic" )
#ADD_DEFINITIONS( "-ggdb3 -ansi" )
ADD_DEFINITIONS( "-O3 -ansi" )

SET(QCVIEWER_SRC_FILES
	draw.cpp
	main.cpp
	window.cpp
	circuitwidget.cpp
	stateWidget.cpp
	GateIcon.cpp
  ConsoleWidget.cpp
)

SET(QCVIEWER_HEADER_FILES
	draw.h
	main.h
	window.h
	circuitwidget.h
	stateWidget.h
	GateIcon.h
  ConsoleWidget.h
)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")


find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-2.4 freetype2)

include_directories(${GTKMM_INCLUDE_DIRS}  ${CMAKE_SOURCE_DIR}/../QCLib)
LINK_DIRECTORIES(${GTKMM_LIBRARY_DIRS} ${CMAKE_SOURCE_DIR}/../QCLib)

SOURCE_GROUP("Header Files" FILES ${QCVIEWER_HEADER_FILES})
SOURCE_GROUP("Source Files" FILES ${QCVIEWER_SRC_FILES})

add_executable (viewer  ${QCVIEWER_SRC_FILES})
target_link_libraries(viewer ${GTKMM_LIBRARIES} ${CMAKE_SOURCE_DIR}/../QCLib/libQC.a )
