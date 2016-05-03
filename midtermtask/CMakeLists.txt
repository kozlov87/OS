project(midtermsh)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Task.h Split.h)

add_executable(midtermsh ${SOURCE_FILES})
