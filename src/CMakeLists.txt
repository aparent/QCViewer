cmake_minimum_required (VERSION 2.6)
project(QCViewer)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-2.4 freetype2 poppler-glib)


add_subdirectory(QCViewer)
