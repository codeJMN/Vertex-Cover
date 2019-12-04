cmake_minimum_required(VERSION 2.8)
project(ece650-a4)

if (EXISTS "${CMAKE_SOURCE_DIR}/minisat")
  add_subdirectory(minisat)
else()
  message (FATAL_ERROR "MiniSat not found. Use `git clone https://github.com/agurfinkel/minisat` to check it out.")
endif()

## ECE650 Specific configuration start here
## Project specific configurations go here

# enable c++11 support
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall -pthread${CMAKE_CXX_FLAGS}")
# set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*, -fuchsia-default-arguments, -google-runtime-references")

# create the main executable
# expose minisat header files
include_directories(${CMAKE_SOURCE_DIR}/minisat)
## add additional .cpp files if needed
#add_executable(ece650-minisat ece650-minisat.cpp)
add_executable(ece650-prj ece650-proj.cpp parse_line.cpp graph.cpp)
# link minisat library with the main executable
target_link_libraries(ece650-prj minisat-lib-static)





