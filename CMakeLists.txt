cmake_minimum_required(VERSION 3.10)
project(xtest)
set(CMAKE_C_STANDARD 11)#C11
set(CMAKE_CXX_STANDARD 17)#C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE INCLUDES "graph/*.h")
file(GLOB_RECURSE SOURCES  "graph/*.c")

message(WARNING "include ${INCLUDES}")
message(WARNING "source ${SOURCES}")

add_compile_options("-g")

add_definitions("-DDEVICE_X11")

include_directories(./graph)
include_directories(./graph/x11)
include_directories(./INSTALL/include/freetype2/)

link_directories(./INSTALL/lib)
add_executable(xtest fttest.c
                     display.c
                    ${SOURCES}
                    ${INCLUDES})
target_link_libraries(xtest -lX11 libfreetype.a)

