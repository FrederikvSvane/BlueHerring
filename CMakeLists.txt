cmake_minimum_required(VERSION 3.15...3.31)

project(blueherring
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  

include_directories(.)

# add_library(myLibExample Foo.cpp Foo.h)

add_executable(blueherring main.cpp board_t.hpp file_util.hpp operators_util.hpp piece_t.hpp square_t.hpp)

# target_link_libraries(blueherring PRIVATE)