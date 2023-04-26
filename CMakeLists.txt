cmake_minimum_required(VERSION 3.16)

project(Boolean3D)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS NO)

add_subdirectory(minimal_window)

add_executable(boolean3d main.c)
target_link_libraries(boolean3d PRIVATE minimal_window)

find_library(LIB_MATH m)

if(LIB_MATH)
    target_link_libraries(boolean3d PRIVATE ${LIB_MATH})
endif()
