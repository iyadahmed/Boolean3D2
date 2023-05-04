#pragma once

#include <assert.h>

// typedef this to your liking
#define SMALL_ARRAY_TYPE(type, n) \
    struct                        \
    {                             \
        type array[n];            \
        size_t occupied;          \
    }

// Always use this to create your arrays
#define CREATE_SMALL_ARRAY(small_array_type, variable_name) \
    small_array_type variable_name;                         \
    variable_name.occupied = 0;

// Using __VA_ARGS__ to support arguments that contain comma ","
// for example a literal (vec3_t){{0, 0, 0}}
#define PUSH_TO_SMALL_ARRAY(small_array, ...)                                                           \
    assert((small_array)->occupied < (sizeof((small_array)->array) / sizeof((small_array)->array[0]))); \
    (small_array)->array[(small_array)->occupied++] =                                                   \
        __VA_ARGS__;