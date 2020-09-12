cmake_minimum_required(VERSION 2.8)
project(ece650-prj)


## ECE650 Specific configuration start here
## Project specific configurations go here
add_subdirectory(minisat)
# enable c++11 support
set (CMAKE_CXX_FLAGS "-std=c++11 -pthread -Wall ${CMAKE_CXX_FLAGS}")

# expose minisat header files
include_directories(${CMAKE_SOURCE_DIR}/minisat)

# create the main executable
## add additional .cpp files if needed
add_executable(ece650-prj ece650-prj.cpp)

# link MiniSAT libraries
target_link_libraries(ece650-prj minisat-lib-static)
