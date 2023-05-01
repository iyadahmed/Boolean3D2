cmake_minimum_required(VERSION 3.16)

project(Boolean3D)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS NO)

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Ofast -Wall -Wextra -march=native -mtune=native)
endif()

add_subdirectory(minimal_window)

add_executable(boolean3d main.c)
target_link_libraries(boolean3d PRIVATE minimal_window)

find_library(LIB_MATH m)

if(LIB_MATH)
    target_link_libraries(boolean3d PRIVATE ${LIB_MATH})
endif()

find_package(OpenMP)

if(OpenMP_C_FOUND)
    target_link_libraries(boolean3d PRIVATE OpenMP::OpenMP_C)
endif()
