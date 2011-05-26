cmake_minimum_required (VERSION 2.6)
project (QCViewer)

ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

SET(QCVIEWER_SRC_FILES
	draw.cpp
	main.cpp
	window.cpp
	circuitwidget.cpp
)

SET(QCVIEWER_HEADER_FILES
	draw.h
	main.h
	window.h
	circuitwidget.h
)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")


find_package(GTK2 2.10 REQUIRED gtkmm)

include_directories(${GTKMM_INCLUDE_DIRS}  ${CMAKE_SOURCE_DIR}/../QCLib)
LINK_DIRECTORIES(${GTKMM_LIBRARY_DIRS} ${CMAKE_SOURCE_DIR}/../QCLib)

SOURCE_GROUP("Header Files" FILES ${QCVIEWER_HEADER_FILES})
SOURCE_GROUP("Source Files" FILES ${QCVIEWER_SRC_FILES})

IF(UNIX)
	add_executable (QCViewer  ${QCVIEWER_SRC_FILES})
	target_link_libraries(QCViewer ${GTKMM_LIBRARIES} ${CMAKE_SOURCE_DIR}/../QCLib/libQC.a )
ELSE(UNIX)
	add_executable (QCViewer  ${QCVIEWER_SRC_FILES})
	target_link_libraries(QCViewer ${GTKMM_LIBRARIES} ${CMAKE_SOURCE_DIR}/../QCLib/Release/QC.lib )
ENDIF(UNIX)
