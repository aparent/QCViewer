set(CMAKE_CXX_FLAGS "-ansi")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ansi")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -ansi")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-2.4 freetype2)

file(GLOB QCVIEWER_SRC_FILES *.cpp)
file(GLOB QCVIEWER_HEADER_FILES *.h)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/../QCLib )
include_directories( ${QCViewer_SOURCE_DIR}/QCLib)

LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/../QCLib)

include_directories(${GTKMM_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTKMM_LIBRARY_DIRS})

add_executable (viewer  ${QCVIEWER_SRC_FILES})
target_link_libraries(viewer ${GTKMM_LIBRARIES} ${CMAKE_CURRENT_BINARY_DIR}/../QCLib/libQC.a )
install(TARGETS viewer DESTINATION ${QCViewer_BINARY_DIR}/../bin)
