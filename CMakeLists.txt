cmake_minimum_required(VERSION 3.16)

project(boolean_3d)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS NO)

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Ofast -Wall -Wextra -march=native -mtune=native)
endif()

add_subdirectory(minimal_window)
find_library(LIB_MATH m)
find_package(OpenMP REQUIRED)

#
# Raytraced Triangle
#
add_executable(raytraced_triangle raytraced_triangle.c)
target_link_libraries(raytraced_triangle PRIVATE minimal_window)

if(LIB_MATH)
    target_link_libraries(raytraced_triangle PRIVATE ${LIB_MATH})
endif()

if(OpenMP_C_FOUND)
    target_link_libraries(raytraced_triangle PRIVATE OpenMP::OpenMP_C)
endif()

#
# Boolean3D
#
add_executable(boolean_3d boolean_3d.c)

if(LIB_MATH)
    target_link_libraries(boolean_3d PRIVATE ${LIB_MATH})
endif()