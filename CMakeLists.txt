cmake_minimum_required(VERSION 3.17)
project(kbi)
cmake_minimum_required(VERSION 3.15)



set(CMAKE_CXX_STANDARD 20)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-4.0)
# look into FindPkgConfig.cmake,
# it contains documentation
# Now the variables GTKMM_INCLUDE_DIRS, GTKMM_LIBRARY_DIRS and GTKMM_LIBRARIES
# contain what you expect
pkg_check_modules(PORTAUDIO2 portaudio-2.0)

add_subdirectory(src)
