cmake_minimum_required(VERSION 3.16)

project(boolean_3d)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS NO)

add_subdirectory(c_utils)
add_subdirectory(minimal_window)
find_library(LIB_MATH m)
find_package(OpenMP REQUIRED)

#
# Main library
#
add_library(boolean_3d_lib INTERFACE
    include/deduplicate_stl_verts.h
    include/plane.h
    include/ray.h
    include/ray_plane_intersection.h
    include/ray_triangle_intersection.h
    include/thread_safe_dynamic_array.h
    include/triangle.h
    include/vec3.h
 "include/aabb.h" "include/aabb_triangle_intersection.h")
target_include_directories(boolean_3d_lib INTERFACE include)
target_link_libraries(boolean_3d_lib INTERFACE c_utils)
set_target_properties(boolean_3d_lib PROPERTIES
    LANGUAGE C
    CMAKE_C_STANDARD 99
    CMAKE_C_STANDARD_REQUIRED YES
    CMAKE_C_EXTENSIONS NO
)

#
# Raytraced Triangle
#
add_executable(raytraced_triangle raytraced_triangle.c)
target_link_libraries(raytraced_triangle PRIVATE minimal_window boolean_3d_lib)

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
target_link_libraries(boolean_3d PRIVATE c_utils boolean_3d_lib)

if(LIB_MATH)
    target_link_libraries(boolean_3d PRIVATE ${LIB_MATH})
endif()