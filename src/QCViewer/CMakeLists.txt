#ADD_DEFINITIONS( "-O2 -Wall -Wextra -ansi -pedantic" )
#ADD_DEFINITIONS( "-ggdb3 -ansi" )
ADD_DEFINITIONS( "-O0 -ansi" )
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-2.4 freetype2)

file(GLOB QCVIEWER_SRC_FILES *.cpp)
file(GLOB QCVIEWER_HEADER_FILES *.h)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")



include_directories(${GTKMM_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTKMM_LIBRARY_DIRS})

add_executable (viewer  ${QCVIEWER_SRC_FILES})
target_link_libraries(viewer ${GTKMM_LIBRARIES} ${CMAKE_SOURCE_DIR}/../QCLib/libQC.a )
install(FILES ${QCLIB_HEADER_FILES} DESTINATION include)
