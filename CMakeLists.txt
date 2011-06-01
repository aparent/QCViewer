cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0011 OLD)
project (viewer)

ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

SET(QCVIEWER_SRC_FILES
	draw.cpp
	main.cpp
	window.cpp
	circuitwidget.cpp
	stateView.cpp
	drawState.cpp
	stateWidget.cpp
)

SET(QCVIEWER_HEADER_FILES
	draw.h
	main.h
	window.h
	circuitwidget.h
	stateView.h
	drawState.h
	stateWidget.h
)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")


find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-2.4)

include_directories(${GTKMM_INCLUDE_DIRS}  ${CMAKE_SOURCE_DIR}/../QCLib)
LINK_DIRECTORIES(${GTKMM_LIBRARY_DIRS} ${CMAKE_SOURCE_DIR}/../QCLib)

SOURCE_GROUP("Header Files" FILES ${QCVIEWER_HEADER_FILES})
SOURCE_GROUP("Source Files" FILES ${QCVIEWER_SRC_FILES})

add_executable (QCViewer  ${QCVIEWER_SRC_FILES})
target_link_libraries(QCViewer ${GTKMM_LIBRARIES} ${CMAKE_SOURCE_DIR}/../QCLib/libQC.a )
