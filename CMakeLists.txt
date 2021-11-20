cmake_minimum_required(VERSION 3.10)
project(freetype_test)
set(CMAKE_C_STANDARD 11)#C11
set(CMAKE_CXX_STANDARD 17)#C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE INCLUDES "driver/*.h")
file(GLOB_RECURSE SOURCES  "driver/*.c")

message(WARNING "include ${INCLUDES}")
message(WARNING "source ${SOURCES}")

add_compile_options("-g")

find_package(SDL2 REQUIRED SDL2)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(./driver/)
include_directories(./INSTALL/include/freetype2/)
include_directories(/d/program/msys64/mingw32/include/)
link_directories(/d/program/msys64/mingw32/bin)
link_directories(./INSTALL/lib)
add_executable(main main.c ${SOURCES} ${INCLUDES})
target_link_libraries(main PRIVATE SDL2 )
target_link_libraries(main PRIVATE libfreetype.a )

